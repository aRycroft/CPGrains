/*
  ==============================================================================

    OSCParamSetter.h
    Created: 24 May 2020 10:50:52am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "ParamSetter.h"
#include "CPGNode.h"

class OSCParamSetter : public ParamSetter{
public:
    OSCParamSetter();
    OSCParamSetter(int portNumber);
    void setWeight(int currentNode, int targetNode, float weight) override;
    void setParam(String toSet, int nodeNo, float value) override;
    void setConParam(String toSet, int from, int to, float value) override;
    void setFile(String filePath) override;
    void setDSPState(bool DSPon) override;
private:
    OSCSender sender;
    int tick = 0;
};