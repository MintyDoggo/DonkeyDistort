/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    
    auto circular_buffer_size = juce::jmap<float>(settings.CHANGE, 512, 44100);
    circular_buffer.setSize(getTotalNumOutputChannels(), (int)juce::jmap<float>(settings.CHANGE, 512, 44100));
    //auto circular_buffer_size = sampleRate * settings.CHANGE;
    //circular_buffer.setSize(getTotalNumOutputChannels(), (int)(sampleRate * settings.CHANGE));

    read_pos = write_pos - samplesPerBlock;

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
    float chaos_max = 0.f;
    random_between_samples = 0.f;
    float max_sample = 0.f;
    int buffer_size = buffer.getNumSamples();
    char even_buffer = 0;
    int circular_buffer_size = circular_buffer.getNumSamples();

    circular_buffer.setSize(getTotalNumOutputChannels(), (int)juce::jmap<float>(settings.CHANGE, 512, 44100));

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

            // create the max value for the randomizer (this is represented by chaos slider)
            chaos_max = juce::jmap(settings.chaos, update_sample, channelData[sample]);

            //chooses random value between old sample pos and new sample pos (update_sample)
            random_between_samples = juce::jmap(juce::Random::getSystemRandom().nextFloat(), update_sample, chaos_max);

            // set buffer
            channelData[sample] = random_between_samples;

            //if (!settings.chaos_mode)
            //{
            //    channelData[sample] = update_sample;
            //}
            //if (settings.chaos_mode)
            //{
            //    channelData[sample] = random_between_samples;
            //}
        }

        //fill_buffer(channel, buffer_size, circular_buffer_size, channelData);
        //read_from_buffer(buffer, circular_buffer, channel, circular_buffer_size, buffer_size);
    }

    write_pos += buffer_size;
    write_pos %= circular_buffer_size;
}

//void DonkeyDistortAudioProcessor::fill_buffer(int channel, int buffer_size, int circular_buffer_size, float* channelData);
//{
//    // if space, just copy
//    if (circular_buffer_size > buffer_size + write_pos)
//    {
//        circular_buffer.copyFrom(channel, write_pos, channelData, buffer_size);
//    }
//    else
//    {
//        // available space at end
//        auto num_samples_to_end = circular_buffer_size - write_pos;
//        circular_buffer.copyFrom(channel, write_pos, channelData, num_samples_to_end);
//
//        // available space at start
//        auto num_samples_at_start = buffer_size - num_samples_to_end;
//        circular_buffer.copyFrom(channel, 0, channelData, num_samples_at_start);
//    }
//}

void DonkeyDistortAudioProcessor::read_from_buffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& circular_buffer, int channel, int circular_buffer_size, int buffer_size)
{
    if (read_pos < 0)
    {
        read_pos += circular_buffer_size;
    }

    int stutter_size;

    stutter_size = juce::jmap<float>(settings.CHANGE, 512, 44100);

    if (read_pos + buffer_size < circular_buffer_size)
    {
        buffer.addFrom(channel, 0, circular_buffer.getReadPointer(channel, read_pos), stutter_size);
    }
    else
    {
        auto num_samples_to_end = circular_buffer_size - read_pos;
        buffer.addFrom(channel, 0, circular_buffer.getReadPointer(channel, read_pos), num_samples_to_end);

        auto num_samples_at_start = buffer_size - num_samples_to_end;
        buffer.addFrom(channel, num_samples_to_end, circular_buffer.getReadPointer(channel, 0), stutter_size - num_samples_to_end);
    }







    //if (read_pos < 0)
    //{
    //    read_pos += circular_buffer_size;
    //}

    //if (read_pos + buffer_size < circular_buffer_size)
    //{
    //    buffer.addFrom(channel, 0, circular_buffer.getReadPointer(channel, read_pos), buffer_size);
    //}
    //else
    //{
    //    auto num_samples_to_end = circular_buffer_size - read_pos;
    //    buffer.addFrom(channel, 0, circular_buffer.getReadPointer(channel, read_pos), num_samples_to_end);

    //    auto num_samples_at_start = buffer_size - num_samples_to_end;
    //    buffer.addFrom(channel, num_samples_to_end, circular_buffer.getReadPointer(channel, 0), num_samples_at_start);
    //}
}



//// get magnitude of both channels in 1 variable
//float channel_max = buffer.getMagnitude(channel, channelData[0], buffer_size);
//if (channel_max > max_sample)
//{
//    max_sample = channel_max;
//}


//for (int sample = 0; sample < buffer_size; ++sample)
//{
//    // obtain donkeys val
//    if (channelData[sample] > 0)
//    {
//        donkeys_val = 1.f - channelData[sample];
//    }
//    if (channelData[sample] < 0)
//    {
//        donkeys_val = -1.f - channelData[sample];
//    }
//    if (channelData[sample] == 0)
//    {
//        donkeys_val = 0.f;
//    }

//    // create the new sample position (update_sample)
//    update_sample = juce::jmap(settings.drive, channelData[sample], donkeys_val);

//    // create the max value for the randomizer (this is represented by chaos slider)
//    chaos_max = juce::jmap(settings.chaos, update_sample, channelData[sample]);

//    //chooses random value between old sample pos and new sample pos (update_sample)
//    random_between_samples = juce::jmap(juce::Random::getSystemRandom().nextFloat(), update_sample, chaos_max);
//    
//    // set buffer
//    channelData[sample] = random_between_samples;

//    //if (!settings.chaos_mode)
//    //{
//    //    channelData[sample] = update_sample;
//    //}
//    //if (settings.chaos_mode)
//    //{
//    //    channelData[sample] = random_between_samples;
//    //}

//}
//==============================================================================
bool DonkeyDistortAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DonkeyDistortAudioProcessor::createEditor()
{
    return new DonkeyDistortAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void DonkeyDistortAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DonkeyDistortAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void DonkeyDistortAudioProcessor::update_paramaters()
{
    settings.drive = APVTS.getRawParameterValue("Drive")->load();
    settings.chaos = APVTS.getRawParameterValue("Chaos_Amt")->load();
    settings.CHANGE = APVTS.getRawParameterValue("CHANGE")->load();
    settings.chaos_mode = APVTS.getRawParameterValue("Chaos")->load();

}

juce::AudioProcessorValueTreeState::ParameterLayout DonkeyDistortAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("Drive", "Drive", 0.f, 1.f, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Chaos_Amt", "Chaos_Amt", 0.f, 1.f, 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CHANGE", "CHANGE", 0.1f, 1.f, 0.1f));
    layout.add(std::make_unique<juce::AudioParameterBool>("Chaos", "Chaos", false));

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
