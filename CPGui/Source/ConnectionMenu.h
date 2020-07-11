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
    ConnectionMenu(ValueTree params, int* clickedCon):
        conParams(params)
    {
        this->clickedCon = clickedCon;
        setUpConnectionMenu();
    }

    int showMenu() {
        if (weightButton.getToggleState()) {
            changeMenuSliders("weight", "weightDir");
        }
        else if (lengthButton.getToggleState()) {
            changeMenuSliders("lengthMod", "lengthModDir");
        }
        else {
            changeMenuSliders("startMod", "startModDir");
        }
        const int result = this->show();
        return result;
    }

    void changeMenuSliders(String weight, String direction)
    {
        weightSlider.setValue(conParams.getChild(*clickedCon).getProperty(weight), dontSendNotification);
        directionSlider.setValue(conParams.getChild(*clickedCon).getProperty(direction), dontSendNotification);
        weightSlider.onValueChange = [this, weight] {
            conParams.getChild(*clickedCon).setProperty(weight, weightSlider.getValue(), nullptr);
        };
        directionSlider.onValueChange = [this, direction] {
            conParams.getChild(*clickedCon).setProperty(direction, directionSlider.getValue(), nullptr);
        };
     
    }
private:
    void setUpConnectionMenu()
    {
        /*This is pretty bad*/
        this->addItem(1, "Delete Connection");
        weightSlider.setNormalisableRange(NormalisableRange<double>(0.0, 10.0, 0.01, 1.0));
        this->addCustomItem(-1, weightSlider, 50, 60, false);
        directionSlider.setName("direction");
        directionSlider.setNormalisableRange(NormalisableRange<double>(0.0, 1.0, 0.01, 1.0));
        this->addCustomItem(-1, directionSlider, 50, 60, false);
        weightButton.onClick = [this] {
            lengthButton.setToggleState(false, dontSendNotification);
            positionButton.setToggleState(false, dontSendNotification);
            weightButton.setToggleState(true, dontSendNotification);
            changeMenuSliders("weight", "weightDir");
        };
        lengthButton.onClick = [this] {
            weightButton.setToggleState(false, dontSendNotification);
            positionButton.setToggleState(false, dontSendNotification);
            lengthButton.setToggleState(true, dontSendNotification);
            changeMenuSliders("lengthMod", "lengthModDir");
        };
        positionButton.onClick = [this] {
            weightButton.setToggleState(false, dontSendNotification);
            lengthButton.setToggleState(false, dontSendNotification);
            positionButton.setToggleState(true, dontSendNotification);
            changeMenuSliders("startMod", "startModDir");
        };
        weightButton.setToggleState(true, dontSendNotification);
        this->addCustomItem(-1, weightButton, 50, 50, false);
        this->addCustomItem(-1, lengthButton, 50, 50, false);
        this->addCustomItem(-1, positionButton, 50, 50, false);
        changeMenuSliders("weight", "weightDir");
    }


    Slider weightSlider, directionSlider;
    ToggleButton weightButton, lengthButton, positionButton;
    ValueTree conParams;
    int* clickedCon;
};