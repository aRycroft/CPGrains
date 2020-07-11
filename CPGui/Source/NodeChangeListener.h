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
    void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override {
        if (treeWhosePropertyHasChanged.getParent() == tree.getChild(0)) {
            int nodeId = treeWhosePropertyHasChanged.getType().toString().getLastCharacter() - '0';
            setter->setParam(property.toString(), nodeId, treeWhosePropertyHasChanged.getProperty(property));
        }
    };
private:
    ValueTree tree;
    ParamSetter *setter;
};
