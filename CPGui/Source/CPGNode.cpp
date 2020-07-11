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
	nodeNumber = nodeNo;
	this->setComponentID(String(nodeNo));
	this->setCentrePosition(x, y);	
	setNodeColour(Colours::orange);
	addParams();
}

void CPGNode::paint(Graphics& g)
{
	auto area = getLocalBounds().reduced(1);

	g.setColour(colour);
	if (isActive) {
		g.fillRoundedRectangle(area.toFloat(), 100.0f);
	}
	else {
		g.drawRoundedRectangle(area.toFloat(), 100.0f, 2.0f);
	}

	g.setColour(Colours::black);
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


void CPGNode::setNodeColour(juce::Colour newColour)
{
	colour = newColour;
	repaint();
}
