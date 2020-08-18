/*
  ==============================================================================

    CPGTriggerComponent.h
    Created: 17 Aug 2020 4:44:06pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class CPGTriggerComponent : public Component
{
public:
    CPGTriggerComponent(juce::Colour colour) {
        this->colour = colour;
    }
    void paint(Graphics& g) override {
        auto area = getLocalBounds().reduced(5);
        g.setColour(colour);
        g.drawRoundedRectangle(area.toFloat(), 100.0f, 5.0f);
    };
private:
    juce::Colour colour;
};