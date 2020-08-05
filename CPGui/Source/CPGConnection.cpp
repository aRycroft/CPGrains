/*
  ==============================================================================

    CPGConnection.cpp
    Created: 29 May 2020 12:30:21pm
    Author:  Alex

  ==============================================================================
*/

#include "CPGConnection.h"

CPGConnection::CPGConnection(CPGNode* parent, CPGNode* connectedTo, ValueTree conParams)
    :params(conParams)
{
    this->parent = parent;
    this->connectedTo = connectedTo;
}

CPGNode* CPGConnection::getConnected()
{
    return connectedTo;
}

int CPGConnection::getConnectedNumber()
{
    return connectedTo->getNodeNumber();
}

CPGNode* CPGConnection::getParent()
{
    return parent;
}

int CPGConnection::getParentNumber()
{
    return parent->getNodeNumber();
}

Identifier CPGConnection::getId()
{
    return parent->getComponentID() + connectedTo->getComponentID();
}

void CPGConnection::recalculatePath()
{
    double weight = params.getProperty(Ids::weight);
    double direction = params.getProperty(Ids::weightDir);
    double fWeight = weight * (1 - direction);
    weight *= direction;
    /*Probably nicer way to do this, this seems inefficient*/
    juce::Point<int> parentPos = CPGConnection::getCentre(parent);
    juce::Point<int> connectedPos = CPGConnection::getCentre(connectedTo);
    int distance = parentPos.getDistanceFrom(connectedPos);
    int bArrowLength = jmin(weight * 20 + 35, (double)distance / 2);
    int fArrowLength = jmin(fWeight * 20 + 35, (double)distance / 2);
    juce::Point<int> lineStart = parentPos + (connectedPos - parentPos) * ((double)fArrowLength / distance);
    juce::Point<int> lineEnd = connectedPos + (parentPos - connectedPos) * ((double)bArrowLength / distance);
    juce::Point<int> parentEdge = parentPos + (connectedPos - parentPos) * ((double)35.0 / distance);
    juce::Point<int> connectedEdge = connectedPos + (parentPos - connectedPos) * ((double)35.0 / distance);
    Line<int> betweenPoints{ lineStart, lineEnd };
    path.clear();
    path.addLineSegment(betweenPoints.toFloat(), 7.0f);
    path.addArrow(Line<int>{lineStart, parentEdge}.toFloat(), 7.0f, 40.0f, 500.0f);
    path.addArrow(Line<int>{lineEnd, connectedEdge}.toFloat(), 7.0f, 40.0f, 500.0f);
}

Path* CPGConnection::getPath()
{
    return &path;
}

Path* CPGConnection::getParameterPathBands() {
    return &bandPath;
}

bool CPGConnection::containsPoint(Point<float> point)
{
    return path.contains(point);
}

juce::Point<int> CPGConnection::getCentre(Component* comp)
{
    int parentX = comp->getPosition().getX() + comp->getWidth() / 2;
    int parentY = comp->getPosition().getY() + comp->getHeight() / 2;
    return juce::Point<int>(parentX, parentY);
}



