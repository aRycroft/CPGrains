/*
  ==============================================================================

    ConnectionChangeListener.h
    Created: 9 Jul 2020 5:26:51pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ParamSetter.h"
#include "Identifiers.h"

class ConnectionChangeListener : public ValueTree::Listener 
{
public:
    ConnectionChangeListener(ValueTree params, ParamSetter* paramSetter) 
        :tree(params)
    {
        setter = paramSetter;
    }

    void valueTreePropertyChanged(ValueTree& vTree, const Identifier& property) override 
    {
        int fromNo = vTree.getProperty(Ids::from);
        int toNo = vTree.getProperty(Ids::to);
        if (property == Ids::weight || property == Ids::weightDir) {
            float fWeight = vTree.getProperty(Ids::weight);
            float dir = vTree.getProperty(Ids::weightDir);
            sendWeight(fromNo, toNo, fWeight, dir);
        }
        else if (property == Ids::lengthMod || property == Ids::lengthModDir) {
            float weight = vTree.getProperty(Ids::lengthMod);
            float dir = vTree.getProperty(Ids::lengthModDir);
            setter->setConParam("lengthWeight", fromNo, toNo, weight * dir);
            setter->setConParam("flengthWeight", toNo, fromNo, weight * (1 - dir));
        }
        else if (property == Ids::startMod || property == Ids::startModDir) {
            float weight = vTree.getProperty(Ids::startMod);
            float dir = vTree.getProperty(Ids::startModDir);
            setter->setConParam("startWeight", fromNo, toNo, weight * dir);
            setter->setConParam("fstartWeight", toNo, fromNo, weight * (1 - dir));
        }   
    }

    void sendWeight(int from, int to, double mult, double dir)
    {
        int fromX = tree.getChild(0).getChild(from).getProperty(Ids::x);
        int fromY = tree.getChild(0).getChild(from).getProperty(Ids::y);

        int toX = tree.getChild(0).getChild(to).getProperty(Ids::x);
        int toY = tree.getChild(0).getChild(to).getProperty(Ids::y);
        float radius = NODESIZE / 2;

        juce::Point<int> pos1 = getCentre(juce::Point<int>{fromX, fromY}, radius);
        juce::Point<int> pos2 = getCentre(juce::Point<int>{toX, toY}, radius);

        float weight = sqrt(pow(pos2.getX() - pos1.getX(), 2) + pow(pos2.getY() - pos1.getY(), 2) * 1.0f) - radius * 2;
        int strength = 200 * mult * dir;
        setter->setWeight(to, from, std::max<float>(1 + (-1 * weight / strength), 0.0f));
        strength = 200 * mult * (1 - dir);
        setter->setWeight(from, to, std::max<float>(1 + (-1 * weight / strength), 0.0f));
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
};
