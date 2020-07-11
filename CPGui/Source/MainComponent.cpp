/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    :nodePanel(NUM_NODES),
    controlsPanel(this, &LandF),
    connectionPanel(this, &LandF),
    setter{ new OSCParamSetter(8000) },
    nodeChangeListener{ new NodeChangeListener{ paramTree, setter.get()} },
    connectionChangeListener{ new ConnectionChangeListener{ paramTree, setter.get()} }
{
    paramTree.addChild(nodeParams, 0, nullptr);
    paramTree.addChild(conParams, 1, nullptr);

    for (int i{ 0 }; i < NUM_NODES; i++) {
        nodeParams.addChild(makeNodeValueTree(i), i, nullptr);

    }
    DBG(NUM_CONNECTIONS);
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        conParams.addChild(makeConnectionValueTree(i), i, nullptr);
    }

    nodeParams.addListener(nodeChangeListener.get());
    conParams.addListener(connectionChangeListener.get());

    setUpConnectionMenu();
    for (int i{ NUM_NODES - 1 }; i >= 0; i--) {
        availableNodes.push(i);
    }
    addAndMakeVisible(nodePanel);
    addAndMakeVisible(controlsPanel);
    addAndMakeVisible(connectionPanel);
    nodePanel.setInterceptsMouseClicks(false, true);

    /*Extra main controls, could put these elsewhere?*/
    mainFreqSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    mainFreqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, false, 50, 20);
    mainFreqSlider.setNormalisableRange(NormalisableRange<double>(0.01, 200.0f, 0.0001f, 1.0f));
    mainFreqSlider.onValueChange = [this] {
        setter->setParam("mainFreq", 0, mainFreqSlider.getValue());
    };
    addAndMakeVisible(&mainFreqSlider);
    mFreqLabel.setText("Main Frequency", dontSendNotification);
    mFreqLabel.attachToComponent(&mainFreqSlider, true);
    addAndMakeVisible(&mFreqLabel);

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
    File f{};
    f = f.getCurrentWorkingDirectory().getSiblingFile("Samples/Guitar.wav");
    fileComp.reset(new FilenameComponent("fileComp",
        {f},                       // current file
        false,                    // can edit file name,
        false,                    // is directory,
        false,                    // is for saving,
        {},                       // browser wildcard suffix,
        {},                       // enforced suffix,
        "Select file to open"));  // text when nothing selected

    addAndMakeVisible(fileComp.get());
    fileComp->addListener(this);
    fileLabel.setText("Choose Sample", dontSendNotification);
    fileLabel.attachToComponent(fileComp.get(), true);
    addAndMakeVisible(&fileLabel);
    currentPanel = nullptr;
    setSize (1000, 600);
    makeNode(100, 200);
    makeNode(400, 200);
    //makeNode(400, 400);
    makeConnection(nodes[0].get(), nodes[1].get());
    setUpMenu();
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        if (conParams.getChild(i).getProperty("active")) {
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
    fileComp->setBounds(width - 150, 0, 150, 50);
    mainFreqSlider.setBounds(width - 150, 50, 120, 120);
    addNodeButton.setBounds(width - 150, 170, 50, 50);
    DSPButton.setBounds(width - 150, 220, 50, 50);

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
    grid.items = { GridItem(nodePanel), GridItem(currentPanel) };
    grid.performLayout(getLocalBounds());
}

void MainComponent::mouseDown(const MouseEvent& e)
{
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (clickedNode != -1 && node != 0 && e.mods.isShiftDown()) {
        makeConnection(nodes[clickedNode].get(), node);
    }
    if (clickedNode != -1) {
        nodes[clickedNode]->setNodeColour(Colours::orange);
        clickedNode = -1;
    }
    if(node != 0) {
        node->setNodeColour(Colours::white);
        clickedNode = node->getNodeNumber();
        nodePanel.clickedNode = node;
        controlsPanel.setValues(node);
        currentPanel = &controlsPanel;
        currentPanel->setVisible(true);
        resized();
        return;
    }
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        if (cons[i] != nullptr && cons[i]->containsPoint(e.getMouseDownPosition().toFloat())) {
            clickedCon = i;
            showConnectionMenu(i);
            return;
        }
    }

    /*for (auto con : nodePanel.allCons) {
        if (con->containsPoint(e.getMouseDownPosition().toFloat())) {
            connectionPanel.setUpAttachments(con->getId());
            connectionPanel.setVisible(true);
            currentPanel = &connectionPanel;
            nodePanel.clickedConnection = con;
            resized();
            return;
        }
    }*/
}

