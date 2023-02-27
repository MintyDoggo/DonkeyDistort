/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DonkeyDistortAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DonkeyDistortAudioProcessorEditor (DonkeyDistortAudioProcessor&);
    ~DonkeyDistortAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DonkeyDistortAudioProcessor& audioProcessor;
    juce::Slider slider;
    juce::Label zaddy_label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DonkeyDistortAudioProcessorEditor)
};
