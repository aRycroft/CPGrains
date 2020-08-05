/*
  ==============================================================================

    GrainPositionSliderBox.h
    Created: 23 Jul 2020 3:16:41pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class GrainPositionSliderBox : public Component, public ValueTree::Listener {
public:
    GrainPositionSliderBox(juce::ValueTree tree, juce::Colour boxColour)
        :nodeTree(tree){
        colour = boxColour;
        nodeTree.addListener(this);
    };
    void paint(Graphics& g) override {
        g.fillAll(colour);
    };
    void resized() override {
        constrainer.setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
    };
    void mouseDown(const MouseEvent& e) override {
        dragger.startDraggingComponent(this, e);
    }
    void mouseDrag(const MouseEvent& e) override {
        //constrainer.setBoundsForComponent(this, Rectangle<int>{getParentWidth(), getHeight()}, true, true, true, true);
        dragger.dragComponent(this, e, &constrainer);
        Rectangle<int> bounds = getBoundsInParent();
        constrainer.checkBounds(bounds, getBoundsInParent(), Rectangle<int>{getParentWidth(), getHeight()},
            false, false, false, false);
        setBounds(bounds);
        nodeTree.setProperty("startTime", (double)getX() / (double)getParentWidth(), nullptr);
        DBG(nodeTree.toXmlString());
    }
    void valueTreePropertyChanged(ValueTree& vTree, const Identifier& property) override {
        if (property == startTime) {
            setTopLeftPosition(juce::Point<double>{(double)getParentWidth() * (double)vTree.getProperty(property), 0}.toInt());
        }
    };
private:
    ComponentBoundsConstrainer constrainer;
    ComponentDragger dragger;
    juce::Colour colour;
    juce::ValueTree nodeTree;
    Identifier startTime{ "startTime" };
};

/*class HorizontalBoundsConstrainer : public ComponentBoundsConstrainer {
public:
    void checkBounds() override {


    };
};*/
