/*
  ==============================================================================

    PresetPicker.h
    Created: 11 Aug 2020 3:40:52pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class PresetPicker : public Component,
                     public Button::Listener{
public:
    PresetPicker(ChangeListener* cl)
    {
        choosePreset.reset(new juce::FileChooser("Select a preset file...",
            {},
            "*.xml;"));
        listener = cl;
        addAndMakeVisible(presetButton);
        presetButton.addListener(this);
    }

    void paint(Graphics& g) override
    {
        g.fillAll(juce::Colours::antiquewhite);
    }

    void resized() override 
    {
        presetButton.setBounds(0, 0, this->getWidth(), this->getHeight());
    }

    void buttonClicked(Button* button) override 
    {
        if (choosePreset->browseForFileToOpen())
        {
            File f = choosePreset->getResult();
            if (f.existsAsFile())
            {
                listener->changeListenerCallback(nullptr);
            }
        }
    }

    String getPresetPath() {
        return choosePreset->getResult().getFullPathName();
    }

private:
    ChangeListener* listener;
    std::unique_ptr<juce::FileChooser> choosePreset;
    juce::TextButton presetButton;
};
