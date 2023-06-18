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
    int seed{ 0 };
    int wavetable{ 0 };
    float bitrate{ 0.f };

    bool chaos_pause{ 0 };
};

//==============================================================================
/**
*/
class DonkeyDistortAudioProcessor : public juce::AudioProcessor, public juce::ValueTree::Listener
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
    void generateRandomArray(float minValue, float maxValue, int seed);

    juce::AudioProcessorValueTreeState& get_APVTS(){ return APVTS; }
    const int rows = 1024;
    const int columns = 1024;

    float zaddy_val;
    float random_between_samples;
    float max_sample;

    juce::Array<float> random_float_values;

    // Create a 2D array
    std::vector<std::vector<float>> random_float_value_table;

    ChainSettings settings;

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState APVTS{ *this, nullptr, "Parameters", createParameterLayout() };
    void valueTreePropertyChanged(juce::ValueTree & treeWhosePropertyHasChanged, const juce::Identifier & property) override;
    void update_paramaters();
    float chooseRandomFloat(float original_sample, int wavetable_pos);
    const float multiplier = 11.f;  // Multiplier constant
    const int increment = 12345;        // Increment constant


    std::atomic<bool> should_update { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DonkeyDistortAudioProcessor)
};
