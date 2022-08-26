/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
leoSynthAudioProcessorEditor::leoSynthAudioProcessorEditor (leoSynthAudioProcessor& p)
    : AudioProcessorEditor (&p),
audioProcessor (p),
osc (audioProcessor.apvts, "OSCWAVETYPE", "OSCFMFREQ", "OSCFMDEPTH", "OSCGAIN"),
osc2 (audioProcessor.apvts, "OSCWAVETYPE2", "OSCFMFREQ2", "OSCFMDEPTH2", "OSCGAIN2"),
adsr("Amp Envelope", audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE"),
filter(audioProcessor.apvts, "FILTERTYPE", "FILTERFREQ", "FILTERRES"),
modAdsr("Mod Envelope", audioProcessor.apvts, "MODATTACK", "MODDECAY", "MODSUSTAIN", "MODRELEASE"),

delay(audioProcessor.apvts, "DELAYTIME", "DELAYFEEDBACK"),
oscilloscope(),
keyboard()

{
   
    setSize (1280, 500);
    addAndMakeVisible (osc);
    addAndMakeVisible (adsr);
    addAndMakeVisible(filter);
    addAndMakeVisible(modAdsr);
    addAndMakeVisible(osc2);
}

leoSynthAudioProcessorEditor::~leoSynthAudioProcessorEditor()
{
}

//==============================================================================
void leoSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void leoSynthAudioProcessorEditor::resized()
{
    const auto paddingX = 5;
    const auto paddingY = 35;
    const auto paddingY2 = 235;
    const auto paddingY3 = 435;
    const auto paddingY4 = 635;
    const auto width = 300;
    const auto height = 200;


    osc.setBounds (paddingX, paddingY, width, height);
    osc2.setBounds(paddingX, paddingY2, width, height);
    
    adsr.setBounds (osc.getRight(), paddingY2/2, width, height);
    filter.setBounds(adsr.getRight(), adsr.getY(), width, height);
    modAdsr.setBounds(filter.getRight(), filter.getY(), width, height);

    
}


