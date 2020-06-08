/*
  ==============================================================================

    CPGListener.h
    Created: 26 May 2020 3:15:29pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "CPGNode.h"
#include "CPGConnection.h"
#include "ParamSetter.h"

class CPGListener : public ComponentListener {
public:
    CPGListener(ParamSetter* setter) { this->setter = setter; };
    void componentMovedOrResized(Component& component, bool wasMoved, bool wasResized) override {
        CPGNode* node = dynamic_cast <CPGNode*> (&component);
        if (node == 0) return;
        for (std::shared_ptr<CPGConnection> connectionPTR : node->getConnections()) {
            CPGConnection* connection = connectionPTR.get();
            Point<int> nodePos = connection->getParent()->getPosition();
            Point<int> connectedPos = connection->getConnected()->getPosition();
            Rectangle<int>rect(nodePos, connectedPos);
            rect.setHeight(std::max<int>(rect.getHeight(), 20));
            rect.setWidth(std::max<int>(rect.getWidth(), 20));
            connection->setBounds(rect);
            /*Must be more efficient way to do this*/
            //connection->getParentComponent()->repaint();
            float weight = calculateWeight(nodePos, connectedPos);
            weight = 1 + (-1 * (weight - 0) / 300);
            weight = std::max(weight, 0.0f);
            setter->setWeight(connection->getParent()->getComponentID().getIntValue(), connection->getConnected()->getComponentID().getIntValue(), weight);
        }
    };
    void resetConnection(CPGConnection* con) {
        Component* c = con->getParent();
        setter->setWeight(con->getParent()->getComponentID().getIntValue(), con->getConnected()->getComponentID().getIntValue(), 0.0f);
    };
private:
    float calculateWeight(Point<int> pos1, Point<int> pos2) {
        return sqrt(pow(pos2.getX() - pos1.getX(), 2) + pow(pos2.getY() - pos2.getY(), 2) * 1.0);
    }
    ParamSetter* setter;
    unsigned tick{ 0 };
};
