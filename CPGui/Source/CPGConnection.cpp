/*
  ==============================================================================

    CPGConnection.cpp
    Created: 29 May 2020 12:30:21pm
    Author:  Alex

  ==============================================================================
*/

#include "CPGConnection.h"

CPGConnection::CPGConnection(Component* parent, Component* connectedTo)
{
    this->parent = parent;
    this->connectedTo = connectedTo;
}

Component* CPGConnection::getConnected()
{
    return connectedTo;
}

Component* CPGConnection::getParent()
{
    return parent;
}

Identifier CPGConnection::getId()
{
    return parent->getComponentID() + connectedTo->getComponentID();
}

void CPGConnection::recalculatePath()
{
    juce::Point<int> parentPos = CPGConnection::getCentre(parent);
    juce::Point<int> connectedPos = CPGConnection::getCentre(connectedTo);
    Line<int> arrow{ parentPos, connectedPos };
    juce::Point<int> parentEdge = parentPos + (connectedPos - parentPos) * (40.0f / arrow.getLength());
    juce::Point<int> connectedEdge = connectedPos + (parentPos - connectedPos) * (40.0f / arrow.getLength());
    Line<int> newArrow{ parentEdge, connectedEdge };

    Path p;
    p.addArrow(newArrow.toFloat(), 5.0f, 20.0f, 50.0f);
    path = p;
}

Path CPGConnection::getPath()
{
    return path;
}

float CPGConnection::calculateWeight(double mult)
{
    juce::Point<int> pos1 = CPGConnection::getCentre(parent);
    juce::Point<int> pos2 = CPGConnection::getCentre(connectedTo);

    float radius = parent->getBounds().getWidth() / 2;
    float weight = sqrt(pow(pos2.getX() - pos1.getX(), 2) + pow(pos2.getY() - pos1.getY(), 2) * 1.0f) - radius * 2;
    int strength = 200 * mult;
    return std::max<float>(1 + (-1 * (weight - 0) / strength), 0.0f);
}

juce::Point<int> CPGConnection::getCentre(Component* comp)
{
    int parentX = comp->getPosition().getX() + comp->getWidth() / 2;
    int parentY = comp->getPosition().getY() + comp->getHeight() / 2;
    return juce::Point<int>(parentX, parentY);
}

