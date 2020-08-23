/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    :
    setter{ new OSCParamSetter(8000) },
    nodeChangeListener{ new NodeChangeListener{ paramTree, setter.get()} },
    connectionChangeListener{ new ConnectionChangeListener{ paramTree, setter.get()} },
    mixerChangeListener{ new MixerChangeListener{ paramTree, setter.get()} }
{
    this->setLookAndFeel(&LandF);
    paramTree.addChild(nodeParams, 0, nullptr);
    paramTree.addChild(conParams, 1, nullptr);

    for (int i{ 0 }; i < NUM_NODES; i++) {
        nodeParams.addChild(makeNodeValueTree(i), i, nullptr);
    }    
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        conParams.addChild(makeConnectionValueTree(i), i, nullptr);
    }

    conMenu.reset(new ConnectionMenu{ conParams, &clickedCon, this });
    nodeMenu.reset(new NodeMenu{ nodeParams, &clickedNode });
    mixerMenu.reset(new MixerMenu(nodeParams, this));
    samplePanel.reset(new SamplePicker{ setter.get(), nodeParams });

    nodeParams.addListener(nodeChangeListener.get());
    conParams.addListener(connectionChangeListener.get());

    addAndMakeVisible(nodePanel);
    addAndMakeVisible(samplePanel.get());
    addChildComponent(mixerMenu.get());

    nodePanel.setInterceptsMouseClicks(false, true);
    /*Extra main controls, could put these elsewhere?*/
    DSPButton.addListener(this);
    addAndMakeVisible(&DSPButton);
    DSPButton.setButtonText("Toggle DSP");
    DSPLabel.setText("Toggle DSP", dontSendNotification);
    DSPLabel.attachToComponent(&DSPButton, true);
    addAndMakeVisible(&DSPLabel);

    toggleMixer.addListener(this);
    addAndMakeVisible(&toggleMixer);
    toggleMixer.setButtonText("Toggle Mixer");
    toggleMixerLabel.setText("Toggle Mixer", dontSendNotification);
    toggleMixerLabel.attachToComponent(&toggleMixer, true);
    addAndMakeVisible(&toggleMixerLabel);
    setSize (1000, 600);
    //loadPreset("SlowMod");
    if (!connect(8001))                       // [3]
        showConnectionErrorMessage("Error: could not connect to UDP port 8001.");

    addListener(this, "/CPG/");     // [4]
}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll(findColour(juce::ResizableWindow::backgroundColourId));
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        if (conParams.getChild(i).getProperty(Ids::active)) {
            cons[i]->recalculatePath();
            g.setColour(Colours::black);
            g.fillPath(*cons[i]->getPath());
            g.setColour(Colours::darkgreen);
            g.fillPath(*cons[i]->getLengthPath());
            g.setColour(Colours::darkred);
            g.fillPath(*cons[i]->getSizePath());
        }
    }
}

void MainComponent::resized()
{

    Grid grid;

    using Track = Grid::TrackInfo;

    grid.templateRows = { Track(10_fr), Track(1_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr), Track(9_fr) };
    if (mixerShowing) {
        grid.items = { GridItem(mixerMenu.get()).withArea(1, GridItem::Span(2)),
            GridItem(samplePanel.get()).withArea(GridItem::Span(2), 2),
            GridItem(toggleMixer).withArea(2, 1),
            GridItem(DSPButton).withArea(3, 1)
        };
    }
    else {
        grid.items = { GridItem(nodePanel).withArea(1, GridItem::Span(2)),
            GridItem(samplePanel.get()).withArea(GridItem::Span(2), 2),
            GridItem(toggleMixer).withArea(2, 1),
            GridItem(DSPButton).withArea(3, 1)
        };
    }
    grid.performLayout(getLocalBounds());

    for (int i{ 0 }; i < NUM_NODES; i++) {
        CPGNode* node = nodes[i].get();
        if (node != nullptr) {
            double x = nodeParams.getChild(i).getProperty(Ids::x);
            double y = nodeParams.getChild(i).getProperty(Ids::y);
            node->setBounds(x * (double) this->getWidth(), y * (double) this->getHeight(),
                NODESIZE, NODESIZE);
            double grainPosition = nodeParams.getChild(i).getProperty(Ids::startTime);
            Component* sliderBox = node->getSliderBox();
            sliderBox->setBounds(grainPosition * sliderBox->getParentWidth(), 0,
                sliderBox->getParentHeight() / 4, sliderBox->getParentHeight() / 4);
        }
    }

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
            const int deleteConRequested = conMenu->showMenu();
            if (deleteConRequested) {
                deleteConnection(i);
            }
            return;
        }
    }
}

