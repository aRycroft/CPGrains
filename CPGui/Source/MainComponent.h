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
            int nodeSize = std::min(proportionOfHeight(0.2), proportionOfWidth(0.2));
            for (auto node : allNodes) {
                node->setBounds(std::min(getParentWidth() - nodeSize, node->getX()), std::min(getParentHeight() - nodeSize, node->getY()), nodeSize, nodeSize);
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
            paramTree = ValueTree(Identifier("nodeParams"));
        }

        void paint(Graphics& g) override
        {
            g.fillAll(Colours::navajowhite);
        }

        void resized() override
        {
            Grid grid;

            using Track = Grid::TrackInfo;
            grid.templateRows = { Track(1_fr) };
            grid.templateColumns = { Track(1_fr), Track(1_fr), Track(1_fr) };
            for (auto* s : sliders) {
                GridItem g1 = GridItem(s);
                //g1.maxWidth = 200.0f;
                //g1.maxHeight = 200.0f;
                grid.items.add(g1);
                grid.performLayout(getLocalBounds());
            }
            /*
            FlexBox fb;
            fb.flexWrap = FlexBox::Wrap::wrap;
            fb.justifyContent = FlexBox::JustifyContent::flexStart;
            fb.alignContent = FlexBox::AlignContent::flexStart;

            for (auto* s : sliders) {
                fb.items.add(FlexItem(*s).withMinWidth(150.0f).
                    withMinHeight(100.0f));
            }
            fb.performLayout(getLocalBounds().toFloat());
            */
        }

        void addSlider(String name, Slider::Listener* listener, double rangeStart, double rangeEnd, double skewFactor) {
            addChildComponent(sliders.add(new Slider(name)));
            Slider* slider = sliders.getLast();
            slider->setSliderStyle(Slider::Rotary);
            slider->setNormalisableRange(NormalisableRange<double>(rangeStart, rangeEnd, 0.001f, skewFactor));
            slider->addListener(listener);
        }

        void showSliders(String componentId)
        {
            for (auto slider : sliders) {
                slider->setValue(paramTree.getChildWithName(componentId)[slider->getName()]);
                slider->setVisible(true);
                resized();
            }
        }

        void hideSliders() {
            for (auto slider : sliders) {
                slider->setVisible(false);
            }

        }

        ValueTree paramTree;
        OwnedArray<Slider> sliders;
    };

    nodeContainer nodePanel;
    controlsContainer controlsPanel;
    TextButton addNodeButton;
    OSCParamSetter setter{ 8000 };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
