/*
  ==============================================================================

    MixerMenu.h
    Created: 27 Jul 2020 11:46:34am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "Identifiers.h"
#include "MixerParamContainer.h"
#include "PresetPicker.h"

class MixerMenu : public Component {
public:
    MixerMenu(ValueTree params, ChangeListener* changeListener)
        : nodeParams(params)
    {
        for (int i = 0; i < 4; i++) {
            nodeContainers[i].setColour(0, juce::Colour::fromString(nodeParams.getChild(i).getProperty(Ids::colour).toString()));
            addAndMakeVisible(nodeContainers[i]);
        }
        for (int i = 0; i < nodeParams.getNumChildren(); i++) {
            addNodeSliders(i);
        }
        presetPicker.reset(new PresetPicker{changeListener});
        addAndMakeVisible(presetPicker.get());
    }

    void resized() override
    {
        Grid grid;

        using Track = Grid::TrackInfo;

        grid.templateRows = { Track(1_fr), Track(1_fr), Track(1_fr) };
        grid.templateColumns = { Track(1_fr), Track(1_fr) };
        grid.autoFlow = juce::Grid::AutoFlow::rowDense;
        grid.items = { GridItem{nodeContainers[0]}, GridItem{nodeContainers[1]},
                       GridItem{nodeContainers[2]}, GridItem{nodeContainers[3]},
                       GridItem{presetPicker.get()}.withArea(3, GridItem::Span(1)) 
        }; 
        grid.performLayout(getLocalBounds());
    }

    void visibilityChanged() override
    {
        if (isVisible()) {
            for (int i = 0; i < nodeParams.getNumChildren(); i++) {
                ValueTree child = nodeParams.getChild(i);
                sliders[5 * i + 0]->setValue(child.getProperty(Ids::grainLength), dontSendNotification);
                sliders[5 * i + 1]->setValue(child.getProperty(Ids::startTime), dontSendNotification);
                sliders[5 * i + 2]->setValue(child.getProperty(Ids::frequency), dontSendNotification);
                sliders[5 * i + 3]->setValue(child.getProperty(Ids::volume), dontSendNotification);
                sliders[5 * i + 4]->setValue(child.getProperty(Ids::pan), dontSendNotification);
            }
        }
    }

    String getPresetFilePath() {
        return presetPicker->getPresetPath();
    }

private:
    void addNodeSliders(int i) {
        addSlider(5.0f, 5000.0f, 0.25f, Ids::grainLength, i);
        addSlider(0.0f, 1.0f, 1.0f, Ids::startTime, i);
        addSlider(0.03125f, 200, 0.5f, Ids::frequency, i);
        addSlider(0.0f, 1.0f, 1.0f, Ids::volume, i);
        addSlider(0.0f, 1.0f, 1.0f, Ids::pan, i);
    }

    void addSlider(float rangeStart, float rangeEnd, float skewFactor, Identifier property, int i) {
        Slider* slider = sliders.add(new Slider{});
        slider->setNormalisableRange(NormalisableRange<double>(rangeStart, rangeEnd, 0.001f, skewFactor));
        slider->onValueChange = [this, i, slider, property]{
            nodeParams.getChild(i).setProperty(property, slider->getValue(), nullptr);
        };
        slider->setSliderStyle(juce::Slider::SliderStyle::Rotary);
        /*Change this in look and feel, we did it before remember?*/
        slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
        Label* label = labels.add(new Label{});
        if (property == Ids::grainLength) label->setText("Size", dontSendNotification);
        if (property == Ids::startTime) label->setText("Position", dontSendNotification);
        if (property == Ids::frequency) label->setText("Density", dontSendNotification);
        if (property == Ids::volume) label->setText("Gain", dontSendNotification);
        if (property == Ids::pan) label->setText("Pan", dontSendNotification);
        label->setJustificationType(juce::Justification::topLeft);
        nodeContainers[i].addAndMakeVisible(label);
        nodeContainers[i].addAndMakeVisible(slider);
    }

    OwnedArray<Slider> sliders;
    OwnedArray<Label> labels;
    OwnedArray<Rectangle<int>> outlineRects;
    ValueTree nodeParams;
    MixerParamContainer nodeContainers[4];
    std::unique_ptr<PresetPicker> presetPicker;
};