void MainComponent::mouseDoubleClick(const MouseEvent& e) {
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (node == 0) return;
    node->toggleActive();
    node->repaint();
    if (node->nodeIsActive()) {
        setter->setParam("active", node->getNodeNumber(), 0);
    }
    else {
        setter->setParam("active", node->getNodeNumber(), 1);
    }
    //deleteNode(node->getNodeNumber());
    //clickedNode = -1;
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


    /*CPGNode* clickedNode = nodePanel.clickedNode;
    if (clickedNode != nullptr) {
        clickedNode->setParam(slider->getName(), slider->getValue());
        setter->setParam(slider->getName(), clickedNode->getComponentID().getIntValue(), slider->getValue());
    }

    CPGConnection* clickedCon = nodePanel.clickedConnection;
    if (clickedCon != nullptr) {
        clickedCon->setParam(slider->getName(), slider->getValue());
        int connectedNumber = clickedCon->getConnected()->getComponentID().getIntValue();
        int parentNumber = clickedCon->getParent()->getComponentID().getIntValue();
        double direction = clickedCon->getPropertyValue("direction");
        double weight = clickedCon->getPropertyValue("weight");
        clickedCon->recalculatePath();
        nodePanel.repaint();
        /*if (direction == 0) {
            setter->setWeight(connectedNumber, parentNumber, clickedCon->calculateWeight(weight));
        }
        else if (direction == 1) {
            setter->setWeight(parentNumber, connectedNumber, clickedCon->calculateWeight(weight));
        }
        else {
            setter->setWeight(connectedNumber, parentNumber, clickedCon->calculateWeight(weight * direction));
            setter->setWeight(parentNumber, connectedNumber, clickedCon->calculateWeight(weight * (1 - direction)));
        }*/
        /*
        ValueTree conTree = connectionPanel.paramTree.getChildWithName(clickedCon->getId());
        conTree.setProperty(slider->getName(), slider->getValue(), nullptr);
        int connectionNumber = clickedCon->getId().toString().getIntValue();
        int connectedNumber = clickedCon->getConnected()->getComponentID().getIntValue();
        int parentNumber = clickedCon->getParent()->getComponentID().getIntValue();
        if (slider->getName().startsWith("f")) {
            int temp = connectedNumber;
            connectedNumber = parentNumber;
            parentNumber = temp;
        }
        if (slider->getName() == "weight" || slider->getName() == "fWeight") {
            setter.setWeight(connectedNumber,
                             parentNumber,
                             clickedCon->calculateWeight(slider->getValue()));
        }
        else {
            setter.setConParam(slider->getName(), parentNumber, connectedNumber, slider->getValue());
        }
        clickedCon->recalculatePath(&conTree);
        nodePanel.repaint();
    }*/
}

void MainComponent::makeNode(int x, int y)
{
    if (availableNodes.empty()) return;
    int nodeId = availableNodes.top();
    availableNodes.pop();
    nodes[nodeId].reset(new CPGNode(nodeId, x, y));
    nodes[nodeId]->addComponentListener(this);
    nodes[nodeId]->addMouseListener(this ,false);
    addAndMakeVisible(nodes[nodeId].get());
    setter->setParam("active", nodeId, 0);
    nodeParams.getChild(nodeId).setProperty("active", true, nullptr);
    resized();
}

void MainComponent::deleteNode(int nodeId) {
    availableNodes.push(nodeId);
    nodeParams.getChild(nodeId).setProperty("active", false, nullptr);
    nodes[nodeId - 1].reset();
    for (int i{ 1 }; i <= NUM_NODES; i++) {
        int connectionIndex = getConnectionIndex(nodeId, i);
        if (nodeId != i && cons[connectionIndex] != nullptr) {
            setter->setWeight(nodeId, i, 0.0);
            setter->setWeight(i, nodeId, 0.0);
            cons[connectionIndex].reset();
        }
    }
    repaint();
}


void MainComponent::makeConnection(CPGNode* from, CPGNode* to)
{
    int connectionIndex = getConnectionIndex(from->getNodeNumber(), to->getNodeNumber());
    if (connectionIndex < 0) return;
    if (cons[connectionIndex] == nullptr) {
        cons[connectionIndex].reset(new CPGConnection{ to, from });
        setter->setWeight(to->getNodeNumber(),
            from->getNodeNumber(),
            cons[connectionIndex]->calculateWeight(1.0)
        );
        //conParams.addChild(makeConnectionValueTree(connectionIndex), connectionIndex, nullptr);
        conParams.getChild(connectionIndex).setProperty("active", true, nullptr);
        conParams.getChild(connectionIndex).setProperty("from", from->getNodeNumber(), nullptr);
        conParams.getChild(connectionIndex).setProperty("to", to->getNodeNumber(), nullptr);
        cons[connectionIndex]->recalculatePath();
    }
    repaint(); 
}

