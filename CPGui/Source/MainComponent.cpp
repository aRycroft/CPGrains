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
    connectionPanel(this, &LandF)
{
    addAndMakeVisible(nodePanel);
    addAndMakeVisible(controlsPanel);
    addAndMakeVisible(connectionPanel);
    nodePanel.setInterceptsMouseClicks(false, true);

    /*Extra main controls, could put these elsewhere?*/
    mainFreqSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    mainFreqSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, false, 50, 20);
    mainFreqSlider.setNormalisableRange(NormalisableRange<double>(0.01, 200.0f, 0.0001f, 1.0f));
    mainFreqSlider.onValueChange = [this] {
        setter.setParam("mainFreq", 0, mainFreqSlider.getValue());
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
    makeConnection(nodePanel.allNodes[0], nodePanel.allNodes[1]);
    setUpMenu();
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
    int width = getWidth();
    fileComp->setBounds(width - 150, 0, 150, 50);
    mainFreqSlider.setBounds(width - 150, 50, 120, 120);
    addNodeButton.setBounds(width - 150, 170, 50, 50);
    DSPButton.setBounds(width - 150, 220, 50, 50);

    Grid grid;

    using Track = Grid::TrackInfo;

    grid.templateRows = { Track(5_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr)};
    grid.items = { GridItem(nodePanel), GridItem(currentPanel) };
    grid.performLayout(getLocalBounds());
}

void MainComponent::mouseDown(const MouseEvent& e)
{
    if (currentPanel != nullptr) {
        currentPanel->setVisible(false);
        currentPanel = nullptr;
    }
    nodePanel.clickedConnection = nullptr;

    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (nodePanel.clickedNode != nullptr && node != 0 && e.mods.isShiftDown()) {
        makeConnection(nodePanel.clickedNode, node);
    }
    if (nodePanel.clickedNode != nullptr) {
        nodePanel.clickedNode->setNodeColour(Colours::orange);
        nodePanel.clickedNode = nullptr;
    }
    if(node != 0) {
        node->setNodeColour(Colours::white);
        nodePanel.clickedNode = node;
        controlsPanel.setValues(node);
        currentPanel = &controlsPanel;
        currentPanel->setVisible(true);
        resized();
        return;
    }
    for (auto con : nodePanel.allCons) {
        if (con->containsPoint(e.getMouseDownPosition().toFloat())) {
            connectionPanel.setUpAttachments(con->getId());
            connectionPanel.setVisible(true);
            currentPanel = &connectionPanel;
            nodePanel.clickedConnection = con;
            resized();
            createConnectionMenu();
            return;
        }
    }
}

void MainComponent::mouseDoubleClick(const MouseEvent& e) {
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (node == 0) return;
    node->toggleActive();
    node->repaint();
    if (node->nodeIsActive()) {
        setter.setParam("active", node->getComponentID().getIntValue(), 0);
    }
    else {
        setter.setParam("active", node->getComponentID().getIntValue(), 1);
    }
}


void MainComponent::buttonClicked(Button* button)
{
    if (button == &addNodeButton) {
        makeNode(rand() % nodePanel.getWidth() - 50, rand() % nodePanel.getHeight() - 50);
    }
    else if (button == &DSPButton) {
        DSPOn = !DSPOn;
        setter.setDSPState(DSPOn);
    }
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    CPGNode* clickedNode = nodePanel.clickedNode;
    if (clickedNode != nullptr) {
        //ValueTree nodeTree = controlsPanel.paramTree.getChildWithName(clickedNode->getComponentID());
        //nodeTree.setProperty(slider->getName(), slider->getValue(), nullptr);
        clickedNode->setParam(slider->getName(), slider->getValue());
        setter.setParam(slider->getName(), clickedNode->getComponentID().getIntValue(), slider->getValue());
    }

    CPGConnection* clickedCon = nodePanel.clickedConnection;
    if (clickedCon != nullptr) {
        clickedCon->setParam(slider->getName(), slider->getValue());
        int connectedNumber = clickedCon->getConnected()->getComponentID().getIntValue();
        int parentNumber = clickedCon->getParent()->getComponentID().getIntValue();
        double direction = clickedCon->getPropertyValue("direction");
        double weight = clickedCon->getPropertyValue("weight");
        /*No Feedback*/
        clickedCon->recalculatePath();
        nodePanel.repaint();
        if (direction == 0) {
            setter.setWeight(connectedNumber, parentNumber, clickedCon->calculateWeight(weight));
        }
        else if (direction == 1) {
            setter.setWeight(parentNumber, connectedNumber, clickedCon->calculateWeight(weight));
        }
        else {
            setter.setWeight(connectedNumber, parentNumber, clickedCon->calculateWeight(weight * direction));
            setter.setWeight(parentNumber, connectedNumber, clickedCon->calculateWeight(weight * (1 - direction)));
        }
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
        nodePanel.repaint();*/
    }
}

