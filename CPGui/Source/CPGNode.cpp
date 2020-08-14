/*
  ==============================================================================

    CPGNode.cpp
    Created: 24 May 2020 1:01:49pm
    Author:  Alex

  ==============================================================================
*/

#include "CPGNode.h"
#include "CPGConnection.h"

CPGNode::CPGNode(int nodeNo, int x, int y, juce::ValueTree tree)
	:nodeTree(tree)
{
	juce::Colour newColour = juce::Colour::fromString(tree.getProperty(Ids::colour).toString());
	nodeNumber = nodeNo;
	this->setCentrePosition(x, y);	
	defaultColour = activeColour = newColour;
	sliderBox.reset(new GrainPositionSliderBox(tree, newColour));
}

void CPGNode::paint(Graphics& g)
{
	auto area = getLocalBounds().reduced(1);

	g.setColour(activeColour);
	if (isActive) {
		g.fillRoundedRectangle(area.toFloat(), 100.0f);
	}
	else {
		g.drawRoundedRectangle(area.toFloat(), 100.0f, 2.0f);
	}

	g.setColour(Colours::black);
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

void CPGNode::setActiveNodeColour(juce::Colour newColour)
{
	activeColour = newColour;
	repaint();
}

