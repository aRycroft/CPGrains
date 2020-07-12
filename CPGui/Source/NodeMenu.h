/*
  ==============================================================================

    ConnectionMenu.h
    Created: 11 Jul 2020 1:16:28pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class NodeMenu : public PopupMenu {
public:
    NodeMenu(ValueTree params, int* clickedNode) :
        nodeParams(params)
    {
        this->clickedNode = clickedNode;
        setUpMenu();
    }

    int showMenu() {
        size.setValue(nodeParams.getChild(*clickedNode).getProperty("grainLength"), dontSendNotification);
        pos.setValue(nodeParams.getChild(*clickedNode).getProperty("startTime"), dontSendNotification);
        frequency.setValue(nodeParams.getChild(*clickedNode).getProperty("frequency"), dontSendNotification);
        size.onValueChange = [this] {
            nodeParams.getChild(*clickedNode).setProperty("grainLength", size.getValue(), nullptr);
        };
        pos.onValueChange = [this] {
            nodeParams.getChild(*clickedNode).setProperty("startTime", pos.getValue(), nullptr);
        };
        frequency.onValueChange = [this] {
            nodeParams.getChild(*clickedNode).setProperty("frequency", frequency.getValue(), nullptr);
        };
        const int result = this->show();
        return result;
    }

private:
    void setUpMenu() {
        size.setNormalisableRange(NormalisableRange<double>(5.0f, 5000.0f, 0.1f, 0.25f));
        this->addCustomItem(-1, size, 200, 60, false);
        pos.setNormalisableRange(NormalisableRange<double>(0.0f, 1000.0f, 0.01f, 1.0f));
        this->addCustomItem(-1, pos, 200, 60, false);
        frequency.setNormalisableRange(NormalisableRange<double>(0.03125f, 200.0f, 0.01f, 1.0f));
        this->addCustomItem(-1, frequency, 200, 60, false);
    }

    Slider size, pos, frequency;
    ValueTree nodeParams;
    int* clickedNode;
};