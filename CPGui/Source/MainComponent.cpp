/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    :nodePanel(NUM_NODES),
    controlsPanel(this)
{
    addAndMakeVisible(nodePanel);
    addAndMakeVisible(controlsPanel);
    /*
    controlsPanel.addSlider("grainLength", this, 5.0f, 5000.0f, 0.25f);
    controlsPanel.addSlider("startTime", this, 0.0f, 1000.0f, 1.0f);
    controlsPanel.addFrequencySlider(this);
    controlsPanel.addSlider("frequency", this, -1.5f, 1.0f, 1.0f);*/
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


    setSize (1000, 600);
    //makeControlNode(100, 100);
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
    //nodePanel.setBounds(0, 0, getWidth(), getHeight() - (getHeight() / 6));
    //controlsPanel.setBounds(0, getHeight() - (getHeight() / 6), getWidth(), (getHeight() / 6));
    fileComp->setBounds(width - 150, 0, 150, 50);
    mainFreqSlider.setBounds(width - 150, 50, 120, 120);
    addNodeButton.setBounds(width - 150, 170, 50, 50);

    Grid grid;

    using Track = Grid::TrackInfo;

    grid.templateRows = { Track(5_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr)};

    grid.items = { GridItem(nodePanel), GridItem(controlsPanel)};

    grid.performLayout(getLocalBounds());
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

/*void MainComponent::makeControlNode(int x, int y) {
    nodePanel.addAndMakeVisible(nodePanel.allNodes.add(new ControlNode(0, x, y)));
    CPGNode* node = nodePanel.allNodes.getLast();
    node->addComponentListener(this);
    node->setAlwaysOnTop(true);
    node->addMouseListener(this, false);
    nodePanel.resized();
}*/

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

void MainComponent::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) 
{
    if (fileComponentThatHasChanged == fileComp.get()) {
        setter.setFile(fileComp->getCurrentFile().getFullPathName());
    }
}

    

