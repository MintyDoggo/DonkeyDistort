/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DonkeyDistortAudioProcessorEditor::DonkeyDistortAudioProcessorEditor (DonkeyDistortAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize (400, 300);
    // Create a vertical slider with a range of 0.0 to 1.0 and a default value of 0.5
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);

    slider.setRange(0.f, 1.f);
    slider.setValue(1.f);
    //slider.addListener(this); // add a listener to the slider
    slider.setValue(audioProcessor.zaddy_val);

    slider.onValueChange = [this]() -> void
    { 
        audioProcessor.zaddy_val = slider.getValue(); 
        zaddy_label.setText("Value: " + juce::String(audioProcessor.zaddy_val), juce::NotificationType::dontSendNotification);
    };

    zaddy_label.setText("Value: " + juce::String(audioProcessor.zaddy_val), juce::NotificationType::dontSendNotification);
    addAndMakeVisible(zaddy_label);
    addAndMakeVisible(slider);
}

DonkeyDistortAudioProcessorEditor::~DonkeyDistortAudioProcessorEditor()
{
}

//==============================================================================
void DonkeyDistortAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DonkeyDistortAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    slider.setBounds(10, 10, getWidth() - 20, 20);
    zaddy_label.setBounds(10, 40, getWidth() - 20, 20);
}

void DonkeyDistortAudioProcessorEditor::sliderValueChanged(juce::Slider* in_slider)
{
    if (in_slider == &slider)
    {
        audioProcessor.zaddy_val = slider.getValue();
    }
}