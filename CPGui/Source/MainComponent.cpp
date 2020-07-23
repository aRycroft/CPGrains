/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    :
    samplePanel(this, nodeParams),
    setter{ new OSCParamSetter(8000) },
    nodeChangeListener{ new NodeChangeListener{ paramTree, setter.get()} },
    connectionChangeListener{ new ConnectionChangeListener{ paramTree, setter.get()} },
    conMenu(conParams, &clickedCon, this),
    nodeMenu(nodeParams, &clickedNode)
{
    paramTree.addChild(nodeParams, 0, nullptr);
    paramTree.addChild(conParams, 1, nullptr);

    for (int i{ 0 }; i < NUM_NODES; i++) {
        nodeParams.addChild(makeNodeValueTree(i), i, nullptr);
    }
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        conParams.addChild(makeConnectionValueTree(i), i, nullptr);
    }

    nodeParams.addListener(nodeChangeListener.get());
    conParams.addListener(connectionChangeListener.get());

    for (int i{ NUM_NODES - 1 }; i >= 0; i--) {
        availableNodes.push(i);
    }

    addAndMakeVisible(nodePanel);
    addAndMakeVisible(samplePanel);
    nodePanel.setInterceptsMouseClicks(false, true);
    /*Extra main controls, could put these elsewhere?*/
    addNodeButton.addListener(this);
    addAndMakeVisible(&addNodeButton);
    addButtonLabel.setText("Add Node", dontSendNotification);
    addButtonLabel.attachToComponent(&addNodeButton, true);
    addAndMakeVisible(&addButtonLabel);

    DSPButton.addListener(this);
    addAndMakeVisible(&DSPButton);
    DSPLabel.setText("Toggle DSP", dontSendNotification);
    DSPLabel.attachToComponent(&DSPButton, true);
    addAndMakeVisible(&DSPLabel);

    addAndMakeVisible(&fileLabel);
    setSize (1000, 600);
    makeNode(100, 200);
    //makeNode(400, 200);
   // makeConnection(nodes[0].get(), nodes[1].get());
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (juce::Colours::darkslategrey);
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        if (conParams.getChild(i).getProperty("active")) {
            cons[i]->recalculatePath();
            g.setColour(Colours::black);
            g.fillPath(*cons[i]->getPath());
            g.setColour(Colours::antiquewhite);
            g.fillPath(*cons[i]->getParameterPathBands());
        }
    }
}

void MainComponent::resized()
{
    int width = getWidth();
    addNodeButton.setBounds(width - 100, 25, 50, 50);
    DSPButton.setBounds(width - 100, 100, 50, 50);

    for (int i{ 0 }; i < NUM_NODES; i++) {
        if (nodeParams.getChild(i).getProperty("active")) {
            CPGNode* test = nodes[i].get();
            test->setBounds(std::min(test->getParentWidth() - NODESIZE, test->getX()),
                std::min(test->getParentHeight() - NODESIZE, test->getY()),
                NODESIZE, NODESIZE);
        }
    }
    Grid grid;

    using Track = Grid::TrackInfo;

    grid.templateRows = { Track(5_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr)};
    grid.items = { GridItem(nodePanel), GridItem(samplePanel) };
    grid.performLayout(getLocalBounds());
}

void MainComponent::mouseDown(const MouseEvent& e)
{
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (clickedNode != -1 && node != 0 && e.mods.isShiftDown()) {
        makeConnection(nodes[clickedNode].get(), node);
    }
    if (clickedNode != -1) {
        nodes[clickedNode]->useDefaultColour();
        clickedNode = -1;
    }
    if(node != 0) {
        node->setActiveNodeColour(Colours::white);
        clickedNode = node->getNodeNumber();
        resized();
        return;
    }
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        if (cons[i] != nullptr && cons[i]->containsPoint(e.getMouseDownPosition().toFloat())) {
            clickedCon = i;
            const int deleteConRequested = conMenu.showMenu();
            if (deleteConRequested) {
                deleteConnection(i);
            }
            return;
        }
    }
}

void MainComponent::mouseDoubleClick(const MouseEvent& e) {
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (node == 0) return;
    clickedNode = node->getNodeNumber();
    const int deleteNodeRequested = nodeMenu.showMenu();
    if (deleteNodeRequested) {
        deleteNode(clickedNode);
    }
}

void MainComponent::buttonClicked(Button* button)
{
    if (button == &addNodeButton) {
        makeNode(rand() % nodePanel.getWidth() - 50, rand() % nodePanel.getHeight() - 50);
    }
    else if (button == &DSPButton) {
        DSPOn = !DSPOn;
        setter->setDSPState(DSPOn);
    }
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    nodeParams.getChild(clickedNode).setProperty(slider->getName(), slider->getValue(), nullptr);
}

void MainComponent::makeNode(int x, int y)
{
    if (availableNodes.empty()) return;
    int nodeId = availableNodes.top();
    availableNodes.pop();
    nodes[nodeId].reset(new CPGNode(nodeId, x, y, juce::Colour::fromString(colours[nodeId])));
    nodes[nodeId]->addComponentListener(this);
    nodes[nodeId]->addMouseListener(this ,false);
    nodePanel.addAndMakeVisible(nodes[nodeId].get());
    setter->setParam("active", nodeId, 0);
    nodeParams.getChild(nodeId).setProperty("active", true, nullptr);
    resized();
}

