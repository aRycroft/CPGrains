/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
/*CpggrainsAudioProcessorEditor::CpggrainsAudioProcessorEditor (CpggrainsAudioProcessor& p, AudioProcessorValueTreeState& tree)
    : AudioProcessorEditor (&p), processor (p), params(tree)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    mainGui.reset(new MainComponent(params));
    addAndMakeVisible(*mainGui);
    setResizable(true, true);
    setSize (400, 300);
}

CpggrainsAudioProcessorEditor::~CpggrainsAudioProcessorEditor()
{
}

//==============================================================================
void CpggrainsAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    /*g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}*/
/*
void CpggrainsAudioProcessorEditor::resized()
{
    mainGui->setBounds(getBounds());
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
*/