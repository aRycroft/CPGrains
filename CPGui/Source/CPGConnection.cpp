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
    addParams();
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
    
    double weight = params.getPropertyAsValue("weight", nullptr).getValue();
    double direction = params.getPropertyAsValue("direction", nullptr).getValue();
    double fWeight = weight * (1 - direction);
    weight *= direction;
    /*Probably nicer way to do this*/
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
    path.addLineSegment(betweenPoints.toFloat(), 5.0f);
    path.addArrow(Line<int>{lineStart, parentEdge}.toFloat(), 5.0f, 40.0f, 500.0f);
    path.addArrow(Line<int>{lineEnd, connectedEdge}.toFloat(), 5.0f, 40.0f, 500.0f);
    bandPath.clear();
    double distanceFromLineStart = jmin(20.0 / betweenPoints.getLength(), (double)betweenPoints.getLength() / 2);
    juce::Point<int> bandPoint = lineStart + (lineEnd - lineStart) * distanceFromLineStart;
    juce::Line<int> band1 {lineStart, bandPoint};
    band1.applyTransform(AffineTransform::rotation(MathConstants<float>::halfPi, bandPoint.getX(), bandPoint.getY()));
    bandPath.addLineSegment(band1.toFloat(), 5.0f);
    juce::Line<int> band2{ lineStart, bandPoint };
    band2.applyTransform(AffineTransform::rotation(MathConstants<float>::halfPi, bandPoint.getX(), bandPoint.getY()));
    bandPath.addLineSegment(band2.toFloat(), 5.0f);
}

Path* CPGConnection::getPath()
{
    return &path;
}

Path* CPGConnection::getParameterPathBands() {
    return &bandPath;
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

double CPGConnection::getPropertyValue(Identifier i)
{
    return params.getPropertyAsValue(i, nullptr).getValue();
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

void CPGConnection::addParams()
{
    params.setProperty("weight", 1.0, nullptr);
    params.setProperty("direction", 0.0, nullptr);
    params.setProperty("lengthWeight", 1.0, nullptr);
    params.setProperty("lengthDirection", 0.0, nullptr);
    params.setProperty("posWeight", 1.0, nullptr);
    params.setProperty("posDirection", 0.0, nullptr);
}

void CPGConnection::setParam(Identifier i, double val)
{
    params.setProperty(i, val, nullptr);
}


