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
    CPGConnection(Component* parent, Component* connectedTo)
    { 
        this->parent = parent;
        this->connectedTo = connectedTo; 
    };
    Component* getConnected() { return connectedTo; };
    Component* getParent() { return parent; };
    int weight = 0;
    void recalculatePath();
    float getAngle();
    Path getPath();
    float calculateWeight();
private:
    Component *connectedTo, *parent;
    Path path;
};