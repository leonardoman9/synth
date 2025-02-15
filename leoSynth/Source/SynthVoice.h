/*
  ==============================================================================

    SynthVoice.h
    Author:  Leonardo Mannini

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "Data/OscData.h"
#include "Data/AdsrData.h"
#include "Data/FilterData.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    void pitchWheelMoved (int newPitchWheelValue) override;
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    void renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;
    
    void reset();
    
    std::array<OscData, 2>& getOscillator1() { return osc; }
    std::array<OscData, 2>& getOscillator2() { return osc2; }
    
    AdsrData& getAdsr() {return adsr;}
    AdsrData& getModAdsr() {return modAdsr;}
    float getFilterAdsrOutput() {return filterAdsrOutput;}
    void updateFilter (const int filterType, const float frequency, const float resonance);
    void updateModParams (const int filterType, const float frequency, const float resonance, const float modulator);

   
    
private:
    juce::AudioBuffer<float> synthBuffer;
    static constexpr int numChannelsToProcess { 2 };
    std::array<OscData, numChannelsToProcess> osc;
    std::array<OscData, numChannelsToProcess> osc2;
    std::array<FilterData, numChannelsToProcess> filter;
    AdsrData adsr;
    AdsrData modAdsr;
    float filterAdsrOutput {0.0f};
    juce::dsp::Gain<float> gain;
    bool isPrepared { false };
};
