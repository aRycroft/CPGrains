/*
  ==============================================================================

    OSCParamSetter.cpp
    Created: 24 May 2020 10:50:52am
    Author:  Alex

  ==============================================================================
*/

#include "OSCParamSetter.h"

OSCParamSetter::OSCParamSetter()
{
    if (!sender.connect("127.0.0.1", 8080)) {
        OSCException("Can't connect");
    }
}

OSCParamSetter::OSCParamSetter(int portNumber)
{
    if (!sender.connect("127.0.0.1", portNumber)) {
        OSCException("Can't connect");
    }
}

void OSCParamSetter::setWeight(int currentNode, int targetNode, float weight)
{
    String msg;
    msg << "/weight/conn/" << currentNode + 1 << "/" << targetNode + 1;
    sender.send(msg, weight);
}

void OSCParamSetter::setParam(String toSet, int nodeNo, float value)
{
    String msg;
    msg << "/control/" << toSet << "/" << nodeNo + 1;
    sender.send(msg, value);
}

void OSCParamSetter::setConParam(String toSet, int from, int to, float value) {
    String msg;
    msg << "/Concontrol/" << from + 1 << "/" << toSet << "/" << to + 1;
    sender.send(msg, value);
}

void OSCParamSetter::setFile(String filePath)
{
    String msg;
    msg << "/file/";
    sender.send(msg, filePath);
}

void OSCParamSetter::setDSPState(bool DSPon)
{
    String msg;
    msg << "/DSP/";
    sender.send(msg, (int)DSPon);
}
