/*
  ==============================================================================

    CPGConnection.h
    Created: 29 May 2020 12:30:21pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "CPGNode.h"
#include "Identifiers.h"

class CPGConnection 
{
public:
    CPGConnection(CPGNode* parent, CPGNode* connectedTo, ValueTree conParams);
    CPGNode* getConnected();
    int getConnectedNumber();
    CPGNode* getParent();
    int getParentNumber();
    Identifier getId();
    void recalculatePath();
    Path* getPath();
    Path* getParameterPathBands();
    bool containsPoint(Point<float> p);
private:
    static Point<int> getCentre(Component* comp);
    CPGNode *connectedTo, *parent;
    Path path, bandPath;
    ValueTree params;
};