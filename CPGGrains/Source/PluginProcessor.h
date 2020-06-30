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
#include <vector>

#define NUM_NODES 4
#define UNITY_CONN_WEIGHT 3
#define P_COMPENSATION 0.973200f
#define DEFAULT_CURVE_X 0.25f, 0.333f, 0.5f, 1.111f, 1.333f, 2.f, 3.f, 4.f, 6.f, 8.f
#define DEFAULT_CURVE_Y 1.955492228f, 1.098290155f, 0.107150259f, 0.133937824f, 0.321450777f, 0.517f, 0.641026425f, 0.937029016f, 1.194725389f, 1.259015544f



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
    void setWeight(int parentNode, int childNode, float value);

private:
    int chooseFile();

    struct PositionListener : public AudioProcessorValueTreeState::Listener {
        void parameterChanged(const String& parameterID, float newValue) override {
            auto nodeNumber = parameterID.getCharPointer();
            int node = nodeNumber.getIntValue64();
            //float x = state->getParameterAsValue(String(node) + "x").getValue();
            //float y = state->getParameterAsValue(String(node) + "y").getValue();
        }
    };


    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    int numSamples, sampleSR;
    GrainGenerator generators[NUM_NODES + 1];
    AudioProcessorValueTreeState grainParams;
    AudioFormatManager formatManager;
    AudioSampleBuffer fileBuffer;
    const float* samplePointer;
    std::unique_ptr<MatsuokaEngine> network;
    PositionListener posListener;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CpggrainsAudioProcessor)
};
