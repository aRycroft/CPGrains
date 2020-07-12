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
    CPGConnection(Component* parent, Component* connectedTo, ValueTree conParams);
    Component* getConnected();
    Component* getParent();
    Identifier getId();
    int weight = 0;
    void recalculatePath();
    Path* getPath();
    Path* getParameterPathBands();
    float calculateWeight(double mult);
    double getPropertyValue(Identifier i);
    bool containsPoint(Point<float> p);
    void setParam(Identifier i, double val);
private:
    static Point<int> getCentre(Component* comp);
    void addParams();
    Component *connectedTo, *parent;
    Path path, bandPath;
    ValueTree params;
};