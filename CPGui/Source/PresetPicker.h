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
                     public Button::Listener
{
public:
    PresetPicker(ActionListener* al)
    {
        listener = al;
        addAndMakeVisible(loadPresetButton);
        loadPresetButton.addListener(this);
        loadPresetButton.setButtonText("Select a preset (from Presets folder)");
        addAndMakeVisible(presetName);
        presetName.setText("NewPreset", false);
        addAndMakeVisible(savePresetButton);
        savePresetButton.addListener(this);
        savePresetButton.setButtonText("Save Current Configuration");
        findExistingPresets();
    }

    void paint(Graphics& g) override
    {
        g.fillAll(juce::Colours::antiquewhite);
    }

    void resized() override 
    {
        Grid grid;

        using Track = Grid::TrackInfo;

        grid.templateRows = { Track(1_fr), Track(1_fr) };
        grid.templateColumns = { Track(1_fr), Track(3_fr) };
        grid.autoFlow = juce::Grid::AutoFlow::rowDense;
        grid.items = {
            GridItem{ loadPresetButton }.withArea(1, GridItem::Span(2)),
            GridItem{ presetName }, GridItem{ savePresetButton } };
        grid.performLayout(getLocalBounds());
    }

    void buttonClicked(Button* button) override 
    {
        if (button == &loadPresetButton) {
            const int presetNumber = presetMenu.show();
            juce::String selectedFilePath = presetFiles[presetNumber - 1];
            presetFilePath = presetFolder.getChildFile(selectedFilePath + ".xml").getFullPathName();
            listener->actionListenerCallback("loadPreset");
        }
        if (button == &savePresetButton) {
            juce::String savePresetName = presetName.getText();
            presetMenu.addItem(++numPresets, savePresetName);
            presetFiles.add(savePresetName);
            listener->actionListenerCallback("savePreset");
        }
    }

    String getPresetPath() {
        return presetFilePath;
    }

    String savePresetPath() {
        return presetName.getText();
    }

private:
    void findExistingPresets() {
        if (presetFolder.exists()) {
            for (auto file : presetFolder.findChildFiles(File::findFiles, false, "*.xml")) {
                presetFiles.add(file.getFileNameWithoutExtension());
                presetMenu.addItem(++numPresets, file.getFileNameWithoutExtension());
            }
        }
    }
    juce::ActionListener* listener;
    juce::TextButton loadPresetButton, savePresetButton;
    juce::TextEditor presetName;
    juce::PopupMenu presetMenu;
    juce::File presetFolder{ File::getSpecialLocation(File::hostApplicationPath).getParentDirectory().getChildFile("Presets") };
    juce::Array<juce::String> presetFiles;
    juce::String presetFilePath;
    int numPresets = 0;
};
