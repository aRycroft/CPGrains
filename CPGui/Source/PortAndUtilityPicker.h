/*
  ==============================================================================

    PortAndUtilityPicker.h
    Created: 21 Aug 2020 10:00:21am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "Identifiers.h"
#include "../JuceLibraryCode/JuceHeader.h"

class PortAndUtilityPicker : public juce::Component,
                             public juce::Button::Listener
{
public:
    PortAndUtilityPicker(ActionListener* al, ValueTree nodeParams)
        :tree(nodeParams)
    {
        listener = al;
        addAndMakeVisible(portInText);
        portInText.setText("8001", false);
        addAndMakeVisible(portOutText);
        portOutText.setText("8000", false);
        addAndMakeVisible(portInButton);
        portInButton.setButtonText("Select port in");
        portInButton.addListener(this);
        addAndMakeVisible(portOutButton);
        portOutButton.setButtonText("Select port out");
        portOutButton.addListener(this);

        addAndMakeVisible(minus100Rate);
        minus100Rate.setButtonText("---");
        minus100Rate.addListener(this);
        addAndMakeVisible(minus10Rate);
        minus10Rate.setButtonText("-");
        minus10Rate.addListener(this);
        addAndMakeVisible(plus10Rate);
        plus10Rate.setButtonText("+");
        plus10Rate.addListener(this);
        addAndMakeVisible(plus100Rate);
        plus100Rate.setButtonText("+++");
        plus100Rate.addListener(this);

        addAndMakeVisible(minus100Size);
        minus100Size.setButtonText("---");
        minus100Size.addListener(this);
        addAndMakeVisible(minus10Size);
        minus10Size.setButtonText("-");
        minus10Size.addListener(this);
        addAndMakeVisible(plus10Size);
        plus10Size.setButtonText("+");
        plus10Size.addListener(this);
        addAndMakeVisible(plus100Size);
        plus100Size.setButtonText("+++");
        plus100Size.addListener(this);

        addAndMakeVisible(randomizePosition);
        randomizePosition.setButtonText("Randomise Positions");
        randomizePosition.addListener(this);

        addAndMakeVisible(portInfo);
        portInfo.setText("Change Port", dontSendNotification);
        addAndMakeVisible(grainDensity);
        grainDensity.setText("Grain Density", dontSendNotification);
        addAndMakeVisible(grainSize);
        grainSize.setText("Grain Size", dontSendNotification);
    }

    void paint(Graphics& g) override
    {
        g.fillAll(juce::Colours::darkred);
    }

    void resized() override
    {
        Grid grid;

        using Track = Grid::TrackInfo;

        grid.templateColumns = { Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr)};
        grid.templateRows = { Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr)};
        grid.autoFlow = juce::Grid::AutoFlow::rowDense;
        grid.items = {
            GridItem{ portInfo }.withArea(1, 1),
            GridItem{ portOutText }.withArea(1, 2),
            GridItem{ portOutButton }.withArea(1, 3),
            GridItem{ portInText }.withArea(1, 4),
            GridItem{ portInButton }.withArea(1, 5),
            GridItem{ grainDensity}.withArea(2, 1),
            GridItem{ minus100Rate }.withArea(2, 2),
            GridItem{ minus10Rate }.withArea(2, 3),
            GridItem{ plus10Rate }.withArea(2, 4),
            GridItem{ plus100Rate }.withArea(2, 5),
            GridItem{ grainSize }.withArea(3, 1),
            GridItem{ minus100Size }.withArea(3, 2),
            GridItem{ minus10Size }.withArea(3, 3),
            GridItem{ plus10Size }.withArea(3, 4),
            GridItem{ plus100Size }.withArea(3, 5),
            GridItem{ randomizePosition }.withArea(4, GridItem::Span(5))
        };
        grid.performLayout(getLocalBounds());
    }

    void buttonClicked(Button* button) override
    {
        if (button == &portInButton) {
            listener->actionListenerCallback("portInChanged");
        }
        else if (button == &portOutButton) {
            listener->actionListenerCallback("portOutChanged");
        }

        else if (button == &minus100Rate) {
            setPropertyInAllNodes(Ids::frequency, -10, 0.03125f);
        }
        else if (button == &minus10Rate) {
            setPropertyInAllNodes(Ids::frequency, -1, 0.03125f);
        }
        else if (button == &plus10Rate) {
            setPropertyInAllNodes(Ids::frequency, +1, 200.0f);
        }
        else if (button == &plus100Rate) {
            setPropertyInAllNodes(Ids::frequency, +10, 200.0f);
        }

        else if (button == &minus100Size) {
            setPropertyInAllNodes(Ids::grainLength, -100, 5);
        }
        else if (button == &minus10Size) {
            setPropertyInAllNodes(Ids::grainLength, -10, 5);
        }
        else if (button == &plus10Size) {
            setPropertyInAllNodes(Ids::grainLength, +10, 5000);
        }
        else if (button == &plus100Size) {
            setPropertyInAllNodes(Ids::grainLength, +100, 5000);
        }

        else if (button == &randomizePosition) {
            for (auto child : tree) {
                child.setProperty(Ids::startTime, Random{}.nextDouble(), nullptr);
            }
        }
        getParentComponent()->visibilityChanged();
    }

    juce::String getPortInNumber() 
    {
        return portInText.getText();
    }

    juce::String getPortOutNumber() 
    {
        return portOutText.getText();
    }

private:
    void setPropertyInAllNodes(Identifier id, int toAdd, float minOrMaxValue) {
        for (auto child : tree) {
            if (toAdd < 0) {
                child.setProperty(id, fmax((float)child[id] + toAdd, minOrMaxValue), nullptr);
            }
            else {
                child.setProperty(id, fmin((float)child[id] + toAdd, minOrMaxValue), nullptr);
            }
        }
    }

    ActionListener* listener;
    juce::TextEditor portInText, portOutText;
    juce::TextButton portInButton, portOutButton;
    juce::TextButton minus100Rate, minus10Rate, plus10Rate, plus100Rate;
    juce::TextButton minus100Size, minus10Size, plus10Size, plus100Size;
    juce::TextButton randomizePosition;
    juce::Label portInfo, grainDensity, grainSize;
    juce::ValueTree tree;
};
