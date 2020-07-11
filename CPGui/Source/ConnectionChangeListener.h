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
        int from = vTree.getProperty("from");
        int to = vTree.getProperty("to");
        if (property == weight || property == weightDir) {
            float weight = vTree.getProperty("weight");
            float dir = vTree.getProperty("weightDir");
            sendWeight(from, to, weight * dir);
            sendWeight(to, from, weight * (1 - dir));
        }
        else if (property == lengthMod || property == lengthModDir) {
            float weight = vTree.getProperty("lengthMod");
            float dir = vTree.getProperty("lengthModDir");
            setter->setConParam("lengthWeight", from, to, weight * dir);
            setter->setConParam("flengthWeight", to, from, weight * (1 - dir));
        }
        else if (property == startMod || property == startModDir) {
            float weight = vTree.getProperty("startMod");
            float dir = vTree.getProperty("startModDir");
            setter->setConParam("startWeight", from, to, weight * dir);
            setter->setConParam("fstartWeight", to, from, weight * (1 - dir));
        }
        
    };
    void sendWeight(int from, int to, double mult)
    {
        int fromX = tree.getChild(0).getChild(from).getProperty("x");
        int fromY = tree.getChild(0).getChild(from).getProperty("y");

        int toX = tree.getChild(0).getChild(to).getProperty("x");
        int toY = tree.getChild(0).getChild(to).getProperty("y");
        float radius = NODESIZE / 2;

        juce::Point<int> pos1 = getCentre(juce::Point<int>{fromX, fromY}, radius);
        juce::Point<int> pos2 = getCentre(juce::Point<int>{toX, toY}, radius);

        float weight = sqrt(pow(pos2.getX() - pos1.getX(), 2) + pow(pos2.getY() - pos1.getY(), 2) * 1.0f) - radius * 2;
        int strength = 200 * mult;
        setter->setWeight(from, to, std::max<float>(1 + (-1 * (weight - 0) / strength), 0.0f));
    }
private:
    juce::Point<int> getCentre(juce::Point<int> comp, int radius)
    {
        int parentX = comp.getX() + radius;
        int parentY = comp.getY() + radius;
        return juce::Point<int>(parentX, parentY);
    }

    ValueTree tree;
    ParamSetter* setter;
    Identifier weight { "weight" };
    Identifier weightDir { "weightDir" };
    Identifier lengthMod { "lengthMod" };
    Identifier lengthModDir { "lengthModDir" };
    Identifier startMod { "startMod" };
    Identifier startModDir { "startModDir" };
};
