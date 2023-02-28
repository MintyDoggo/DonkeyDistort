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
    // slider
    drive_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    drive_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    drive_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);


    drive_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Drive", drive_slider);
    //drive_slider.setRange(0.f, 1.f);
    //drive_slider.setValue(1.f);
    //drive_slider.setValue(audioProcessor.zaddy_val);

    drive_slider.onValueChange = [this]() -> void
    { 
        //audioProcessor.  = drive_slider.getValue();
        zaddy_label.setText("Value: " + juce::String(audioProcessor.settings.drive) + " Value: " + juce::String((int)audioProcessor.chaos_mode), juce::NotificationType::dontSendNotification);
    };

    zaddy_label.setText("Value: " + juce::String(audioProcessor.settings.drive) + " Value: " + juce::String((int)audioProcessor.chaos_mode), juce::NotificationType::dontSendNotification);

    // button
    chaos_button = new juce::TextButton("My Button");
    chaos_button->setBounds(10, 10, 100, 30);

    chaos_button->onClick = [this]() -> void
    {
        audioProcessor.chaos_mode = !audioProcessor.chaos_mode;
        repaint();
    };

    addAndMakeVisible(chaos_button);
    addAndMakeVisible(zaddy_label);
    addAndMakeVisible(drive_slider);
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
    drive_slider.setBounds(10, 10, getWidth() - 20, 20);
    zaddy_label.setBounds(10, 40, getWidth() - 20, 20);
}

void DonkeyDistortAudioProcessorEditor::sliderValueChanged(juce::Slider* in_slider)
{
    if (in_slider == &drive_slider)
    {
        audioProcessor.zaddy_val = drive_slider.getValue();
    }
}