void MainComponent::mouseDoubleClick(const MouseEvent& e) 
{
    CPGNode* node = dynamic_cast <CPGNode*> (e.eventComponent);
    if (!node) {
        makeNode(e.mouseDownPosition.getX() - NODESIZE / 2, e.mouseDownPosition.getY() - NODESIZE / 2);
    }
    else {
        clickedNode = node->getNodeNumber();
        const int deleteNodeRequested = nodeMenu->showMenu();
        if (deleteNodeRequested) {
            deleteNode(clickedNode);
        }
    }
}

void MainComponent::buttonClicked(Button* button)
{
    if (button == &DSPButton) {
        DSPOn = !DSPOn;
        setter->setDSPState(DSPOn);
        if (DSPOn) {
            sendStateToDSP();
        }
    }
    else if (button == &toggleMixer) {
        mixerShowing = !mixerShowing;
        if (mixerShowing) {
            mixerMenu->setVisible(true);
            nodePanel.setVisible(false);
            animator.cancelAllAnimations(true);
        }
        else {
            mixerMenu->setVisible(false);
            nodePanel.setVisible(true);
        }
        resized();
    }
}

void MainComponent::makeNode(int x, int y)
{
    int nodeId = -1;
    for (int i = 0; i < NUM_NODES; i++) {
        if (nodes[i].get() == nullptr) {
            nodeId = i;
            break;
        }
    }
    if (nodeId == -1) return;
    ValueTree nodeTree = nodeParams.getChild(nodeId);
    nodes[nodeId].reset(new CPGNode(nodeId, x, y, nodeTree));
    nodes[nodeId]->addComponentListener(this);
    nodes[nodeId]->addMouseListener(this ,false);
    nodePanel.addAndMakeVisible(nodes[nodeId].get());
    samplePanel->addAndMakeVisible(nodes[nodeId]->getSliderBox());
    nodeTree.setProperty(Ids::active, true, nullptr);
    nodeTree.setProperty(Ids::x, (double) x / (double) getWidth(), nullptr);
    nodeTree.setProperty(Ids::y, (double) y / (double) getHeight(), nullptr);
    resized();
}

void MainComponent::deleteNode(int nodeId) {
    nodeParams.getChild(nodeId).setProperty(Ids::active, false, nullptr);
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
        cons[connectionIndex].reset(new CPGConnection{ to, from, conParams.getChild(connectionIndex)});
        conParams.getChild(connectionIndex).setProperty(Ids::active, true, nullptr);
        conParams.getChild(connectionIndex).setProperty(Ids::from, from->getNodeNumber(), nullptr);
        conParams.getChild(connectionIndex).setProperty(Ids::to, to->getNodeNumber(), nullptr);
        connectionChangeListener->valueTreePropertyChanged(conParams.getChild(connectionIndex), Ids::weight);
        cons[connectionIndex]->recalculatePath();
    }
    repaint(); 
}

void MainComponent::deleteConnection(int conId)
{
    cons[conId].reset();
    conParams.getChild(conId).setProperty(Ids::active, false, nullptr);
    conParams.getChild(conId).setProperty(Ids::weight, 0.0, nullptr);
    conParams.getChild(conId).setProperty(Ids::lengthMod, 0.0, nullptr);
    conParams.getChild(conId).setProperty(Ids::startMod, 0.0, nullptr);
    clickedCon = -1;
    repaint();
}


int MainComponent::getConnectionIndex(int from, int to)
{
    if (from == to) return -1;
    int connectionIndex;
    if (from == 0) connectionIndex = to - 1;
    else if (to == 0) connectionIndex = from - 1;
    else connectionIndex = to + from;
    return connectionIndex;
}

void MainComponent::componentMovedOrResized(Component& movedComp, bool wasMoved, bool wasResized)
{
    if (wasMoved) {
        CPGNode* node = dynamic_cast <CPGNode*> (&movedComp);
        if (node == 0) return;
        double xCentreRelative = (double)node->getX() / (double)getWidth();
        double yCentreRelative = (double)node->getY() / (double)getHeight();
        nodeParams.getChild(node->getNodeNumber()).setProperty(Ids::x, xCentreRelative, nullptr);
        nodeParams.getChild(node->getNodeNumber()).setProperty(Ids::y, yCentreRelative, nullptr);
        int nodeNumber = node->getNodeNumber();
        for (int i{ 0 }; i < NUM_NODES; i++) {
            int connectionIndex = getConnectionIndex(nodeNumber, i);
            if (nodeNumber != i && cons[connectionIndex] != nullptr) {
                cons[connectionIndex]->recalculatePath();
                connectionChangeListener->valueTreePropertyChanged(conParams.getChild(connectionIndex), Ids::weight);
            }
        }
        repaint();
    }
}

void MainComponent::actionListenerCallback(const String& message)
{
    if (message == "loadPreset") {
        loadPreset(mixerMenu->getPresetFilePath());
    }
    else if (message == "savePreset") {
        savePreset(mixerMenu->savePresetFilePath());
    }
    else if (message == "portInChanged") {
        connect(mixerMenu->getPortInNumber().getIntValue());
    }
    else if (message == "portOutChanged") {
        setter->changePortOutNumber(mixerMenu->getPortOutNumber().getIntValue());
    }
}

