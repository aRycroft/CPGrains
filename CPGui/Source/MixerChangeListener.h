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
        setter->setParam(property.toString(), vTree.getType().toString().getIntValue(), vTree.getProperty(property));
    };
private:
    ValueTree tree;
    ParamSetter* setter;
    
};
