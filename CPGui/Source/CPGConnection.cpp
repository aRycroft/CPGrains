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
    Point<int> parentPos = parent->getPosition();
    Point<int> connectedPos = connectedTo->getPosition();
    Line<int> arrow{ parentPos, connectedPos };
    Path p;
    /*p.addQuadrilateral(parentPos.getX(), parentPos.getY(), 
        parentPos.getX(), parentPos.getY(), 
        connectedPos.getX(), connectedPos.getY(),
        connectedPos.getX(), connectedPos.getY());*/
    p.addArrow(arrow.toFloat(), 5.0f, 20.0f, 50.0f);
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
    Point<int> pos1 = parent->getPosition();
    Point<int> pos2 = connectedTo->getPosition();
    float weight = sqrt(pow(pos2.getX() - pos1.getX(), 2) + pow(pos2.getY() - pos1.getY(), 2) * 1.0);
    return std::max<float>(1 + (-1 * (weight - 0) / 300), 0.0f);
}