/*void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    loadPreset(mixerMenu->getPresetFilePath());
}*/

void MainComponent::savePreset(String fileName)
{
    File fileLocation{ juce::File::getSpecialLocation(juce::File::SpecialLocationType::hostApplicationPath)
        .getParentDirectory().getChildFile("Presets/" + fileName + ".xml") };
    paramTree.createXml()->writeTo(fileLocation);
}

void MainComponent::loadPreset(String fileName)
{
    File fileLocation(fileName);
    if (fileLocation.existsAsFile()) {
        resetState();
        XmlDocument xml{ fileLocation };
        auto xmlElement = xml.getDocumentElement();
        ValueTree newTree = paramTree.fromXml(xmlElement->toString());
        nodeParams.copyPropertiesAndChildrenFrom(newTree.getChild(0), nullptr);
        conParams.copyPropertiesAndChildrenFrom(newTree.getChild(1), nullptr);
        for (int i = 0; i < NUM_NODES; i++) {
            if (nodeParams.getChild(i).getProperty(Ids::active)) {
                makeNode((double) nodeParams.getChild(i).getProperty(Ids::x) * getWidth(), (double) nodeParams.getChild(i).getProperty(Ids::y) * getHeight());
            }
        }
        for (int i = 0; i < NUM_CONNECTIONS; i++) {
            if (conParams.getChild(i).getProperty(Ids::active)) {
                makeConnection(nodes[conParams.getChild(i).getProperty(Ids::from)].get(),
                    nodes[conParams.getChild(i).getProperty(Ids::to)].get());
            }
        }
    }
    sendStateToDSP();
    mixerMenu->visibilityChanged();
}

void MainComponent::resetState() 
{
    for (int i = 0; i < NUM_NODES; i++) {
        deleteNode(i);
    }
    for (int i = 0; i < NUM_CONNECTIONS; i++) {
        deleteConnection(i);
    }
}

void MainComponent::sendStateToDSP()
{
    for (auto param : paramTree) {
        for (auto child : param) {
            for (int i = 0; i < child.getNumProperties(); i++) {
                child.sendPropertyChangeMessage(child.getPropertyName(i));
            }
        }
    }
    String file = samplePanel->getFileName();
    if (!file.isEmpty()) {
        setter->setFile(samplePanel->getFileName());
    }
}

void MainComponent::oscMessageReceived(const juce::OSCMessage& message)
{
    if (message.size() >= 1 && message[0].isInt32() && !mixerShowing) {
        int nodeNumber = message[0].getInt32();
        if (nodes[nodeNumber] != nullptr) {
            CPGTriggerComponent trig{ juce::Colour::fromString(colours[nodeNumber]) };
            trig.setSize(NODESIZE, NODESIZE);
            trig.setTopLeftPosition(nodes[nodeNumber]->getPosition());
            trig.setInterceptsMouseClicks(false, false);
            addAndMakeVisible(trig);
            animator.animateComponent(&trig, trig.getBounds().expanded(10), 0.0f, 
                std::fmin(nodeParams.getChild(nodeNumber).getProperty(Ids::grainLength), 400.0), true, 1.0f, 1.0f);
        }
    }
}

void MainComponent::showConnectionErrorMessage(const juce::String& messageText)
{
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
        "Connection error",
        messageText,
        "OK");
}


ValueTree MainComponent::makeNodeValueTree(int nodeId)
{
    return ValueTree{ "node" + String{nodeId} }
        .setProperty(Ids::active, false, nullptr)
        .setProperty(Ids::muted, false, nullptr)
        .setProperty(Ids::x, 0.0, nullptr)
        .setProperty(Ids::y, 0.0, nullptr)
        .setProperty(Ids::grainLength, 200.0, nullptr)
        .setProperty(Ids::startTime, 0.0, nullptr)
        .setProperty(Ids::frequency, 1.0, nullptr)
        .setProperty(Ids::pan, 0.5f, nullptr)
        .setProperty(Ids::volume, 0.7, nullptr)
        .setProperty(Ids::colour, colours[nodeId], nullptr);
}

ValueTree MainComponent::makeConnectionValueTree(int connectionIndex)
{
    return ValueTree{ "con" + String{connectionIndex } }
        .setProperty(Ids::active, false, nullptr)
        .setProperty(Ids::from, 0, nullptr)
        .setProperty(Ids::to, 0, nullptr)
        .setProperty(Ids::weight, 1.0, nullptr)
        .setProperty(Ids::weightDir, 0.0, nullptr)
        .setProperty(Ids::lengthMod, 0.0, nullptr)
        .setProperty(Ids::lengthModDir, 0.0, nullptr)
        .setProperty(Ids::startMod, 0.0, nullptr)
        .setProperty(Ids::startModDir, 0.0, nullptr);
}
