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
        controlsPanel.setUpAttachments(node->getComponentID());
        currentPanel = &controlsPanel;
        currentPanel->setVisible(true);
        resized();
        return;
    }
    for (auto con : nodePanel.allCons) {
        if (con->getPath().contains(e.getMouseDownPosition().toFloat())) {
            connectionPanel.setUpAttachments(con->getId());
            connectionPanel.setVisible(true);
            currentPanel = &connectionPanel;
            nodePanel.clickedConnection = con;
            resized();
            PopupMenu m;
            m.addItem(1, "Delete Connection");
            const int result = m.show();
            if (result == 1)
            {
                setter.setWeight(nodePanel.allCons.getLast()->getConnected()->getComponentID().getIntValue(),
                    nodePanel.allCons.getLast()->getParent()->getComponentID().getIntValue(),
                    0.0f
                );
                nodePanel.allCons.removeObject(con);
                nodePanel.repaint();
            }
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
        ValueTree nodeTree = controlsPanel.paramTree.getChildWithName(clickedNode->getComponentID());
        nodeTree.setProperty(slider->getName(), slider->getValue(), nullptr);
        setter.setParam(slider->getName(), clickedNode->getComponentID().getIntValue(), slider->getValue());
    }
    CPGConnection* clickedCon = nodePanel.clickedConnection;
    if (clickedCon != nullptr) {
        ValueTree conTree = connectionPanel.paramTree.getChildWithName(clickedCon->getId());
        conTree.setProperty(slider->getName(), slider->getValue(), nullptr);
        int connectionNumber = clickedCon->getId().toString().getIntValue();
        int connectedNumber = clickedCon->getConnected()->getComponentID().getIntValue();
        int parentNumber = clickedCon->getParent()->getComponentID().getIntValue();
        if (slider->getName() == "weight") {
            setter.setWeight(connectedNumber,
                             parentNumber,
                             clickedCon->calculateWeight(slider->getValue()));
        }
        else {
            setter.setConParam(slider->getName(), parentNumber, connectedNumber, slider->getValue());
        }
    }
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
    setter.setParam("active", node->getComponentID().getIntValue(), 0);
    nodePanel.resized();

    ValueTree nodeParam(node->getComponentID());
    nodeParam.setProperty("grainLength", 200.0f, nullptr);
    nodeParam.setProperty("startTime", 0.0f, nullptr);
    nodeParam.setProperty("frequency", 1.0f, nullptr);
    nodeParam.setProperty("pan", 0.5f, nullptr);
    nodeParam.setProperty("volume", 0.7f, nullptr);
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
    CPGConnection* con = nodePanel.allCons.add(new CPGConnection(from, to));
    con->recalculatePath();
    setter.setWeight(con->getConnected()->getComponentID().getIntValue(),
                     con->getParent()->getComponentID().getIntValue(),
                     con->calculateWeight(1.0)
    );
    nodePanel.repaint();
    ValueTree conParam(con->getId());
    conParam.setProperty("weight", 1.0f, nullptr);
    conParam.setProperty("lengthWeight", 0.0f, nullptr);
    conParam.setProperty("startWeight", 0.0f, nullptr);
    connectionPanel.paramTree.addChild(conParam, -1, nullptr);
}

void MainComponent::componentMovedOrResized(Component &movedComp, bool wasMoved, bool wasResized) {
    CPGNode* node = dynamic_cast <CPGNode*> (&movedComp);
    if (node == 0) return;
    for (auto con : nodePanel.allCons) {
        con->recalculatePath();
        nodePanel.repaint();
        if (con->getParent() == node || con->getConnected() == node) {
            ValueTree conParams = connectionPanel.paramTree.getChildWithName(con->getId());
            double weightMult = conParams.getPropertyAsValue("weight", nullptr).getValue();
            setter.setWeight(con->getConnected()->getComponentID().getIntValue(),
                             con->getParent()->getComponentID().getIntValue(),
                             con->calculateWeight(weightMult)
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

    

