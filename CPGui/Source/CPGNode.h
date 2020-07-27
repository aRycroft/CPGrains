/*
  ==============================================================================

	CPGNode.h
	Created: 24 May 2020 1:01:49pm
	Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "GrainPositionSliderBox.h"
#define NODESIZE 70
class CPGNode : public Component
{
public:
	CPGNode(int nodeNo, int x, int y, juce::ValueTree tree, juce::Colour colour);
	void paint(Graphics& g) override;
	void resized() override;
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& event);
	void setActiveNodeColour(juce::Colour newColour);
	void useDefaultColour() {
		activeColour = defaultColour;
		repaint();
	};
	float getPropertyValue(Identifier i) { return 0.0f; };
	void setParam(Identifier i, double val) {};
	void toggleActive() {
		isActive = !isActive;
	}
	bool nodeIsActive() { return isActive; };
	int getNodeNumber() {
		return nodeNumber;
	};
	GrainPositionSliderBox* getSliderBox() {
		return sliderBox.get();
	};

private:
	//void addParams();
	ComponentBoundsConstrainer constrainer;
	ComponentDragger dragger;
	bool isActive{ true };
	juce::Colour defaultColour;
	juce::Colour activeColour;
	int nodeNumber;
	std::unique_ptr<GrainPositionSliderBox> sliderBox;
	juce::ValueTree nodeTree;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CPGNode)
};
