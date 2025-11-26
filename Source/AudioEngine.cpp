#include "AudioEngine.h"

namespace
{
constexpr float safetyHighPassHz = 60.0f;
constexpr float placeholderDrive = 1.6f;
}

AudioEngine::AudioEngine()
{
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    setup.bufferSize = 512;
    setup.sampleRate = 48000.0;

    auto error = deviceManager.initialise(2, 2, nullptr, true, {}, &setup);
    if (error.isNotEmpty())
        juce::Logger::writeToLog("Audio init error: " + error);

    deviceManager.addAudioCallback(this);
}

AudioEngine::~AudioEngine()
{
    deviceManager.removeAudioCallback(this);
}

void AudioEngine::start()
{
    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        prepareProcessors(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());
    }
}

void AudioEngine::stop()
{
    deviceManager.removeAudioCallback(this);
}

void AudioEngine::setInputGain(float gainDb)
{
    inputGainDb = gainDb;
    processorChain.get<2>().setGainDecibels(gainDb);
}

void AudioEngine::setOutputGain(float gainDb)
{
    outputGainDb = gainDb;
    processorChain.get<4>().setGainDecibels(gainDb);
}

void AudioEngine::setBypassed(bool shouldBypass)
{
    bypassed = shouldBypass;
}

void AudioEngine::setSafeModeEnabled(bool enabled)
{
    safeMode = enabled;
}

void AudioEngine::resetProcessing()
{
    setInputGain(0.0f);
    setOutputGain(0.0f);
    bypassed = false;
    safeMode = false;

    prepareProcessors(processSpec.sampleRate, (int) processSpec.maximumBlockSize);
}

DeviceStatus AudioEngine::getDeviceStatus() const
{
    return lastStatus;
}

void AudioEngine::prepareProcessors(double sampleRate, int samplesPerBlock)
{
    if (sampleRate <= 0 || samplesPerBlock <= 0)
        return;

    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    processSpec.numChannels = 2;

    auto& highPass = processorChain.get<0>();
    highPass.state = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, safetyHighPassHz);

    auto& comp = processorChain.get<1>();
    comp.setThreshold(-18.0f);
    comp.setRatio(2.5f);
    comp.setAttack(8.0f);
    comp.setRelease(80.0f);

    processorChain.get<2>().setGainDecibels(inputGainDb.get());

    processorChain.get<3>().functionToUse = [](float x)
    {
        return std::tanh(placeholderDrive * x);
    };

    processorChain.get<4>().setGainDecibels(outputGainDb.get());

    processorChain.prepare(processSpec);

    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        lastStatus.deviceName = device->getName();
        lastStatus.sampleRate = device->getCurrentSampleRate();
        lastStatus.bufferSize = device->getCurrentBufferSizeSamples();
    }
}

void AudioEngine::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                                        float** outputChannelData, int numOutputChannels,
                                        int numSamples)
{
    juce::AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), numInputChannels, numSamples);
    juce::AudioBuffer<float> outputBuffer(outputChannelData, numOutputChannels, numSamples);

    // Copy input to output to maintain channel count before processing.
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        auto* dest = outputBuffer.getWritePointer(channel);
        if (channel < buffer.getNumChannels())
            juce::FloatVectorOperations::copy(dest, buffer.getReadPointer(channel), numSamples);
        else
            juce::FloatVectorOperations::clear(dest, numSamples);
    }

    if (bypassed.load())
        return;

    juce::dsp::AudioBlock<float> block(outputBuffer);

    if (safeMode.load())
    {
        // In safe mode we avoid the waveshaper to minimise CPU.
        auto& chain = processorChain;
        chain.setBypassed<3>(true);
        chain.process(juce::dsp::ProcessContextReplacing<float>(block));
        chain.setBypassed<3>(false);
    }
    else
    {
        processorChain.process(juce::dsp::ProcessContextReplacing<float>(block));
    }
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device == nullptr)
        return;

    prepareProcessors(device->getCurrentSampleRate(), device->getCurrentBufferSizeSamples());
}

void AudioEngine::audioDeviceStopped()
{
    processorChain.reset();
}
