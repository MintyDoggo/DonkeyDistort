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
    juce::Slider drive_slider;
    juce::Slider chaos_slider;
    juce::Label zaddy_label;
    juce::ToggleButton chaos_button;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drive_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chaos_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> chaos_button_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DonkeyDistortAudioProcessorEditor)
};
