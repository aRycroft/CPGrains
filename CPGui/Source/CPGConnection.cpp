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
    /*This should be a function but I'm lazy*/
    weight = params[Ids::lengthMod];
    direction = params[Ids::lengthModDir];
    fWeight = weight * (1 - direction);
    weight *= direction;
    sizePath.clear();
    lengthPath.clear();
    if (betweenPoints.getLength() >= 20) {
        if (fWeight) {
            Line<int> band1{ betweenPoints.getStart() + (betweenPoints.getEnd() - betweenPoints.getStart()) *
                (10.0 / betweenPoints.getLength()),
                    betweenPoints.getStart() + (betweenPoints.getEnd() - betweenPoints.getStart()) *
                (20.0 / betweenPoints.getLength()) };
            AffineTransform rotate90;
            rotate90 = rotate90.rotation(float_Pi / 2, band1.getStartX(), band1.getStartY());
            band1.applyTransform(rotate90);
            lengthPath.addLineSegment(band1.toFloat(), 10.0f);
        }
        if (weight) {
            Line<int> band1{ betweenPoints.getEnd() + (betweenPoints.getStart() - betweenPoints.getEnd()) *
                (10.0 / betweenPoints.getLength()),
                    betweenPoints.getEnd() + (betweenPoints.getStart() - betweenPoints.getEnd()) *
                (20.0 / betweenPoints.getLength()) };
            AffineTransform rotate90;
            rotate90 = rotate90.rotation(float_Pi / 2, band1.getStartX(), band1.getStartY());
            band1.applyTransform(rotate90);
            lengthPath.addLineSegment(band1.toFloat(), 10.0f);
        }
        weight = params[Ids::startMod];
        direction = params[Ids::startModDir];
        fWeight = weight * (1 - direction);
        weight *= direction;

        if (fWeight) {
            Line<int> band1{ betweenPoints.getStart() + (betweenPoints.getEnd() - betweenPoints.getStart()) *
                (20.0 / betweenPoints.getLength()),
                    betweenPoints.getStart() + (betweenPoints.getEnd() - betweenPoints.getStart()) *
                (30.0 / betweenPoints.getLength()) };
            AffineTransform rotate90;
            rotate90 = rotate90.rotation(float_Pi / 2, band1.getStartX(), band1.getStartY());
            band1.applyTransform(rotate90);
            sizePath.addLineSegment(band1.toFloat(), 10.0f);
        }
        if (weight) {
            Line<int> band1{ betweenPoints.getEnd() + (betweenPoints.getStart() - betweenPoints.getEnd()) *
                (20.0 / betweenPoints.getLength()),
                    betweenPoints.getEnd() + (betweenPoints.getStart() - betweenPoints.getEnd()) *
                (30.0 / betweenPoints.getLength()) };
            AffineTransform rotate90;
            rotate90 = rotate90.rotation(float_Pi / 2, band1.getStartX(), band1.getStartY());
            band1.applyTransform(rotate90);
            sizePath.addLineSegment(band1.toFloat(), 10.0f);
        }
    }

    /*
    betweenPoints.setStart(betweenPoints.getStart() + (betweenPoints.getEnd() - betweenPoints.getStart()) *
        (10.0 / betweenPoints.getLength()));
    Point<int> end = betweenPoints.getStart() + (betweenPoints.getEnd() - betweenPoints.getStart()) * 
        ((double)fWeight / betweenPoints.getLength());
    Line<int> band1{ betweenPoints.getStart(), end };*/


    
}

Path* CPGConnection::getPath()
{
    return &path;
}

Path* CPGConnection::getLengthPath()
{
    return &lengthPath;
}

Path* CPGConnection::getSizePath()
{
    return &sizePath;
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



