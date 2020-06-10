/*
  ==============================================================================

    ParamSetter.h
    Created: 24 May 2020 10:47:11am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class ParamSetter
{
public:
    virtual void setWeight(int currentNode, int targetNode, float weight) = 0;
    virtual void setParam(String toSet, int nodeNo, float value) = 0;
    virtual void setFile(String filePath) = 0;
};