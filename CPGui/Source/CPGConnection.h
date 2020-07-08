/*
  ==============================================================================

    CPGConnection.h
    Created: 29 May 2020 12:30:21pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
class CPGConnection
{
public:
    CPGConnection(Component* parent, Component* connectedTo);
    Component* getConnected();
    Component* getParent();
    Identifier getId();
    int weight = 0;
    void recalculatePath(ValueTree* conValues);
    Path* getPath();
    float calculateWeight(double mult);
    bool containsPoint(Point<float> p);
private:
    static Point<int> getCentre(Component* comp);
    Component *connectedTo, *parent;
    Path path;
};