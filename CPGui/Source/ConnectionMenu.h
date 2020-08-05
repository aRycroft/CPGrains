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
        weightSlider.setNormalisableRange(NormalisableRange<double>(startValue, endValue, 0.01, 1.0));
        weightSlider.setValue(conParams.getChild(*clickedCon).getProperty(weight), dontSendNotification);
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

    void setParams(ValueTree newParams) {
        conParams.copyPropertiesAndChildrenFrom(newParams, nullptr);
    }
private:
    void setUpConnectionMenu()
    {
        int height = 20;
        /*This is pretty bad*/
        this->addItem(1, "Delete Connection");
        weightSlider.setNormalisableRange(NormalisableRange<double>(0.0, 10.0, 0.01, 1.0));
        weightLabel.setText("Weight", dontSendNotification);
        this->addCustomItem(-1, weightLabel, 50, height, false);
        this->addCustomItem(-1, weightSlider, 50, height, false);
        directionSlider.setNormalisableRange(NormalisableRange<double>(0.0, 1.0, 0.01, 1.0));
        directionLabel.setText("Direction", dontSendNotification);
        this->addCustomItem(-1, directionLabel, 50, height, false);
        this->addCustomItem(-1, directionSlider, 50, height, false);
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
        weightBLabel.setText("CPG Connection", dontSendNotification);
        this->addCustomItem(-1, weightBLabel, 50, height, false);
        this->addCustomItem(-1, weightButton, 50, height, false);
        lengthBLabel.setText("Grain Size Modifier (Multiplied by CPG signal)", dontSendNotification);
        this->addCustomItem(-1, lengthBLabel, 50, height, false);
        this->addCustomItem(-1, lengthButton, 50, height, false);
        posBLabel.setText("Grain Position Modifier (Multiplied by CPG signal)", dontSendNotification);
        this->addCustomItem(-1, posBLabel, 50, height, false);
        this->addCustomItem(-1, positionButton, 50, height, false);
        changeMenuSliders("weight", "weightDir", 0.0, 10.0);
    }
    Slider weightSlider, directionSlider;
    ToggleButton weightButton, lengthButton, positionButton;
    Label weightLabel, directionLabel, weightBLabel, lengthBLabel, posBLabel;
    ValueTree conParams;
    int* clickedCon;
    Component* parent;
};