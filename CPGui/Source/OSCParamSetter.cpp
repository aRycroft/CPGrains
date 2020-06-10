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
    msg << "/weight/conn/" << targetNode << "/" << currentNode;
    sender.send(msg, weight);
}

void OSCParamSetter::setParam(String toSet, int nodeNo, float value)
{
    String msg;
    msg << "/control/" << toSet << "/" << nodeNo;
    sender.send(msg, value);
}

void OSCParamSetter::setFile(String filePath)
{
    String msg;
    msg << "/file/";
    sender.send(msg, filePath);
}
