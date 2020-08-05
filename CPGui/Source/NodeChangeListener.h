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
#include "Identifiers.h"
class NodeChangeListener : public ValueTree::Listener {
public:
    NodeChangeListener(ValueTree params, ParamSetter* paramSetter) :
        tree(params)
    {
        setter = paramSetter;
    }
    void valueTreePropertyChanged(ValueTree& vTree, const Identifier& property) override {
        if (property == Ids::grainLength || property == Ids::startTime || property == Ids::frequency || property == Ids::pan || property == Ids::volume) {
            int nodeID = vTree.getType().toString().getLastCharacter() - '0';
            setter->setParam(property.toString(), nodeID, vTree.getProperty(property));
        }
    };
private:
    ValueTree tree;
    ParamSetter *setter;
};