void MainComponent::showConnectionMenu(int connectionIndex)
{
    /*All this could do with a refactor*/
    /*Radio buttons are not working nicely*/
    changeMenuSliders("weight", "weightDir");
    weightButton.onClick();
    weightSlider.setValue(conParams.getChild(connectionIndex).getProperty("weight"));
    directionSlider.setValue(conParams.getChild(connectionIndex).getProperty("weightDir"));

    weightSlider.onValueChange = [this, connectionIndex] {
        conParams.getChild(connectionIndex).setProperty("weight", weightSlider.getValue(), nullptr);
    };
    directionSlider.onValueChange = [this, connectionIndex] {
        conParams.getChild(connectionIndex).setProperty("weightDir", directionSlider.getValue(), nullptr);
    };    

    const int result = m.show();
    if (result == 1)
    {
        setter->setWeight(nodePanel.allCons.getLast()->getConnected()->getComponentID().getIntValue(),
            nodePanel.allCons.getLast()->getParent()->getComponentID().getIntValue(),
            0.0f
        );
        nodePanel.allCons.removeObject(nodePanel.clickedConnection);
        nodePanel.clickedConnection = nullptr;
        nodePanel.repaint();
    }
}

void MainComponent::changeMenuSliders(Identifier weight, Identifier direction)
{
    weightSlider.setValue(conParams.getChild(clickedCon).getProperty(weight), dontSendNotification);
    directionSlider.setValue(conParams.getChild(clickedCon).getProperty(direction), dontSendNotification);
    weightSlider.onValueChange = [this, weight] {
        conParams.getChild(clickedCon).setProperty(weight, weightSlider.getValue(), nullptr);
    };
    directionSlider.onValueChange = [this, direction] {
        conParams.getChild(clickedCon).setProperty(direction, directionSlider.getValue(), nullptr);
    };
}

void MainComponent::setUpConnectionMenu()
{
    /*This is pretty bad*/
    m.addItem(1, "Delete Connection");
    //weightSlider.setName("weight");
    weightSlider.setNormalisableRange(NormalisableRange<double>(0.0, 10.0, 0.01, 1.0));
    m.addCustomItem(-1, weightSlider, 50, 60, false);
    directionSlider.setName("direction");
    directionSlider.setNormalisableRange(NormalisableRange<double>(0.0, 1.0, 0.01, 1.0));
    m.addCustomItem(-1, directionSlider, 50, 60, false);
    weightButton.setRadioGroupId(1);
    lengthButton.setRadioGroupId(1);
    positionButton.setRadioGroupId(1);
    weightButton.onClick = [this] {
        lengthButton.setToggleState(false, dontSendNotification);
        positionButton.setToggleState(false, dontSendNotification);
        weightButton.setToggleState(true, dontSendNotification);
        changeMenuSliders("weight", "weightDir");
    };
    lengthButton.onClick = [this] {
        weightButton.setToggleState(false, dontSendNotification);
        positionButton.setToggleState(false, dontSendNotification);
        lengthButton.setToggleState(true, dontSendNotification);
        changeMenuSliders("lengthMod", "lengthModDir");
    };
    positionButton.onClick = [this] {
        weightButton.setToggleState(false, dontSendNotification);
        lengthButton.setToggleState(false, dontSendNotification);
        positionButton.setToggleState(true, dontSendNotification);
        changeMenuSliders("startMod", "startModDir");
    };
    weightButton.setToggleState(true, dontSendNotification);
    m.addCustomItem(-1, weightButton, 50, 50, false);
    m.addCustomItem(-1, lengthButton, 50, 50, false);
    m.addCustomItem(-1, positionButton, 50, 50, false);
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
        .setProperty("volume", 0.7, nullptr);
}

ValueTree MainComponent::makeConnectionValueTree(int connectionIndex)
{
    return ValueTree{ Identifier{String{connectionIndex} } }
        .setProperty("active", false, nullptr)
        .setProperty("from", 0, nullptr)
        .setProperty("to", 0, nullptr)
        .setProperty("weight", 1.0, nullptr)
        .setProperty("weightDir", 0.5, nullptr)
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

void MainComponent::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) 
{
    if (fileComponentThatHasChanged == fileComp.get()) {
        setter->setFile(fileComp->getCurrentFile().getFullPathName());
    }
}

void MainComponent::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
}


/*
void MainComponent::initParamTree()
{
    for (int i{ 0 }; i < NUM_NODES; i++) {
        ValueTree node{ "node" + i };
        initNodeParams(&node);
        paramTree.addChild(node, i, nullptr);

    }
}

void MainComponent::initNodeParams(ValueTree* nodeParams)
{
    nodeParams->setProperty("grainLength", 200.0, nullptr);
    nodeParams->setProperty("startTime", 0.0, nullptr);
    nodeParams->setProperty("frequency", 1.0, nullptr);
    nodeParams->setProperty("pan", 0.f, nullptr);
    nodeParams->setProperty("volume", 0.7, nullptr);
}*/