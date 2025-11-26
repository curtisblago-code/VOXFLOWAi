#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"

class MainComponent : public juce::Component,
                      private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void initialiseLayout();
    void updateStatusLabels();
    void bindEvents();

    AudioEngine audioEngine;

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label sampleRateLabel;
    juce::Label bufferSizeLabel;

    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::ToggleButton bypassButton;
    juce::ToggleButton safeModeButton;

    juce::TextButton resetButton { "Reset" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
