/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings
{
    float drive{ 0.f };
    float chaos{ 0.f };
    float CHANGE{ 0.1f };
    bool chaos_mode{ 0 };
};

//==============================================================================
/**
*/
class DonkeyDistortAudioProcessor  : public juce::AudioProcessor, public juce::ValueTree::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DonkeyDistortAudioProcessor();
    ~DonkeyDistortAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // my stoff

    juce::AudioProcessorValueTreeState& get_APVTS(){ return APVTS; }

    float zaddy_val;
    float random_between_samples;
    ChainSettings settings;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState APVTS{ *this, nullptr, "Parameters", createParameterLayout() };
    void valueTreePropertyChanged(juce::ValueTree & treeWhosePropertyHasChanged, const juce::Identifier & property) override;
    void update_paramaters();
    void fill_buffer(int channel, int buffer_size, int circular_buffer_size, float* channelData);
    void read_from_buffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& circular_buffer, int channel, int circular_buffer_size, int buffer_size);


    std::atomic<bool> should_update { false };
    juce::AudioBuffer<float> circular_buffer;
    int write_pos = 0;
    int read_pos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DonkeyDistortAudioProcessor)

};
