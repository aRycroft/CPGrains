/*
  ==============================================================================

    MixerChangeListener.h
    Created: 27 Jul 2020 11:45:59am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ParamSetter.h"
class MixerChangeListener : public ValueTree::Listener {
public:
    MixerChangeListener(ValueTree params, ParamSetter* paramSetter) :
        tree(params)
    {
        setter = paramSetter;
    }
    void valueTreePropertyChanged(ValueTree& vTree, const Identifier& property) override {
        int nodeID =  vTree.getType().toString().getLastCharacter() - '0';
        setter->setParam(property.toString(), nodeID, vTree.getProperty(property));
    };
private:
    ValueTree tree;
    ParamSetter* setter;
    
};
