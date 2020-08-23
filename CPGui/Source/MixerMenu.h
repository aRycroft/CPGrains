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
#include "PortAndUtilityPicker.h"

class MixerMenu : public Component,
                  public Button::Listener
{
public:
    MixerMenu(ValueTree params, ActionListener* actionListener)
        : nodeParams(params)
    {
        for (int i = 0; i < 4; i++) {
            nodeContainers[i].setColour(0, juce::Colour::fromString(nodeParams.getChild(i).getProperty(Ids::colour).toString()));
            addAndMakeVisible(nodeContainers[i]);
        }
        for (int i = 0; i < nodeParams.getNumChildren(); i++) {
            addNodeSliders(i);
            addButtons(i);
        }
        presetPicker.reset(new PresetPicker{ actionListener });
        portPicker.reset(new PortAndUtilityPicker{ actionListener, nodeParams });
        addAndMakeVisible(presetPicker.get());
        addAndMakeVisible(portPicker.get());
    }

    void resized() override
    {
        Grid grid;

        using Track = Grid::TrackInfo;

        grid.templateRows = { Track(2_fr), Track(2_fr), Track(1_fr) };
        grid.templateColumns = { Track(1_fr), Track(1_fr) };
        grid.autoFlow = juce::Grid::AutoFlow::rowDense;
        grid.items = { GridItem{nodeContainers[0]}, GridItem{nodeContainers[1]},
                       GridItem{nodeContainers[2]}, GridItem{nodeContainers[3]},
                       GridItem{presetPicker.get()}, GridItem{portPicker.get()}
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

    void buttonClicked(Button* button) override 
    {
        int nodeNumber = button->getName().getLastCharacter() - '0';
        /*Muted*/
        if (button->getName().startsWithChar('m')) {
            nodeParams.getChild(nodeNumber).setProperty(Ids::muted, button->getToggleState(), nullptr);
        }
        /*Soloed*/
        else if (button->getName().startsWithChar('s')) {
            if (button->getToggleState()) {
                for (int i{ 0 }; i < nodeParams.getNumChildren(); i++) {
                    mAndSbuttons[i * 2]->setEnabled(false);
                    if (i != nodeNumber && !mAndSbuttons[i * 2 + 1]->getToggleState()) {
                        nodeParams.getChild(i).setProperty(Ids::muted, true, nullptr);
                    }
                    else {
                        mAndSbuttons[i * 2]->setToggleState(false, dontSendNotification);
                        nodeParams.getChild(i).setProperty(Ids::muted, false, nullptr);
                    }
                }
            }
            else {
                bool otherSoloedButton{ false };
                for (int i{ 0 }; i < nodeParams.getNumChildren(); i++) {
                    if (mAndSbuttons[i * 2 + 1]->getToggleState()) {
                        otherSoloedButton = true;
                    }
                    
                }
                if (!otherSoloedButton) {
                    for (int i{ 0 }; i < nodeParams.getNumChildren(); i++) {
                        mAndSbuttons[i * 2]->setEnabled(true);
                        mAndSbuttons[i * 2 + 1]->setEnabled(true);
                        nodeParams.getChild(i).setProperty(Ids::muted, mAndSbuttons[i * 2]->getToggleState(), nullptr);
                    }
                }
                else {
                    nodeParams.getChild(nodeNumber).setProperty(Ids::muted, true, nullptr);
                }
            }
        }
    }

    String getPresetFilePath() {
        return presetPicker->getPresetPath();
    }

    String savePresetFilePath() {
        return presetPicker->savePresetPath();
    }

    juce::String getPortInNumber()
    {
        return portPicker->getPortInNumber();
    }

    juce::String getPortOutNumber()
    {
        return portPicker->getPortOutNumber();
    }

private:
    void addButtons(int i) {
        juce::ToggleButton* muteButton = mAndSbuttons.add(new ToggleButton{ "mute" + String{i} });
        juce::ToggleButton* soloButton = mAndSbuttons.add(new ToggleButton{ "solo" + String{i} });
        muteButton->setButtonText("Mute");
        soloButton->setButtonText("Solo");
        muteButton->addListener(this);
        soloButton->addListener(this);
        nodeContainers[i].addAndMakeVisible(muteButton);
        nodeContainers[i].addAndMakeVisible(soloButton);
    }

    void addNodeSliders(int i) {
        addSlider(5.0f, 5000.0f, 0.25f, Ids::grainLength, i);
        addSlider(0.0f, 1.0f, 1.0f, Ids::startTime, i);
        addSlider(0.03125f, 200.0f, 0.5f, Ids::frequency, i);
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
        if (property != Ids::pan && property != Ids::volume) {
            slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, false, 60, 40);
            slider->setNumDecimalPlacesToDisplay(1);
        }
        else {
            slider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
        }
        Label* label = labels.add(new Label{});
        if (property == Ids::grainLength) label->setText("Size", dontSendNotification);
        if (property == Ids::startTime) label->setText("Position", dontSendNotification);
        if (property == Ids::frequency) label->setText("Density", dontSendNotification);
        if (property == Ids::volume) label->setText("Gain", dontSendNotification);
        if (property == Ids::pan) label->setText("Pan", dontSendNotification);
        label->setJustificationType(juce::Justification::topLeft);
        label->attachToComponent(slider, true);
        nodeContainers[i].addAndMakeVisible(label);
        nodeContainers[i].addAndMakeVisible(slider);
    }

    OwnedArray<Slider> sliders;
    OwnedArray<Label> labels;
    OwnedArray<juce::ToggleButton> mAndSbuttons;
    ValueTree nodeParams;
    MixerParamContainer nodeContainers[4];
    std::unique_ptr<PresetPicker> presetPicker;
    std::unique_ptr<PortAndUtilityPicker> portPicker;
};