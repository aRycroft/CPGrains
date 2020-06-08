/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CPGNode.h"
#include "OSCParamSetter.h"
#include <stack>

#define NUM_NODES 4

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component,
                        public ComponentListener,
                        public MouseListener,
                        public Button::Listener,
                        public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent& e) override;
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
private:
    void makeNode(int x, int y);
    void makeConnection(CPGNode* from, CPGNode* to);
    void componentMovedOrResized(Component& movedComp, bool wasMoved, bool wasResized);
    void addSlider(String name, double rangeStart, double rangeEnd, double skewFactor);

    struct nodeContainer : public Component {
        nodeContainer(int numNodes) {
            allNodes.ensureStorageAllocated(NUM_NODES);
            allCons.ensureStorageAllocated(NUM_NODES * NUM_NODES);
            for (int i = NUM_NODES; i > 0; i--) {
                availableNodes.push(i);
            }
        }
        void paint(Graphics& g) override
        {
            g.fillAll(Colours::cadetblue);
            for (auto con : allCons) {
                g.strokePath(con->getPath(), PathStrokeType(2));
            }
        }

        void resized() override
        {
            for (auto nodes : allNodes) {
                nodes->setBounds(nodes->getX(), nodes->getY(), 50, 50);
            }
            for (auto con : allCons) {
                con->recalculatePath();
            }
        }

        CPGNode* clickedNode{ nullptr };
        std::stack<int> availableNodes;
        OwnedArray<CPGNode> allNodes;
        OwnedArray<CPGConnection> allCons;
    };

    struct controlsContainer : public Component{
        controlsContainer() {
        }
        void showSliders(String componentId)
        {
            grainLengthSlider.setValue(paramTree.getChildWithName(componentId)["grainLength"]);
            grainLengthSlider.setVisible(true);
            startTimeSlider.setValue(paramTree.getChildWithName(componentId)["startTime"]);
            startTimeSlider.setVisible(true);
        }

        void hideSliders() {
            grainLengthSlider.setVisible(false);
            startTimeSlider.setVisible(false);
        }

        OwnedArray<Slider> sliders;

    };


    nodeContainer nodePanel;
    controlsContainer controlsPanel;
    bool repaintCons{ true };
    TextButton addNodeButton;
    OSCParamSetter setter{ 8000 };
    ValueTree paramTree;
    Slider grainLengthSlider, startTimeSlider;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
