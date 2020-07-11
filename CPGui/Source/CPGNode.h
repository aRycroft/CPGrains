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
#define NODESIZE 70
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
	float getPropertyValue(Identifier i) { return 0.0f; };
	void addParams() {};
	void setParam(Identifier i, double val) {};
	void toggleActive() {
		isActive = !isActive;
	}
	bool nodeIsActive() { return isActive; };
	int getNodeNumber() {
		return nodeNumber;
	};
private:
	//void addParams();
	ComponentBoundsConstrainer constrainer;
	ComponentDragger dragger;
	bool isActive{ true };
	juce::Colour colour;
	int nodeNumber;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CPGNode)
};
