/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DonkeyDistortAudioProcessorEditor::DonkeyDistortAudioProcessorEditor (DonkeyDistortAudioProcessor& p) : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLNF);
    setSize (200, 200);
    // slider
    drive_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    drive_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    drive_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 40);
    drive_slider.addListener(this);

    chaos_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    chaos_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    chaos_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 40);
    chaos_slider.addListener(this);

    seed_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    seed_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    seed_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 40);
    seed_slider.addListener(this);

    wavetable_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    wavetable_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    wavetable_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 40);
    wavetable_slider.setRange(0, audioProcessor.columns - 1);
    wavetable_slider.addListener(this);

    bitrate_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    bitrate_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    bitrate_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 40);
    bitrate_slider.setRange(1.f, 100.f);
    bitrate_slider.setSkewFactorFromMidPoint(10.f);
    bitrate_slider.addListener(this);
    
    chaos_pause.addListener(this);
    debug_button.addListener(this);
    debug_button2.addListener(this);

    drive_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Drive", drive_slider);
    chaos_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Chaos_Amt", chaos_slider);
    seed_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Seed", seed_slider);
    wavetable_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Wavetable", wavetable_slider);
    bitrate_slider_attachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.get_APVTS(), "Bitrate", bitrate_slider);

    chaos_pause_attachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.get_APVTS(), "Chaos_Pause", chaos_pause);

    //chaos_pause.onClick = [this]() 
    //{
    //    //debug_label.setText(juce::String(audioProcessor.testfloat), juce::dontSendNotification);
    //};

    social_link_button.onClick = [this]() 
    {
        juce::URL myntianURL("https://www.patreon.com/Myntian");
        myntianURL.launchInDefaultBrowser();
    };

    addAndMakeVisible(drive_slider);
    addAndMakeVisible(chaos_slider);
    addAndMakeVisible(social_link_button);
    //addAndMakeVisible(seed_slider);
    //addAndMakeVisible(wavetable_slider);
    //addAndMakeVisible(bitrate_slider);

    //addAndMakeVisible(chaos_pause);
    //addAndMakeVisible(debug_button);
    //addAndMakeVisible(debug_button2);
    //addAndMakeVisible(debug_label);
}

DonkeyDistortAudioProcessorEditor::~DonkeyDistortAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void DonkeyDistortAudioProcessorEditor::paint (juce::Graphics& g)
{
    background = juce::ImageCache::getFromMemory(BinaryData::new_background_png, BinaryData::new_background_pngSize);
    myntian_button_image = juce::ImageCache::getFromMemory(BinaryData::MyntianJuceButton_png, BinaryData::MyntianJuceButton_pngSize);

    g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
    social_link_button.setImages(true, false, false, myntian_button_image, 1.f, juce::Colour(), myntian_button_image, 1.f, juce::Colour(0xFF5588EE), myntian_button_image, 1.f, juce::Colour(0xff88BBff));
}

void DonkeyDistortAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    drive_slider.setBounds(10, 20, getWidth() - 20, 20);
    chaos_slider.setBounds(10, 55, getWidth() - 20, 20);
    social_link_button.setBounds(17, 178, 120, 30);
    //social_link_button.setColour(social_link_button.buttonColourId, juce::Colours::transparentWhite);
    //social_link_button.setColour(social_link_button.buttonOnColourId, juce::Colours::transparentWhite);
    //social_link_button.setColour(social_link_button.textColourOffId, juce::Colours::transparentWhite);
    //social_link_button.setColour(social_link_button.textColourOnId, juce::Colours::(0.0f, 0.0f, 0.0f, 0.0f));


    //seed_slider.setBounds(10, 125, (getWidth() - 20) / 2, 20);
    //wavetable_slider.setBounds(10, 90, getWidth() - 20, 20);
    //bitrate_slider.setBounds(100, 125, (getWidth() - 20) / 2, 20);

    //chaos_pause.setBounds(10, 155, getWidth() - 25, 20);
    //debug_label.setBounds(10, 140, getWidth() - 25, 20);
    //debug_button.setBounds(80, 160, getWidth() - 25, 20);
    //debug_button2.setBounds(80, 180, getWidth() - 25, 20);
}

void DonkeyDistortAudioProcessorEditor::sliderValueChanged(juce::Slider* in_slider)
{
    if (in_slider == &drive_slider)
    {
    }
    if (in_slider == &seed_slider)
    {
        //debug_label.setText(juce::String(seed_slider.getValue()), juce::dontSendNotification);
        audioProcessor.generateRandomArray(0.f, 1.f, in_slider->getValue());
    }
    if (in_slider == &wavetable_slider)
    {
        //debug_label.setText(juce::String(wavetable_slider.getValue()), juce::dontSendNotification);
        //debug_label.setText(juce::String(audioProcessor.random_float_value_table[0][wavetable_slider.getValue()]), juce::dontSendNotification);
    }
    if (in_slider == &bitrate_slider)
    {
    }

}

void DonkeyDistortAudioProcessorEditor::buttonClicked(juce::Button* in_button)
{
    if (in_button == &chaos_pause)
    {
        //debug_label.setText(juce::String(audioProcessor.testfloat), juce::dontSendNotification);
        audioProcessor.generateRandomArray(0.f, 1.f, seed_slider.getValue());
    }

    if (in_button == &debug_button)
    {
        //debug_label.setText(juce::String(audioProcessor.random_float_value_table[audioProcessor.settings.wavetable][0]), juce::dontSendNotification);
    }
    if (in_button == &debug_button2)
    {
        //debug_label.setText(juce::String(audioProcessor.random_float_value_table[audioProcessor.settings.wavetable][audioProcessor.columns - 1]), juce::dontSendNotification);
    }
}
