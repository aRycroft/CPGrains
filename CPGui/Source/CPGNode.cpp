/*
  ==============================================================================

    CPGNode.cpp
    Created: 24 May 2020 1:01:49pm
    Author:  Alex

  ==============================================================================
*/

#include "CPGNode.h"
#include "CPGConnection.h"

CPGNode::CPGNode(int nodeNo, int x, int y)
{
	this->setComponentID(String(nodeNo));
	this->setCentrePosition(x, y);	
	setNodeColour(Colours::orange);
	tree = ValueTree(Identifier("grainLength"));
	tree.setProperty("grainLength", 200.0f, nullptr);
	tree.setProperty(Identifier("startTime"), 0.0f, nullptr);
}

void CPGNode::paint(Graphics& g)
{
	auto area = getLocalBounds().reduced(2);

	g.setColour(colour);
	g.drawRoundedRectangle(area.toFloat(), 10.0f, 2.0f);

	g.setColour(findColour(TextButton::textColourOffId));
	g.drawFittedText(getComponentID(), area, Justification::centred, 1);
}

void CPGNode::resized()
{
	// Just set the limits of our constrainer so that we don't drag ourselves off the screen
	constrainer.setMinimumOnscreenAmounts(getHeight(), getWidth(),
		getHeight(), getWidth());
}

void CPGNode::mouseDown(const MouseEvent& e)
{
	// Prepares our dragger to drag this Component
	dragger.startDraggingComponent(this, e);
}


void CPGNode::mouseDrag(const MouseEvent& e)
{
	// Moves this Component according to the mouse drag event and applies our constraints to it
	dragger.dragComponent(this, e, &constrainer);
}

void CPGNode::mouseUp(const MouseEvent& event) {
}

Array<std::shared_ptr<CPGConnection>>* CPGNode::getConnections()
{
	return &connections;
}

bool CPGNode::addConnection(std::shared_ptr<CPGConnection> connection) {
	if (connection->getParent() == connection->getConnected()) return false;
	for (auto con: connections) {
		if (con->getParent() == connection->getParent() && 
			con->getConnected() == connection->getConnected()) {
			return false;
		}
	}
	connections.add(connection);
	return true;
}

void CPGNode::removeConnection(CPGConnection* connection) {
	std::shared_ptr<CPGConnection> toRemove;
	for (auto conPTR : connections) {
		if (conPTR.get() == connection) {
			toRemove = conPTR;
		}
	}
	connections.removeFirstMatchingValue(toRemove);
}

void CPGNode::setNodeColour(juce::Colour newColour)
{
	colour = newColour;
	repaint();
}

