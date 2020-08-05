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
    mixerChangeListener{ new MixerChangeListener{ paramTree, setter.get()} },
    conMenu(conParams, &clickedCon, this),
    nodeMenu(nodeParams, &clickedNode)
{
    paramTree.addChild(nodeParams, 0, nullptr);
    paramTree.addChild(conParams, 1, nullptr);
    paramTree.addChild(mixerParams, 2, nullptr);

    for (int i{ 0 }; i < NUM_NODES; i++) {
        nodeParams.addChild(makeNodeValueTree(i), i, nullptr);
        mixerParams.addChild(makeMixerValueTree(i), i, nullptr);
    }
 
    for (int i{ 0 }; i < NUM_CONNECTIONS; i++) {
        conParams.addChild(makeConnectionValueTree(i), i, nullptr);
    }

    mixerParams.setProperty(Ids::pan, 0.5, nullptr);
    mixerParams.setProperty(Ids::volume, 1.0, nullptr);

    nodeParams.addListener(nodeChangeListener.get());
    conParams.addListener(connectionChangeListener.get());
    mixerParams.addListener(mixerChangeListener.get());

    for (int i{ NUM_NODES - 1 }; i >= 0; i--) {
        availableNodes.push(i);
    }

    mixerMenu.reset(new MixerMenu(mixerParams));

    addAndMakeVisible(nodePanel);
    addAndMakeVisible(samplePanel);
    addChildComponent(mixerMenu.get());

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

    toggleMixer.addListener(this);
    addAndMakeVisible(&toggleMixer);
    toggleMixerLabel.setText("Toggle Mixer", dontSendNotification);
    toggleMixerLabel.attachToComponent(&toggleMixer, true);
    addAndMakeVisible(&toggleMixerLabel);
    addPreset.addListener(this);
    addAndMakeVisible(&addPreset);
    setSize (1000, 600);
    loadPreset("test");
    //makeNode(100, 200);
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
    toggleMixer.setBounds(width - 100, 150, 50, 50);
    addPreset.setBounds(width - 100, 200, 50, 50);

    for (int i{ 0 }; i < NUM_NODES; i++) {
        CPGNode* test = nodes[i].get();
        if (test != nullptr) {
            test->setBounds(std::min(test->getParentWidth() - NODESIZE, test->getX()),
                std::min(test->getParentHeight() - NODESIZE, test->getY()),
                NODESIZE, NODESIZE);
            Component* sliderBox = test->getSliderBox();
            sliderBox->setBounds(sliderBox->getX(), sliderBox->getY(), sliderBox->getParentHeight() / 4, sliderBox->getParentHeight() / 4);
        }
       
    }
    Grid grid;

    using Track = Grid::TrackInfo;
   
    grid.templateRows = { Track(5_fr), Track(1_fr) };
    grid.templateColumns = { Track(1_fr)};
    if (mixerShowing) {
        grid.items = { GridItem(nodePanel), GridItem(mixerMenu.get()) };
    }
    else {
        grid.items = { GridItem(nodePanel), GridItem(samplePanel) };
    }
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
        if(DSPOn) sendStateToDSP();
    }
    else if (button == &toggleMixer) {
        if (mixerShowing) {
            mixerMenu->setVisible(false);
            samplePanel.setVisible(true);
        }
        else {
            mixerMenu->setVisible(true);
            samplePanel.setVisible(false);
        }
        mixerShowing = !mixerShowing;
        resized();
    }
    else if (button == &addPreset) {
        savePreset("test");
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
    nodes[nodeId].reset(new CPGNode(nodeId, x, y, nodeParams.getChild(nodeId), juce::Colour::fromString(colours[nodeId])));
    nodes[nodeId]->addComponentListener(this);
    nodes[nodeId]->addMouseListener(this ,false);
    nodePanel.addAndMakeVisible(nodes[nodeId].get());
    samplePanel.addAndMakeVisible(nodes[nodeId]->getSliderBox());
    setter->setParam("active", nodeId, 0);
    nodeParams.getChild(nodeId).setProperty(Ids::active, true, nullptr);
    resized();
}

void MainComponent::deleteNode(int nodeId) {
    availableNodes.push(nodeId);
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
        connectionChangeListener->valueTreePropertyChanged(conParams.getChild(connectionIndex), "weight");
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

ValueTree MainComponent::makeNodeValueTree(int nodeId)
{
    return ValueTree{ "node" + String{nodeId} }
        .setProperty(Ids::active, false, nullptr)
        .setProperty(Ids::x, 0.0, nullptr)
        .setProperty(Ids::y, 0.0, nullptr)
        .setProperty(Ids::grainLength, 200.0, nullptr)
        .setProperty(Ids::startTime, 0.0, nullptr)
        .setProperty(Ids::frequency, 1.0, nullptr)
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

ValueTree MainComponent::makeMixerValueTree(int nodeId)
{
    return ValueTree{ "nodeMix" + String{nodeId}  }
        .setProperty(Ids::pan, 0.5f, nullptr)
        .setProperty(Ids::volume, 0.7, nullptr)
        .setProperty(Ids::colour, colours[nodeId], nullptr);
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

void MainComponent::sendStateToDSP()
{
    for (auto param : paramTree) {
        for (auto child : param) {
            for (int i = 0; i < child.getNumProperties(); i++) {
                child.sendPropertyChangeMessage(child.getPropertyName(i));
            }
        }
    }
}

void MainComponent::savePreset(String fileName)
{
    //File fileLocation{ "C:\\Users\\Alex\\Documents\\MProj\\CPGrains\\CPGui\\Builds\\VisualStudio2019\\x64\\Debug\\App\\test.txt" };
    File fileLocation{ juce::File::getSpecialLocation(juce::File::SpecialLocationType::hostApplicationPath)
        .getParentDirectory().getFullPathName() + "\\" + fileName + ".xml"};
    paramTree.createXml()->writeTo(fileLocation);
}

void MainComponent::loadPreset(String fileName)
{
    File fileLocation{ juce::File::getSpecialLocation(juce::File::SpecialLocationType::hostApplicationPath)
        .getParentDirectory().getFullPathName() + "\\" + fileName + ".xml" };
    if (fileLocation.existsAsFile()) {
        XmlDocument xml{ fileLocation };
        auto xmlElement = xml.getDocumentElement();
        ValueTree newTree = paramTree.fromXml(xmlElement->toString());
        //paramTree.copyPropertiesAndChildrenFrom(newTree, nullptr);
        nodeParams.copyPropertiesAndChildrenFrom(newTree.getChild(0), nullptr);
        conParams.copyPropertiesAndChildrenFrom(newTree.getChild(1), nullptr);
        mixerParams.copyPropertiesAndChildrenFrom(newTree.getChild(2), nullptr);
        for (int i = 0; i < NUM_NODES; i++) {
            if (nodeParams.getChild(i).getProperty(Ids::active)) {
                deleteNode(i);
                makeNode(nodeParams.getChild(i).getProperty(Ids::x), nodeParams.getChild(i).getProperty(Ids::y));
            }
            else {
                deleteNode(i);
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
}

void MainComponent::componentMovedOrResized(Component &movedComp, bool wasMoved, bool wasResized) {
    CPGNode* node = dynamic_cast <CPGNode*> (&movedComp);
    if (node == 0) return;
    nodeParams.getChild(node->getNodeNumber()).setProperty(Ids::x, node->getX(), nullptr);
    nodeParams.getChild(node->getNodeNumber()).setProperty(Ids::y, node->getY(), nullptr);
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

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    setter->setFile(samplePanel.getFileName());
}