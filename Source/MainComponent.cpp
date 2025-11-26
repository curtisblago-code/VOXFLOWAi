#include "MainComponent.h"

namespace
{
constexpr auto sliderWidth = 200;
}

MainComponent::MainComponent()
{
    initialiseLayout();
    bindEvents();

    audioEngine.start();
    startTimerHz(2);
    setSize(900, 540);
}

MainComponent::~MainComponent()
{
    stopTimer();
}

void MainComponent::initialiseLayout()
{
    titleLabel.setText("VOXFLOWAi - Pre-DAW Vocal Processor", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont({22.0f, juce::Font::bold});
    addAndMakeVisible(titleLabel);

    statusLabel.setJustificationType(juce::Justification::centredLeft);
    sampleRateLabel.setJustificationType(juce::Justification::centredLeft);
    bufferSizeLabel.setJustificationType(juce::Justification::centredLeft);

    addAndMakeVisible(statusLabel);
    addAndMakeVisible(sampleRateLabel);
    addAndMakeVisible(bufferSizeLabel);

    inputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    inputGainSlider.setRange(-24.0, 24.0, 0.1);
    inputGainSlider.setValue(0.0);
    inputGainSlider.setSkewFactorFromMidPoint(0.0);
    inputGainSlider.setTooltip("Trim incoming signal before AI processing");
    addAndMakeVisible(inputGainSlider);

    outputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    outputGainSlider.setRange(-24.0, 24.0, 0.1);
    outputGainSlider.setValue(0.0);
    outputGainSlider.setSkewFactorFromMidPoint(0.0);
    outputGainSlider.setTooltip("Level-match AI output for the DAW/stream");
    addAndMakeVisible(outputGainSlider);

    bypassButton.setButtonText("Bypass processing");
    bypassButton.setTooltip("Route dry signal directly to output");
    addAndMakeVisible(bypassButton);

    safeModeButton.setButtonText("Safe mode (light DSP only)");
    safeModeButton.setTooltip("Reduces CPU load and keeps audio stable when AI is unavailable");
    addAndMakeVisible(safeModeButton);

    resetButton.setTooltip("Reset gains and rebuild the DSP graph");
    addAndMakeVisible(resetButton);
}

void MainComponent::bindEvents()
{
    inputGainSlider.onValueChange = [this]
    {
        audioEngine.setInputGain((float) inputGainSlider.getValue());
    };

    outputGainSlider.onValueChange = [this]
    {
        audioEngine.setOutputGain((float) outputGainSlider.getValue());
    };

    bypassButton.onClick = [this]
    {
        audioEngine.setBypassed(bypassButton.getToggleState());
    };

    safeModeButton.onClick = [this]
    {
        audioEngine.setSafeModeEnabled(safeModeButton.getToggleState());
    };

    resetButton.onClick = [this]
    {
        bypassButton.setToggleState(false, juce::dontSendNotification);
        safeModeButton.setToggleState(false, juce::dontSendNotification);
        inputGainSlider.setValue(0.0);
        outputGainSlider.setValue(0.0);
        audioEngine.resetProcessing();
    };
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::darkorange);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 12.0f, 2.0f);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    titleLabel.setBounds(bounds.removeFromTop(40));

    auto statusArea = bounds.removeFromTop(80);
    statusLabel.setBounds(statusArea.removeFromTop(24));
    sampleRateLabel.setBounds(statusArea.removeFromTop(24));
    bufferSizeLabel.setBounds(statusArea.removeFromTop(24));

    auto controlArea = bounds.removeFromTop(220);

    auto gainRow = controlArea.removeFromTop(100);
    inputGainSlider.setBounds(gainRow.removeFromLeft(sliderWidth));
    outputGainSlider.setBounds(gainRow.removeFromLeft(sliderWidth));

    auto toggleRow = controlArea.removeFromTop(50);
    bypassButton.setBounds(toggleRow.removeFromLeft(220));
    safeModeButton.setBounds(toggleRow.removeFromLeft(260));

    resetButton.setBounds(bounds.removeFromBottom(40).removeFromRight(120));
}

void MainComponent::timerCallback()
{
    updateStatusLabels();
}

void MainComponent::updateStatusLabels()
{
    auto status = audioEngine.getDeviceStatus();
    statusLabel.setText("Device: " + status.deviceName, juce::dontSendNotification);
    sampleRateLabel.setText("Sample rate: " + juce::String(status.sampleRate, 2) + " Hz", juce::dontSendNotification);
    bufferSizeLabel.setText("Buffer size: " + juce::String(status.bufferSize) + " samples", juce::dontSendNotification);
}
