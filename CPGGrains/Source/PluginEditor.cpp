/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"
//==============================================================================
CpggrainsAudioProcessorEditor::CpggrainsAudioProcessorEditor(CpggrainsAudioProcessor& p, AudioProcessorValueTreeState& state)
    :AudioProcessorEditor(&p),
    proc(p),
    nodePanel(NUM_NODES),
    params(state),
    controlsPanel(params, this, &LandF)
{
    addAndMakeVisible(nodePanel);
    addAndMakeVisible(controlsPanel);
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
        { f },                       // current file
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

    setResizable(true, true);
    setSize(1000, 600);
    //makeControlNode(100, 100);
}

CpggrainsAudioProcessorEditor::~CpggrainsAudioProcessorEditor()
{
}

//==============================================================================
void CpggrainsAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.setColour(Colours::white);
}

void CpggrainsAudioProcessorEditor::resized()
{
    int width = getWidth();
    //nodePanel.setBounds(0, 0, getWidth(), getHeight() - (getHeight() / 6));
    //controlsPanel.setBounds(0, getHeight() - (getHeight() / 6), getWidth(), (getHeight() / 6));
    fileComp->setBounds(width - 150, 0, 150, 50);
    mainFreqSlider.setBounds(width - 150, 50, 120, 120);
    addNodeButton.setBounds(width - 150, 170, 50, 50);
    DSPButton.setBounds(width - 150, 220, 50, 50);

    Grid grid;

    using Track = Grid::TrackInfo;

    grid.templateRows = { Track(5_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr) };
    grid.items = { GridItem(nodePanel), GridItem(controlsPanel) };
    grid.performLayout(getLocalBounds());
}

void CpggrainsAudioProcessorEditor::mouseDown(const MouseEvent& e)
{
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (nodePanel.clickedNode != nullptr && node != 0 && e.mods.isShiftDown()) {
        makeConnection(nodePanel.clickedNode, node);
        return;
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

void CpggrainsAudioProcessorEditor::mouseDoubleClick(const MouseEvent& e) {
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


void CpggrainsAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &addNodeButton) {
        makeNode(rand() % nodePanel.getWidth() - 50, rand() % nodePanel.getHeight() - 50);
    }
    else if (button == &DSPButton) {
        DSPOn = !DSPOn;
        setter.setDSPState(DSPOn);
    }
}

void CpggrainsAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    CPGNode* clickedNode = nodePanel.clickedNode;
    if (clickedNode == nullptr) return;




    /*ValueTree nodeTree = controlsPanel.paramTree.getChildWithName(clickedNode->getComponentID());
    nodeTree.setProperty(slider->getName(), slider->getValue(), nullptr);
    setter.setParam(slider->getName(), clickedNode->getComponentID().getIntValue(), slider->getValue());*/
}

/*void CpggrainsAudioProcessorEditor::makeControlNode(int x, int y) {
    nodePanel.addAndMakeVisible(nodePanel.allNodes.add(new ControlNode(0, x, y)));
    CPGNode* node = nodePanel.allNodes.getLast();
    node->addComponentListener(this);
    node->setAlwaysOnTop(true);
    node->addMouseListener(this, false);
    nodePanel.resized();
}*/

void CpggrainsAudioProcessorEditor::makeNode(int x, int y)
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
    //controlsPanel.paramTree.addChild(nodeParam, -1, nullptr);
}

void CpggrainsAudioProcessorEditor::makeConnection(CPGNode* from, CPGNode* to)
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

void CpggrainsAudioProcessorEditor::componentMovedOrResized(Component& movedComp, bool wasMoved, bool wasResized) {
    CPGNode* node = dynamic_cast <CPGNode*> (&movedComp);
    if (node == 0) return;
    for (auto con : nodePanel.allCons) {
        con->recalculatePath();
        nodePanel.repaint();
        if (con->getParent() == node || con->getConnected() == node) {
            proc.setWeight(con->getConnected()->getComponentID().getIntValue(),
                con->getParent()->getComponentID().getIntValue(),
                con->calculateWeight()
            );
        }
    }
    RangedAudioParameter* xPosition = params.getParameter(String(node->getComponentID()) + "x");
    RangedAudioParameter* yPosition = params.getParameter(String(node->getComponentID()) + "y");
    float x = (float)node->getX();
    float xBounds = (float)node->getParentWidth() - node->getWidth();
    xPosition->setValueNotifyingHost(x / xBounds);
    float y = (float)node->getY();
    float yBounds = (float)node->getParentHeight() - node->getHeight();
    yPosition->setValueNotifyingHost(y / yBounds);
}

void CpggrainsAudioProcessorEditor::filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged)
{
    if (fileComponentThatHasChanged == fileComp.get()) {
        setter.setFile(fileComp->getCurrentFile().getFullPathName());
    }
}



