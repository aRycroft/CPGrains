/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MainComponent.h"
//==============================================================================
/**
*/
class CpggrainsAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    CpggrainsAudioProcessorEditor (CpggrainsAudioProcessor&, AudioProcessorValueTreeState&);
    ~CpggrainsAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CpggrainsAudioProcessor& processor;
    AudioProcessorValueTreeState& params;
    std::unique_ptr<MainComponent> mainGui;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CpggrainsAudioProcessorEditor)
};