void MainComponent::deleteNode(int nodeId) {
    availableNodes.push(nodeId);
    nodeParams.getChild(nodeId).setProperty("active", false, nullptr);
    nodes[nodeId].reset();
    for (int i{ 0 }; i < NUM_NODES; i++) {
        int connectionIndex = getConnectionIndex(nodeId, i);
        if (nodeId != i && cons[connectionIndex] != nullptr) {
            deleteConnection(connectionIndex);
        }
    }
    clickedNode = -1;
    repaint();
}


void MainComponent::makeConnection(CPGNode* from, CPGNode* to)
{
    int connectionIndex = getConnectionIndex(from->getNodeNumber(), to->getNodeNumber());
    if (connectionIndex < 0) return;
    if (cons[connectionIndex] == nullptr) {
        cons[connectionIndex].reset(new CPGConnection{ to, from, conParams.getChild(connectionIndex) });
        setter->setWeight(to->getNodeNumber(),
            from->getNodeNumber(),
            cons[connectionIndex]->calculateWeight(1.0)
        );
        conParams.getChild(connectionIndex).setProperty("active", true, nullptr);
        conParams.getChild(connectionIndex).setProperty("from", from->getNodeNumber(), nullptr);
        conParams.getChild(connectionIndex).setProperty("to", to->getNodeNumber(), nullptr);
        cons[connectionIndex]->recalculatePath();
    }
    repaint(); 
}

void MainComponent::deleteConnection(int conId)
{
    CPGNode* connected = (CPGNode*)cons[conId]->getConnected();
    CPGNode* parent = (CPGNode*)cons[conId]->getParent();
    cons[conId].reset();
    conParams.getChild(conId).setProperty("active", false, nullptr);
    conParams.getChild(conId).setProperty("weight", 0.0, nullptr);
    conParams.getChild(conId).setProperty("lengthMod", 0.0, nullptr);
    conParams.getChild(conId).setProperty("startMod", 0.0, nullptr);
    clickedCon = -1;
    repaint();
}

ValueTree MainComponent::makeNodeValueTree(int nodeId)
{
    return ValueTree{ Identifier{String{nodeId} } }
        .setProperty("active", false, nullptr)
        .setProperty("x", 0.0, nullptr)
        .setProperty("y", 0.0, nullptr)
        .setProperty("grainLength", 200.0, nullptr)
        .setProperty("startTime", 0.0, nullptr)
        .setProperty("frequency", 1.0, nullptr)
        .setProperty("pan", 0.f, nullptr)
        .setProperty("volume", 0.7, nullptr)
        .setProperty("colour", colours[nodeId], nullptr);
        
}

ValueTree MainComponent::makeConnectionValueTree(int connectionIndex)
{
    return ValueTree{ Identifier{String{connectionIndex} } }
        .setProperty("active", false, nullptr)
        .setProperty("from", 0, nullptr)
        .setProperty("to", 0, nullptr)
        .setProperty("weight", 1.0, nullptr)
        .setProperty("weightDir", 0.0, nullptr)
        .setProperty("lengthMod", 0.0, nullptr)
        .setProperty("lengthModDir", 0.0, nullptr)
        .setProperty("startMod", 0.0, nullptr)
        .setProperty("startModDir", 0.0, nullptr);
}

int MainComponent::getConnectionIndex(int from, int to)
{
    /*Weird system to find index in array depending on node numbers*/
    if (from == to) return -1;
    //from -= 1;
    //to -= 1;
    int connectionIndex;
    if (from == 0) connectionIndex = to - 1;
    else if (to == 0) connectionIndex = from - 1;
    else connectionIndex = to + from;
    return connectionIndex;
}


void MainComponent::componentMovedOrResized(Component &movedComp, bool wasMoved, bool wasResized) {
    CPGNode* node = dynamic_cast <CPGNode*> (&movedComp);
    if (node == 0) return;
    nodeParams.getChild(node->getNodeNumber()).setProperty("x", node->getX(), nullptr);
    nodeParams.getChild(node->getNodeNumber()).setProperty("y", node->getY(), nullptr);
    for (int i{ 0 }; i < NUM_NODES; i++) {
        int nodeNumber = node->getNodeNumber();
        int connectionIndex = getConnectionIndex(nodeNumber, i);
        if (nodeNumber != i && cons[connectionIndex] != nullptr) {
            cons[connectionIndex]->recalculatePath();
            double weight = conParams.getChild(connectionIndex).getPropertyAsValue("weight", nullptr).getValue();
            double dir = conParams.getChild(connectionIndex).getPropertyAsValue("weightDir", nullptr).getValue();
            connectionChangeListener->sendWeight(nodeNumber, i, weight * dir);
            connectionChangeListener->sendWeight(i, nodeNumber, weight * (1 - dir));
        }
    }
    repaint();
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    setter->setFile(samplePanel.getFileName());
}

/*void MainComponent::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) 
{
    setter->setFile(fileComp->getCurrentFile().getFullPathName());
    //samplePanel.shouldShowWaveform(true);
}*/
