/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>


//==============================================================================
leoSynthAudioProcessor::leoSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts (*this, nullptr, "Parameters", createParams())
#endif
{
    synth.addSound (new SynthSound());
    for(int i=0; i<10; i++){
        synth.addVoice(new SynthVoice());
    }
}

leoSynthAudioProcessor::~leoSynthAudioProcessor()
{
    
}

//==============================================================================
const juce::String leoSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool leoSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool leoSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool leoSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double leoSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int leoSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int leoSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void leoSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String leoSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void leoSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void leoSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
   
}

void leoSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool leoSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void leoSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            
            //OSC
            auto& oscWaveChoice = *apvts.getRawParameterValue ("OSCWAVETYPE");
            auto& fmFreq = *apvts.getRawParameterValue ("OSCFMFREQ");
            auto& fmDepth = *apvts.getRawParameterValue ("OSCFMDEPTH");
            auto& osc1Pitch = *apvts.getRawParameterValue("OSC1PITCH");
            auto& osc1Gain = *apvts.getRawParameterValue ("OSCGAIN");


            //OSC
            auto& oscWaveChoice2 = *apvts.getRawParameterValue ("OSCWAVETYPE2");
            auto& fmFreq2 = *apvts.getRawParameterValue ("OSCFMFREQ2");
            auto& fmDepth2 = *apvts.getRawParameterValue ("OSCFMDEPTH2");
            auto& osc2Pitch = *apvts.getRawParameterValue("OSC2PITCH");
            
            auto& osc2Gain = *apvts.getRawParameterValue ("OSCGAIN2");
            //AMP ADSR
            auto& attack = *apvts.getRawParameterValue ("ATTACK");
            auto& decay = *apvts.getRawParameterValue ("DECAY");
            auto& sustain = *apvts.getRawParameterValue ("SUSTAIN");
            auto& release = *apvts.getRawParameterValue ("RELEASE");
           
            
            // FILTER
            auto& filterType = *apvts.getRawParameterValue("FILTERTYPE");
            auto& cutoff = *apvts.getRawParameterValue("FILTERFREQ");
            auto& resonance = *apvts.getRawParameterValue("FILTERRES");
            // FILTER
            
            
            // MOD ADSR
            auto& modAttack = *apvts.getRawParameterValue ("MODATTACK");
            auto& modDecay = *apvts.getRawParameterValue ("MODDECAY");
            auto& modSustain = *apvts.getRawParameterValue ("MODSUSTAIN");
            auto& modRelease = *apvts.getRawParameterValue ("MODRELEASE");
            // MOD ADSR
            
          
            auto& osc1 = voice->getOscillator1();
            auto& osc2 = voice->getOscillator2();
            
            auto& adsr = voice->getAdsr();
            auto& filterAdsr = voice->getModAdsr();
            
            for (int i=0; i<getTotalNumOutputChannels();i++)
            {
                osc1[i].setWaveType(oscWaveChoice);
                osc1[i].setFrequency(fmFreq);
                osc1[i].updateFm(fmFreq, fmDepth);
                osc1[i].setGain(osc1Gain);
                osc1[i].setPitch(osc1Pitch);
                osc2[i].setWaveType(oscWaveChoice2);
                osc2[i].setFrequency(fmFreq2);
                osc2[i].updateFm(fmFreq2, fmDepth2);
                osc2[i].setGain(osc2Gain);
                osc2[i].setPitch(osc2Pitch);
            }
            adsr.updateADSR(attack.load(), decay.load(), sustain.load(), release.load());
            filterAdsr.updateADSR(modAttack.load(), modDecay.load(), modSustain.load(), modRelease.load());
            voice->updateFilter(filterType, cutoff, resonance);
           
            
        }
    }
    
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

}

//==============================================================================
bool leoSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* leoSynthAudioProcessor::createEditor()
{
    return new leoSynthAudioProcessorEditor (*this);
}

//==============================================================================
void leoSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void leoSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new leoSynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout leoSynthAudioProcessor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    //OSC MAIN
    // OSC select
    params.push_back (std::make_unique<juce::AudioParameterChoice>("OSCWAVETYPE", "Osc 1 Wave Type", juce::StringArray { "Sine", "Saw", "Square" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSCFMFREQ", "Osc 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSCFMDEPTH", "Osc 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1PITCH", "Osc 1 Pitch", juce::NormalisableRange<float> { -64.0f, 64.0f, 1.0f}, -64.0f));
   
    params.push_back (std::make_unique<juce::AudioParameterChoice>("OSCWAVETYPE2", "Osc 2 Wave Type", juce::StringArray { "Sine", "Saw", "Square" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSCFMFREQ2", "Osc 2 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSCFMDEPTH2", "Osc 2 FM Depth", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC2PITCH", "Osc 2 Pitch", juce::NormalisableRange<float> { -64.0f, 64.0f, 1.0f}, -64.0f));
    
    // OSC Gain
        params.push_back (std::make_unique<juce::AudioParameterFloat>("OSCGAIN", "Oscillator 1 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, 0.1f, "dB"));
        params.push_back (std::make_unique<juce::AudioParameterFloat>("OSCGAIN2", "Oscillator 2 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, 0.1f, "dB"));
        

    // ADSR
    params.push_back (std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> { 0.01f, 3.0f, 0.01f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> { 0.01f, 1.0f, 0.1f }, 0.01f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.01f, 1.0f, 0.01f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> { 0.01f, 3.0f, 0.01f }, 0.4f));
    
    // Filter ADSR
    params.push_back (std::make_unique<juce::AudioParameterFloat>("MODATTACK", "Mod Attack", juce::NormalisableRange<float> { 0.01f, 100.0f, 0.01f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("MODDECAY", "Mod Decay", juce::NormalisableRange<float> { 0.01f, 1.0f, 0.01f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("MODSUSTAIN", "Mod Sustain", juce::NormalisableRange<float> { 0.01f, 1.0f, 0.01f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("MODRELEASE", "Mod Release", juce::NormalisableRange<float> { 0.01f, 3.0f, 0.01f }, 0.4f));
    
    // Filter
    params.push_back (std::make_unique<juce::AudioParameterChoice>("FILTERTYPE", "Filter Type", juce::StringArray { "Low-pass", "Band-pass", "High-pass" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERFREQ", "Filter Cutoff", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 200.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERRES", "Filter Resonance", juce::NormalisableRange<float> { 1.0f, 10.0f, 0.1f }, 1.0f));

   
    // Delay
    params.push_back (std::make_unique<juce::AudioParameterFloat>("DELAYTIME", "Delay Time", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 200.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("DELAYFEEDBACK", "Feedback", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 200.0f));
    
    return { params.begin(), params.end() };
}
