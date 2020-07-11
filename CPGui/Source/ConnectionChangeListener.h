/*
  ==============================================================================

    ConnectionChangeListener.h
    Created: 9 Jul 2020 5:26:51pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ParamSetter.h"
class ConnectionChangeListener : public ValueTree::Listener {
public:
    ConnectionChangeListener(ValueTree params, ParamSetter* paramSetter) :
        tree(params)
    {
        setter = paramSetter;
    }
    void valueTreePropertyChanged(ValueTree& vTree, const Identifier& property) override {
        if (vTree.getParent() == tree.getChild(0)) {
            int nodeId = vTree.getType().toString().getLastCharacter() - '0';
            setter->setParam(property.toString(), nodeId, vTree.getProperty(property));
        }
    };
private:
    ValueTree tree;
    ParamSetter* setter;
};
