/*
  ==============================================================================

    CPGConnection.cpp
    Created: 29 May 2020 12:30:21pm
    Author:  Alex

  ==============================================================================
*/

#include "CPGConnection.h"

void CPGConnection::recalculatePath()
{
    Point<int> parentPos = CPGConnection::getCentre(parent);
    Point<int> connectedPos = CPGConnection::getCentre(connectedTo);
    Line<int> arrow{ parentPos, connectedPos };
    Point<int> parentEdge = parentPos + (connectedPos - parentPos) * (40.0f / arrow.getLength());
    Point<int> connectedEdge = connectedPos + (parentPos - connectedPos) * (40.0f / arrow.getLength());
    Line<int> newArrow{ parentEdge, connectedEdge };

    Path p;
    /*p.addQuadrilateral(parentPos.getX(), parentPos.getY(), 
        parentPos.getX(), parentPos.getY(), 
        connectedPos.getX(), connectedPos.getY(),
        connectedPos.getX(), connectedPos.getY());*/
    p.addArrow(newArrow.toFloat(), 5.0f, 20.0f, 50.0f);
    path = p;
}

float CPGConnection::getAngle() {
    return parent->getPosition().getAngleToPoint(connectedTo->getPosition());
}

Path CPGConnection::getPath()
{
    return path;
}

float CPGConnection::calculateWeight() {
    /*Point<int> pos1 = parent->getPosition();
    Point<int> pos2 = connectedTo->getPosition();

    int parentX = pos1.getX() + parent->getWidth() / 2;
    int connectedX = pos2.getX() + connectedTo->getWidth() / 2;
    int parentY = pos1.getY() + parent->getHeight() / 2;
    int connectedY = pos2.getY() + connectedTo->getHeight() / 2;
    */
    Point<int> pos1 = CPGConnection::getCentre(parent);
    Point<int> pos2 = CPGConnection::getCentre(connectedTo);
    
    float weight = sqrt(pow(pos2.getX() - pos1.getX(), 2) + pow(pos2.getY() - pos1.getY(), 2) * 1.0);
    return std::max<float>(1 + (-1 * (weight - 0) / 300), 0.0f);
}

Point<int> CPGConnection::getCentre(Component* comp)
{
    int parentX = comp->getPosition().getX() + comp->getWidth() / 2;
    int parentY = comp->getPosition().getY() + comp->getHeight() / 2;
    return Point<int>(parentX, parentY);
}

