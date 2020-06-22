/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "Grain.h"
#include "GrainGenerator.h"
#include <list>

//==============================================================================
/**
*/
class CpggrainsAudioProcessor  : public AudioProcessor,
    public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    CpggrainsAudioProcessor();
    ~CpggrainsAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    void processSample(Grain* g, int currentIndex, float* LWritePointer, float* RWritePointer, float sampleValue);

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void parameterChanged(const String& parameterID, float	newValue) override;
    void setNodeFrequency(int nodeNumber, double freq);

private:
    int chooseFile();
    std::tuple<int, int> getStartAndEndSample(int nodeNumber, int endOfSample);
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    int numSamples, sampleSR;
    std::unique_ptr<Grain> test;
    GrainGenerator generators[5];
    Grain grains[128];
    std::list<Grain*> grainList;
    OwnedArray<Grain> allGrains;

    AudioProcessorValueTreeState grainParams;
    AudioFormatManager formatManager;
    AudioSampleBuffer fileBuffer;
    const float* readPointer;
    std::unique_ptr<MatsuokaEngine> network;
    uint16 tick = 0;
    unsigned maxGrains = 50, currentNumGrains = 0;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CpggrainsAudioProcessor)
};
