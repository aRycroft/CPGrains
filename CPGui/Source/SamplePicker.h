/*
  ==============================================================================

    SamplePicker.h
    Created: 12 Jul 2020 11:15:34am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

struct SamplePicker : public Component,
                      public ChangeListener,
                      public Button::Listener,
                      public ValueTree::Listener
{
    SamplePicker(ChangeListener* listener, ValueTree nodeParams)
        :
        waveformCache(5),
        waveform(512, waveformManager, waveformCache),
        params(nodeParams)
    {
        params.addListener(this);
        addAndMakeVisible(openButton);
        openButton.setButtonText("Click here to select a sample..");
        openButton.addListener(this);
        waveformManager.registerBasicFormats();
        waveform.addChangeListener(this);
        chooser.reset(new juce::FileChooser("Select a Wave file to play...",
            {},
            "*.wav;*.mp3"));
        this->listener = listener;
    }
    void paint(Graphics& g) override 
    {
        g.fillAll(Colours::aliceblue);
        g.setColour(Colours::black);
        if (showWaveform) {
            waveform.drawChannels(g,
                Rectangle<int>(getWidth(), getHeight()),
                0.0,
                waveform.getTotalLength(),
                1.0f);
            for (int i{ 0 }; i < params.getNumChildren(); i++) {
                ValueTree child = params.getChild(i);
                if (child.getProperty("active")) {
                    g.setColour(juce::Colour::fromString(child.getProperty("colour").toString()));
                    g.setOpacity(0.6);
                    float positionInWaveform = (float)child.getProperty("startTime") * getWidth();
                    juce::Line<float> line{ positionInWaveform, 0, positionInWaveform, (float)getHeight() };
                    double length = (double)child.getProperty("grainLength") * getWidth() / (waveform.getTotalLength() * 1000.0);
                    length = juce::jmax(length, 3.0);
                    g.drawLine(line, length);
                }
            }
        }
        
    }
    void resized() override
    {
        openButton.setBounds(0, 0, getWidth(), getHeight());
    }

    void shouldShowWaveform(bool trueForWaveform) {
        showWaveform = trueForWaveform;
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        openButton.setAlpha(0);
        repaint();
    }

    void buttonClicked(Button* button) override {
        
        if (chooser->browseForFileToOpen())
        {
            File f = chooser->getResult();
            if (f.existsAsFile())
            {
                waveform.setSource(new juce::FileInputSource(f));
                showWaveform = true;
                listener->changeListenerCallback(nullptr);
            }
        }
    }

    void valueTreePropertyChanged(ValueTree& vTree, const Identifier& property) override {
        repaint();
    }

    String getFileName() {
        return chooser->getResult().getFullPathName();
    }

private:
    AudioFormatManager waveformManager;
    AudioThumbnailCache waveformCache;
    AudioThumbnail waveform;
    std::unique_ptr<FilenameComponent> fileComp;
    std::unique_ptr<juce::FileChooser> chooser;
    bool showWaveform{ false };
    juce::TextButton openButton;
    juce::ChangeListener* listener;
    juce::ValueTree params;
};