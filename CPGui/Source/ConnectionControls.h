/*
  ==============================================================================

    ConnectionControls.h
    Created: 29 Jun 2020 12:21:08pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "ControlInterface.h"
struct ConnectionControls : public ControlInterface {
    ConnectionControls(Slider::Listener* listener, LookAndFeel_V4* LandF)
    {
        paramTree = ValueTree(Identifier("connectionParams"));
        addSliders(listener, LandF);
    }

    void setUpAttachments(Identifier componentId) {
        for (auto& slider : sliders) {
            slider->setValue(paramTree.getChildWithName(componentId)[slider->getName()], dontSendNotification);
        }
    }

    void paint(Graphics& g) override
    {
        g.fillAll(Colours::darkslategrey);
    }

    void resized() override
    {
        Grid grid;

        using Track = Grid::TrackInfo;
        grid.templateRows = { Track(1_fr) };
        grid.templateColumns = { Track(1_fr), Track(3_fr), 
                                 Track(1_fr), Track(3_fr),
                                 Track(1_fr), Track(3_fr),
                                 Track(1_fr), Track(3_fr),
                                 Track(1_fr), Track(3_fr),
                                 Track(1_fr), Track(3_fr) };
        for (auto* s : sliders) {
            GridItem g1 = GridItem(s);
            grid.items.add(GridItem());
            grid.items.add(g1);
        }
        grid.performLayout(getLocalBounds());
    }

    void addSliders(Slider::Listener* listener, LookAndFeel_V4* LandF) {
        addAndMakeVisible(sliders.add(new Slider("weight")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 20.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        Label* l = labels.getLast();
        l->setText("Weight", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("lengthWeight")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 100.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Grain Length Mod", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("startWeight")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1000.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Start Time Mod", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("fWeight")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 20.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Feedback Weight", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("fLengthWeight")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 100.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Grain Length Mod", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("fStartWeight")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1000.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Start Time Mod", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);


        for (auto* slider : sliders) {
            slider->setLookAndFeel(LandF);
            slider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, false, 40, 40);
            slider->setNumDecimalPlacesToDisplay(1);
            slider->setSliderStyle(Slider::Rotary);
            slider->addListener(listener);
        }
    }

    void addParams(String componentId) {
        ValueTree conParam(componentId);
        conParam.setProperty("weight", 1.0f, nullptr);
        conParam.setProperty("fWeight", 0.0f, nullptr);
        conParam.setProperty("lengthWeight", 0.0f, nullptr);
        conParam.setProperty("startWeight", 0.0f, nullptr);
        conParam.setProperty("lengthWeight", 0.0f, nullptr);
        conParam.setProperty("startWeight", 0.0f, nullptr);
        paramTree.addChild(conParam, -1, nullptr);
    }
    ValueTree paramTree;
    OwnedArray<Slider> sliders;
    OwnedArray<ComboBox> radioBoxes;
    OwnedArray<Label> labels;
};