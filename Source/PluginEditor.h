/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StyleSheet.h"

//==============================================================================
/**
*/
class DonkeyDistortAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::ToggleButton::Listener
{
public:
    DonkeyDistortAudioProcessorEditor (DonkeyDistortAudioProcessor&);
    ~DonkeyDistortAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;

    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::CustomLNF myCustomLNF;
    DonkeyDistortAudioProcessor& audioProcessor;

    juce::Slider drive_slider;
    juce::Slider chaos_slider;
    juce::Slider seed_slider;
    juce::Slider wavetable_slider;
    juce::Slider bitrate_slider;

    juce::ToggleButton chaos_pause;

    juce::Label debug_label;
    juce::ToggleButton debug_button;
    juce::ToggleButton debug_button2;

    juce::ImageButton social_link_button;
    juce::Image background;
    juce::Image myntian_button_image;


    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drive_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chaos_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> seed_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wavetable_slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitrate_slider_attachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> chaos_pause_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DonkeyDistortAudioProcessorEditor)
};
