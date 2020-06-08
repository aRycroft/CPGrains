/*
  ==============================================================================

	CPGNode.h
	Created: 24 May 2020 1:01:49pm
	Author:  Alex

  ==============================================================================
*/

#pragma once
#include <list>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CPGConnection.h"
class CPGNode : public Component
{
public:
	CPGNode(int nodeNo, int x, int y);
	void paint(Graphics& g) override;
	void resized() override;
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& event);
	Array<std::shared_ptr<CPGConnection>>* getConnections();
	bool addConnection(std::shared_ptr<CPGConnection> connection);
	void removeConnection(CPGConnection* connection);
	void setNodeColour(juce::Colour newColour);
	ValueTree tree;
private:
	ComponentBoundsConstrainer constrainer;
	ComponentDragger dragger;
	Array<std::shared_ptr<CPGConnection>> connections{};
	int xPos, yPos;
	juce::Colour colour;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CPGNode)
};
