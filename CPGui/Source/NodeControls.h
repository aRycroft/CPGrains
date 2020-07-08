/*
  ==============================================================================

    ControlPanel.h
    Created: 29 Jun 2020 11:00:58am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "ControlInterface.h"
struct NodeControls : public ControlInterface{
    NodeControls(Slider::Listener* listener, LookAndFeel_V4* LandF)
        :
        waveformCache(5),
        waveform(512, waveformManager, waveformCache)
    {
        waveformManager.registerBasicFormats();
        paramTree = ValueTree(Identifier("nodeParams"));
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

        /*waveform.drawChannels(g,                                      
            Rectangle<int>(100, 100),
            0.0,                                    
            waveform.getTotalLength(),             
            1.0f);*/

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
                                 Track(1_fr), Track(3_fr) };
        for (auto* s : sliders) {
            GridItem g1 = GridItem(s);
            grid.items.add(GridItem());
            grid.items.add(g1);
        }
        grid.performLayout(getLocalBounds());
    }
    void addSliders(Slider::Listener* listener, LookAndFeel_V4* LandF) {
        addAndMakeVisible(sliders.add(new Slider("grainLength")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(5.0f, 5000.0f, 0.1f, 0.25f));
        addAndMakeVisible(labels.add(new Label()));
        Label* l = labels.getLast();
        l->setText("Grain Size", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("startTime")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1000.0f, 0.01f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Position", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("frequency")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.03125f, 16.0f, 0.01f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Freq", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("pan")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Pan", dontSendNotification);
        l->attachToComponent(sliders.getLast(), true);

        addAndMakeVisible(sliders.add(new Slider("volume")));
        sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
        addAndMakeVisible(labels.add(new Label()));
        l = labels.getLast();
        l->setText("Volume", dontSendNotification);
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
        ValueTree nodeParam(componentId);
        nodeParam.setProperty("grainLength", 200.0f, nullptr);
        nodeParam.setProperty("startTime", 0.0f, nullptr);
        nodeParam.setProperty("frequency", 1.0f, nullptr);
        nodeParam.setProperty("pan", 0.5f, nullptr);
        nodeParam.setProperty("volume", 0.7f, nullptr);
        paramTree.addChild(nodeParam, -1, nullptr);
    }
    
    AudioFormatManager waveformManager;
    AudioThumbnailCache waveformCache;
    AudioThumbnail waveform;
    ValueTree paramTree;
    OwnedArray<Slider> sliders;
    OwnedArray<Label> labels;
    bool displayingControls{ false };
    bool displayingWaveform{ false };
};