/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "random"

// Constructor
DonkeyDistortAudioProcessor::DonkeyDistortAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    APVTS.state.addListener(this);
    zaddy_val = 1.f;


    // Resize the array to the desired dimensions
    random_float_value_table.resize(rows);

    for (int i = 0; i < rows; ++i)
    {
        random_float_value_table[i].resize(columns);
    }

    generateRandomArray(0.f, 1.f, settings.seed);

}

DonkeyDistortAudioProcessor::~DonkeyDistortAudioProcessor()
{
}

//==============================================================================
const juce::String DonkeyDistortAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DonkeyDistortAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DonkeyDistortAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DonkeyDistortAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DonkeyDistortAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DonkeyDistortAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DonkeyDistortAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DonkeyDistortAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DonkeyDistortAudioProcessor::getProgramName (int index)
{
    return {};
}

void DonkeyDistortAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DonkeyDistortAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void DonkeyDistortAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DonkeyDistortAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DonkeyDistortAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (should_update)
    {
        update_paramaters();
    }

    // moast donkey distorted val
    float donkeys_val = 0.f;
    float update_sample = 0.f;
    max_sample = 0.f;

    float chaos_max = 0.f;
    random_between_samples = 0.f;
    int buffer_size = buffer.getNumSamples();

    // for each channel (left right)
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer_size; ++sample)
        {
            // obtain donkeys val
            if (channelData[sample] > 0)
            {
                donkeys_val = 1.f - channelData[sample];
            }
            if (channelData[sample] < 0)
            {
                donkeys_val = -1.f - channelData[sample];
            }
            if (channelData[sample] == 0)
            {
                donkeys_val = 0.f;
            }

            // create the new sample position (update_sample)
            update_sample = juce::jmap(settings.drive, channelData[sample], donkeys_val);

            if (update_sample == 0.f)
            {
                update_sample = 0.f;
            }
            else if (std::abs(update_sample) < 1.0f)
            {
                update_sample *= (1.f - ((-0.8 * (std::pow(1 - settings.drive, 2)) + .8)));
            }
            
            // create the max value for the randomizer (this is represented by chaos slider)
            chaos_max = juce::jmap(settings.chaos, update_sample, channelData[sample]);

            if (settings.chaos_pause)
            {
                update_sample = juce::jmap(chooseRandomFloat(channelData[sample], settings.wavetable), channelData[sample], update_sample);
                chaos_max = juce::jmap(settings.chaos, update_sample, channelData[sample]);
            }

            //chooses random value between old sample pos and new sample pos (update_sample)
            random_between_samples = juce::jmap(juce::Random::getSystemRandom().nextFloat(), update_sample, chaos_max);

            // set buffer
            channelData[sample] = random_between_samples;
        }
    }
}

//==============================================================================
bool DonkeyDistortAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DonkeyDistortAudioProcessor::createEditor()
{
    return new DonkeyDistortAudioProcessorEditor (*this);
}

//==============================================================================
void DonkeyDistortAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(APVTS.copyState().createXml());
    copyXmlToBinary(*xml, destData);

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DonkeyDistortAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName(APVTS.state.getType()))
            APVTS.replaceState(juce::ValueTree::fromXml(*xmlState));

    update_paramaters();

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void DonkeyDistortAudioProcessor::update_paramaters()
{
    settings.drive = *APVTS.getRawParameterValue("Drive");
    settings.chaos = *APVTS.getRawParameterValue("Chaos_Amt");
    settings.seed = *APVTS.getRawParameterValue("Seed");
    settings.wavetable = *APVTS.getRawParameterValue("Wavetable");
    settings.bitrate = *APVTS.getRawParameterValue("Bitrate");

    settings.chaos_pause = *APVTS.getRawParameterValue("Chaos_Pause");
}

juce::AudioProcessorValueTreeState::ParameterLayout DonkeyDistortAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("Drive", "Drive", 0.f, 1.f, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Chaos_Amt", "Chaos_Amt", 0.f, 1.f, 0.f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Seed", "Seed", 0, 1000, 0.f));
    layout.add(std::make_unique<juce::AudioParameterInt>("Wavetable", "Wavetable", 0, columns - 1, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Bitrate", "Bitrate", 1.f, 100.f, 0.f));

    layout.add(std::make_unique<juce::AudioParameterBool>("Chaos_Pause", "Chaos_Pause", false));

    return layout;
}

void DonkeyDistortAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    should_update = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DonkeyDistortAudioProcessor();
}

void DonkeyDistortAudioProcessor::generateRandomArray(float minValue, float maxValue, int seed)
{
    std::mt19937 rng(seed);
    std::uniform_real_distribution<> dis(minValue, maxValue);

    for (int j = 0; j < columns; ++j)
    {
        random_float_value_table[0][j] = dis(rng);
        random_float_value_table[rows - 1][j] = dis(rng);
    }

    // Smooth interpolation between the first and last row
    for (int i = 1; i < rows - 1; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            float t = static_cast<float>(i) / (rows - 1);
            random_float_value_table[i][j] = (1.0f - t) * random_float_value_table[0][j] + t * random_float_value_table[rows - 1][j];
        }
    }
}

float DonkeyDistortAudioProcessor::chooseRandomFloat(float original_sample, int wavetable_pos)
{
    original_sample = std::abs(original_sample);
    original_sample = std::round(original_sample * settings.bitrate) / settings.bitrate;
    int random_index = int(multiplier * original_sample + increment) % columns;

    if (wavetable_pos < columns)
        return random_float_value_table[wavetable_pos][random_index];
    else
        return 0.f;
}