void MainComponent::makeNode(int x, int y)
{
    int componentID = nodePanel.addNode(this, this, x, y);
    if (!componentID) return;
    setter.setParam("active", componentID, 0);
}

void MainComponent::makeConnection(CPGNode* from, CPGNode* to)
{
    CPGConnection* con = nodePanel.addConn(from, to);
    if (con == nullptr) return;
    setter.setWeight(con->getConnected()->getComponentID().getIntValue(),
                     con->getParent()->getComponentID().getIntValue(),
                     con->calculateWeight(1.0)
    );
    connectionPanel.addParams(con->getId().toString());
    con->recalculatePath();
    nodePanel.repaint();
}

void MainComponent::createConnectionMenu()
{
    const int result = m.show();
    if (result == 1)
    {
        setter.setWeight(nodePanel.allCons.getLast()->getConnected()->getComponentID().getIntValue(),
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
    weightSlider.setValue(nodePanel.clickedConnection->getPropertyValue(weight), dontSendNotification);
    directionSlider.setValue(nodePanel.clickedConnection->getPropertyValue(direction), dontSendNotification);
    weightSlider.onValueChange = [this, weight] {
        nodePanel.clickedConnection->setParam(weight.toString(), weightSlider.getValue());
    };
    directionSlider.onValueChange = [this, direction] {
        nodePanel.clickedConnection->setParam(direction.toString(), directionSlider.getValue());
    };
}

void MainComponent::setUpMenu()
{
    /*This is pretty bad*/
    m.addItem(1, "Delete Connection");
    weightSlider.setName("weight");
    weightSlider.setNormalisableRange(NormalisableRange<double>(0.0, 10.0, 0.01, 1.0));
    m.addCustomItem(-1, weightSlider, 50, 60, false);
    directionSlider.setName("direction");
    directionSlider.setNormalisableRange(NormalisableRange<double>(0.0, 1.0, 0.01, 1.0));
    m.addCustomItem(-1, directionSlider, 50, 60, false);
    weightButton.onClick = [this] {
        lengthButton.setToggleState(false, dontSendNotification);
        positionButton.setToggleState(false, dontSendNotification);
        weightButton.setToggleState(true, dontSendNotification);
        changeMenuSliders("weight", "direction");
    };
    lengthButton.onClick = [this] {
        weightButton.setToggleState(false, dontSendNotification);
        positionButton.setToggleState(false, dontSendNotification);
        lengthButton.setToggleState(true, dontSendNotification);
        changeMenuSliders("lengthWeight", "lengthDirection");
    };
    positionButton.onClick = [this] {
        weightButton.setToggleState(false, dontSendNotification);
        lengthButton.setToggleState(false, dontSendNotification);
        positionButton.setToggleState(true, dontSendNotification);
        changeMenuSliders("posWeight", "posDirection");
    };
    weightButton.setToggleState(true, dontSendNotification);
    m.addCustomItem(-1, weightButton, 50, 50, false);
    m.addCustomItem(-1, lengthButton, 50, 50, false);
    m.addCustomItem(-1, positionButton, 50, 50, false);
}

void MainComponent::componentMovedOrResized(Component &movedComp, bool wasMoved, bool wasResized) {
    CPGNode* node = dynamic_cast <CPGNode*> (&movedComp);
    if (node == 0) return;
    for (auto con : nodePanel.allCons) {
        ValueTree conParams = connectionPanel.paramTree.getChildWithName(con->getId());
        con->recalculatePath();
        nodePanel.repaint();
        if (con->getParent() == node || con->getConnected() == node) {
            double weightMult = conParams.getPropertyAsValue("weight", nullptr).getValue();
            setter.setWeight(con->getConnected()->getComponentID().getIntValue(),
                             con->getParent()->getComponentID().getIntValue(),
                             con->calculateWeight(weightMult)
            );
            double feedBackWeight = conParams.getPropertyAsValue("fWeight", nullptr).getValue();
            setter.setWeight(con->getParent()->getComponentID().getIntValue(),
                con->getConnected()->getComponentID().getIntValue(),
                con->calculateWeight(feedBackWeight)
            );
        }
    }
}

void MainComponent::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) 
{
    if (fileComponentThatHasChanged == fileComp.get()) {
        setter.setFile(fileComp->getCurrentFile().getFullPathName());
    }
}

    

