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
    controlsPanel.addSlider("grainLength", this, 5.0f, 5000.0f, 0.25f);
    controlsPanel.addSlider("startTime", this, 0.0f, 1000.0f, 1.0f);
    controlsPanel.addSlider("frequency", this, 0.0f, 32.0f, 0.5f);
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
    controlsPanel.setBounds(0, getHeight() - (getHeight() / 5), getWidth(), (getHeight() / 5));
    addNodeButton.setBounds(0, 0, 50, 50);
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
        controlsPanel.showSliders(node->getComponentID());
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
    CPGNode* clickedNode = nodePanel.clickedNode;
    if (clickedNode == nullptr) return;
    ValueTree nodeTree = controlsPanel.paramTree.getChildWithName(clickedNode->getComponentID());
    nodeTree.setProperty(slider->getName(), slider->getValue(), nullptr);
    setter.setParam(slider->getName(), clickedNode->getComponentID().getIntValue(), slider->getValue());
}

void MainComponent::makeNode(int x, int y)
{
    if (nodePanel.availableNodes.empty()) return;
    int id = nodePanel.availableNodes.top();
    nodePanel.availableNodes.pop();
    nodePanel.addAndMakeVisible(nodePanel.allNodes.add(new CPGNode(id, x, y)));
    CPGNode* node = nodePanel.allNodes.getLast();
    node->addComponentListener(this);
    node->setAlwaysOnTop(true);
    node->addMouseListener(this, false);
    nodePanel.resized();

    ValueTree nodeParam(node->getComponentID());
    nodeParam.setProperty("grainLength", 50.0f, nullptr);
    nodeParam.setProperty("startTime", 0.0f, nullptr);
    nodeParam.setProperty("frequency", 2.0f, nullptr);
    //nodeParam.setProperty("freq", 1.5f, nullptr);
    controlsPanel.paramTree.addChild(nodeParam, -1, nullptr);
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





