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

    chaos_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    chaos_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    chaos_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);

    drive_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Drive", drive_slider);
    chaos_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Chaos_Amt", chaos_slider);

    //drive_slider.setRange(0.f, 1.f);
    //drive_slider.setValue(1.f);
    //drive_slider.setValue(audioProcessor.zaddy_val);

    drive_slider.onValueChange = [this]() -> void
    { 
        zaddy_label.setText("Value: " + juce::String(audioProcessor.settings.drive) + " Value: " + juce::String((int)audioProcessor.settings.chaos_mode), juce::NotificationType::dontSendNotification);
    };

    zaddy_label.setText("Value: " + juce::String(audioProcessor.settings.drive) + " Value: " + juce::String((int)audioProcessor.settings.chaos_mode), juce::NotificationType::dontSendNotification);

    // button
    chaos_button.setButtonText("Click me!");
    chaos_button_attachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.get_APVTS(), "Chaos", chaos_button);

    //chaos_button.onClick = [this]() -> void
    //{
    //    audioProcessor.chaos_mode = !audioProcessor.chaos_mode;
    //    repaint();
    //};

    addAndMakeVisible(chaos_button);
    addAndMakeVisible(zaddy_label);
    addAndMakeVisible(drive_slider);
    addAndMakeVisible(chaos_slider);

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
    chaos_slider.setBounds(10, 40, getWidth() - 20, 20);

    chaos_button.setBounds(30, 100, getWidth() - 60, 20);

    zaddy_label.setBounds(10, 40, getWidth() - 20, 20);
}

//void DonkeyDistortAudioProcessorEditor::sliderValueChanged(juce::Slider* in_slider)
//{
//    if (in_slider == &drive_slider)
//    {
//        audioProcessor.zaddy_val = drive_slider.getValue();
//    }
//}