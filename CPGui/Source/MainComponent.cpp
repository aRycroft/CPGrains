/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    :nodePanel(NUM_NODES),
    controlsPanel()
{
    addAndMakeVisible(nodePanel);
    addAndMakeVisible(controlsPanel);
    addSlider("grainLength", 
    paramTree = ValueTree(Identifier("nodeParams"));
    nodePanel.setInterceptsMouseClicks(false, true);
    addAndMakeVisible(&addNodeButton);
    addNodeButton.addListener(this);
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour(Colours::white);
}

void MainComponent::resized()
{
    nodePanel.setBounds(0, 0, getWidth(), getHeight() - (getHeight() / 5));
    addNodeButton.setBounds(0, 0, 50, 50);
    grainLengthSlider.setBounds(0, getHeight() - (getHeight() / 5), 150, 100);
    startTimeSlider.setBounds(getWidth() / 10 * 3, getHeight() - (getHeight() / 5), 150, 100);
    
}

void MainComponent::mouseDown(const MouseEvent& e)
{
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (nodePanel.clickedNode != nullptr && node != 0 && e.mods.isShiftDown()) {
        makeConnection(nodePanel.clickedNode, node);
    }
    if (nodePanel.clickedNode != nullptr) {
        nodePanel.clickedNode->setNodeColour(Colours::orange);
        nodePanel.clickedNode = nullptr;
        controlsPanel.hideSliders();
    }
    if (node != 0) {
        node->setNodeColour(Colours::white);
        nodePanel.clickedNode = node;
        controlsPanel.showSliders(nodePanel.clickedNode->getComponentID());
        return;
    }
    CPGConnection* clickedConnection;
    for (auto con : nodePanel.allCons) {
        if (con->getPath().contains(e.getMouseDownPosition().toFloat())) {
            clickedConnection = con;
            setter.setWeight(nodePanel.allCons.getLast()->getConnected()->getComponentID().getIntValue(),
                nodePanel.allCons.getLast()->getParent()->getComponentID().getIntValue(),
                0.0f
            );
            nodePanel.allCons.removeObject(con);
            nodePanel.repaint();
            return;
        }
    }
}

void MainComponent::buttonClicked(Button* button)
{
    makeNode(rand() % nodePanel.getWidth() - 50, rand() % nodePanel.getHeight() - 50);
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    ValueTree nodeTree = paramTree.getChildWithName(nodePanel.clickedNode->getComponentID());
    nodeTree.setProperty(slider->getComponentID(), slider->getValue(), nullptr);
    setter.setParam(slider->getComponentID(), nodePanel.clickedNode->getComponentID().getIntValue(), slider->getValue());
}

void MainComponent::makeNode(int x, int y)
{
    if (nodePanel.availableNodes.empty()) return;
    int id = nodePanel.availableNodes.top();
    nodePanel.availableNodes.pop();
    nodePanel.addAndMakeVisible(nodePanel.allNodes.add(new CPGNode(id, x, y)));
    //addAndMakeVisible(allNodes.add(new CPGNode(id, x, y)));
    CPGNode* node = nodePanel.allNodes.getLast();
    node->addComponentListener(this);
    node->setAlwaysOnTop(true);
    node->addMouseListener(this, false);
    node->setBounds(node->getX(), node->getY(), 50, 50);
    ValueTree nodeParam(node->getComponentID());
    nodeParam.setProperty("grainLength", 50.0f, nullptr);
    nodeParam.setProperty("startTime", 0.0f, nullptr);
    //nodeParam.setProperty("freq", 1.5f, nullptr);
    paramTree.addChild(nodeParam, -1, nullptr);
}

void MainComponent::makeConnection(CPGNode* from, CPGNode* to)
{
    if (from == to) return;
    for (auto con : nodePanel.allCons) {
        if (con->getParent() == from &&
            con->getConnected() == to) {
            return;
        }
    }
    nodePanel.allCons.add(new CPGConnection(from, to));
    nodePanel.allCons.getLast()->recalculatePath();
    setter.setWeight(nodePanel.allCons.getLast()->getConnected()->getComponentID().getIntValue(),
                    nodePanel.allCons.getLast()->getParent()->getComponentID().getIntValue(),
                    nodePanel.allCons.getLast()->calculateWeight()
    );
    nodePanel.repaint();
}

void MainComponent::componentMovedOrResized(Component &movedComp, bool wasMoved, bool wasResized) {
    CPGNode* node = dynamic_cast <CPGNode*> (&movedComp);
    if (node == 0) return;
    for (auto con : nodePanel.allCons) {
        con->recalculatePath();
        nodePanel.repaint();
        if (con->getParent() == node || con->getConnected() == node) {
            setter.setWeight(con->getConnected()->getComponentID().getIntValue(),
                con->getParent()->getComponentID().getIntValue(),
                con->calculateWeight()
            );
        }
    }
}
void MainComponent::addSlider(String name, double rangeStart, double rangeEnd, double skewFactor) {
    Slider slider;
    slider.setComponentID(name);
    slider.setSliderStyle(Slider::Rotary);
    slider.setNormalisableRange(NormalisableRange<double>(rangeStart, rangeEnd, 0.001f, skewFactor));
    slider.addListener(this);

}




