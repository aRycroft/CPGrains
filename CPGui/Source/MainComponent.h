/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CPGLookAndFeel.h"
#include "CPGNode.h"
#include "OSCParamSetter.h"
#include "NodeChangeListener.h"
#include "ConnectionChangeListener.h"
#include <stack>
#include "ConnectionMenu.h"
#include "NodeMenu.h"
#include "SamplePicker.h"
#define NUM_NODES 4
#define NUM_CONNECTIONS (NUM_NODES * (NUM_NODES - 1)) / 2
#define NODESIZE 70

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component,
                        public ComponentListener,
                        public Button::Listener,
                        public Slider::Listener,
                        public ValueTree::Listener,
                        public ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent& event) override;
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    void componentMovedOrResized(Component& movedComp, bool wasMoved, bool wasResized);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    //void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;
private:
    void makeNode(int x, int y);
    void deleteNode(int nodeId);
    void makeConnection(CPGNode* from, CPGNode* to);
    /*void showConnectionMenu(int connectionIndex);
    void changeMenuSliders(Identifier i, Identifier direction);
    void setUpConnectionMenu();
    */void setUpMenu() {};
    ValueTree makeNodeValueTree(int nodeId);
    ValueTree makeConnectionValueTree(int connectionIndex);
    int getConnectionIndex(int from, int to);

    CPGLookAndFeel LandF;
    Component nodePanel;
    SamplePicker samplePanel;
    TextButton addNodeButton;
    ToggleButton DSPButton;
    Slider mainFreqSlider;
    std::unique_ptr<FilenameComponent> fileComp;
    Label addButtonLabel, mFreqLabel, fileLabel, DSPLabel;
    bool DSPOn{ false };

    /*Popup menu stuff, could put this is another class*/
    std::unique_ptr<OSCParamSetter> setter;
    std::stack<int> availableNodes;
    ValueTree paramTree{ "params" };
    ValueTree nodeParams{ "nodeParams" };
    ValueTree conParams{ "conParams" };
    std::unique_ptr<CPGNode> nodes[NUM_NODES];
    std::unique_ptr<CPGConnection> cons[NUM_CONNECTIONS];
    int clickedNode{ -1 };
    int clickedCon{ -1 };
    std::unique_ptr<NodeChangeListener> nodeChangeListener;
    std::unique_ptr<ConnectionChangeListener> connectionChangeListener;
    ConnectionMenu conMenu;
    NodeMenu nodeMenu;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
    