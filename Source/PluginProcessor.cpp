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

void DonkeyDistortAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (should_update)
    {
        update_paramaters();
    }


    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    // moast donkey distorted val
    float donkeys_val = 0.f;
    float update_sample = 0.f;
    random_between_samples = 0.f;
    bool chaos_mode = false;

    // for each channel (left right)
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);


        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
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
            update_sample = juce::jmap(settings.drive, channelData[sample], donkeys_val);

            channelData[sample] = update_sample;
            //random_between_samples = juce::jmap(juce::Random::getSystemRandom().nextFloat(), channelData[sample], update_sample);

            //if (chaos_mode)
            //{
            //    channelData[sample] = settings.drive;
            //}
            //if (!chaos_mode)
            //{
            //    //channelData[sample] = random_between_samples;
            //    channelData[sample] = settings.drive;
            //}
        }
    }




    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);
    //    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    //    {
    //        channelData.
    //    }
    //    // ..do something to the data...
    //}
}

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
}

juce::AudioProcessorValueTreeState::ParameterLayout DonkeyDistortAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("Drive", "Drive", 0.f, 1.f, 0.f));


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
