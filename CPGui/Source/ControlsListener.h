/*
  ==============================================================================

    ControlsListener.h
    Created: 4 Jun 2020 11:03:40am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "ParamSetter.h"
class ControlsListener : public Slider::Listener{
public:
    ControlsListener(ParamSetter* setter){ 
        this->setter = setter; 
    };
    void sliderValueChanged(Slider* slider) override {
        setter->setParam(slider->getComponentID(), 0, slider->getValue());
    };
private:
    ParamSetter* setter;
};