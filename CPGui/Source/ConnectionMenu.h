/*
  ==============================================================================

    ConnectionMenu.h
    Created: 11 Jul 2020 1:16:28pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class ConnectionMenu : public PopupMenu {
public:
    ConnectionMenu(ValueTree params, int* clickedCon, Component* parent):
        conParams(params)
    {
        this->clickedCon = clickedCon;
        this->parent = parent;
        setUpConnectionMenu();
    }

    int showMenu() {
        if (weightButton.getToggleState()) {
            changeMenuSliders("weight", "weightDir", 0.0, 10.0);
        }
        else if (lengthButton.getToggleState()) {
            changeMenuSliders("lengthMod", "lengthModDir", 0.0, 2000.0);
        }
        else {
            changeMenuSliders("startMod", "startModDir", 0.0, 5000.0);
        }
        const int result = this->show();
        return result;
    }

    void changeMenuSliders(String weight, String direction, double startValue, double endValue)
    {
        weightSlider.setValue(conParams.getChild(*clickedCon).getProperty(weight), dontSendNotification);
        weightSlider.setNormalisableRange(NormalisableRange<double>(startValue, endValue, 0.01, 1.0));
        directionSlider.setValue(conParams.getChild(*clickedCon).getProperty(direction), dontSendNotification);
        weightSlider.onValueChange = [this, weight] {
            conParams.getChild(*clickedCon).setProperty(weight, weightSlider.getValue(), nullptr);
            parent->repaint();
        };
        directionSlider.onValueChange = [this, direction] {
            conParams.getChild(*clickedCon).setProperty(direction, directionSlider.getValue(), nullptr);
            parent->repaint();
        };
     
    }
private:
    void setUpConnectionMenu()
    {
        /*This is pretty bad*/
        this->addItem(1, "Delete Connection");
        weightSlider.setNormalisableRange(NormalisableRange<double>(0.0, 10.0, 0.01, 1.0));
        this->addCustomItem(-1, weightSlider, 50, 60, false);
        directionSlider.setNormalisableRange(NormalisableRange<double>(0.0, 1.0, 0.01, 1.0));
        this->addCustomItem(-1, directionSlider, 50, 60, false);
        weightButton.onClick = [this] {
            lengthButton.setToggleState(false, dontSendNotification);
            positionButton.setToggleState(false, dontSendNotification);
            weightButton.setToggleState(true, dontSendNotification);
            changeMenuSliders("weight", "weightDir", 0.0, 10.0);
        };
        lengthButton.onClick = [this] {
            weightButton.setToggleState(false, dontSendNotification);
            positionButton.setToggleState(false, dontSendNotification);
            lengthButton.setToggleState(true, dontSendNotification);
            changeMenuSliders("lengthMod", "lengthModDir", 0.0, 2000.0);
        };
        positionButton.onClick = [this] {
            weightButton.setToggleState(false, dontSendNotification);
            lengthButton.setToggleState(false, dontSendNotification);
            positionButton.setToggleState(true, dontSendNotification);
            changeMenuSliders("startMod", "startModDir", 0.0, 5000.0);
        };
        weightButton.setToggleState(true, dontSendNotification);
        this->addCustomItem(-1, weightButton, 50, 50, false);
        this->addCustomItem(-1, lengthButton, 50, 50, false);
        this->addCustomItem(-1, positionButton, 50, 50, false);
        changeMenuSliders("weight", "weightDir", 0.0, 10.0);
    }


    Slider weightSlider, directionSlider;
    ToggleButton weightButton, lengthButton, positionButton;
    ValueTree conParams;
    int* clickedCon;
    Component* parent;
};