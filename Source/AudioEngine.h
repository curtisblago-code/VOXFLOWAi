#pragma once

#include <JuceHeader.h>

struct DeviceStatus
{
    juce::String deviceName;
    double sampleRate = 0.0;
    int bufferSize = 0;
};

class AudioEngine : private juce::AudioIODeviceCallback
{
public:
    AudioEngine();
    ~AudioEngine() override;

    void start();
    void stop();

    void setInputGain(float gainDb);
    void setOutputGain(float gainDb);
    void setBypassed(bool shouldBypass);
    void setSafeModeEnabled(bool enabled);
    void resetProcessing();

    DeviceStatus getDeviceStatus() const;

private:
    void prepareProcessors(double sampleRate, int samplesPerBlock);

    void audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
                               float** outputChannelData, int numOutputChannels,
                               int numSamples) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    juce::AudioDeviceManager deviceManager;

    juce::dsp::ProcessSpec processSpec { 0.0, 0u, 0u };
    juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>, // gentle high-pass safety
        juce::dsp::Compressor<float>,  // tame peaks before AI
        juce::dsp::Gain<float>,        // input trim
        juce::dsp::WaveShaper<float>,  // placeholder "AI" saturation
        juce::dsp::Gain<float>         // output trim
    > processorChain;

    juce::Atomic<float> inputGainDb { 0.0f };
    juce::Atomic<float> outputGainDb { 0.0f };
    juce::Atomic<bool> bypassed { false };
    juce::Atomic<bool> safeMode { false };

    DeviceStatus lastStatus;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
