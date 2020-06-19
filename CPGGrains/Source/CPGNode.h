/*
  ==============================================================================

	CPGNode.h
	Created: 24 May 2020 1:01:49pm
	Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "CPGConnection.h"
class CPGNode : public Component
{
public:
	CPGNode() {};
	CPGNode(int nodeNo, int x, int y);
	void paint(Graphics& g) override;
	void resized() override;
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& event);
	void setNodeColour(juce::Colour newColour);
	void toggleActive() {
		isActive = !isActive;
	}
	bool nodeIsActive() { return isActive; };
private:
	ComponentBoundsConstrainer constrainer;
	ComponentDragger dragger;
	bool isActive{ true };
	juce::Colour colour;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CPGNode)
};

/*class ControlNode : public CPGNode {
public:
	ControlNode(int nodeNo, int x, int y) {
		this->setComponentID(String(nodeNo));
		this->setCentrePosition(x, y);
		setNodeColour(Colours::orange);
	}
	void paint(Graphics& g) {
		auto area = getLocalBounds().reduced(1);
		g.setColour(colour);
		g.drawRoundedRectangle(area.toFloat(), 100.0f, 2.0f);

		g.setColour(Colours::black);
		g.drawFittedText(getComponentID(), area, Justification::centred, 1);
	};

};
*/