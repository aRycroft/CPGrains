/*
  ==============================================================================

    NodeChangeListener.h
    Created: 9 Jul 2020 4:47:57pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ParamSetter.h"
class NodeChangeListener : public ValueTree::Listener {
public:
    NodeChangeListener(ValueTree params, ParamSetter* paramSetter) :
        tree(params)
    {
        setter = paramSetter;
    }
    void valueTreePropertyChanged(ValueTree& vTree, const Identifier& property) override {
        if (property == grainLength || property == startTime || property == frequency) {
            setter->setParam(property.toString(), vTree.getType().toString().getIntValue(), vTree.getProperty(property));
        }
    };
private:
    ValueTree tree;
    ParamSetter *setter;
    Identifier grainLength{ "grainLength" };
    Identifier startTime{ "startTime" };
    Identifier frequency{ "frequency" };
};
