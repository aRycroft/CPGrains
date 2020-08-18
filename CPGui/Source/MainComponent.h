/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Identifiers.h"
#include "CPGLookAndFeel.h"
#include "CPGNode.h"
#include "CPGConnection.h"
#include "OSCParamSetter.h"
#include "NodeChangeListener.h"
#include "ConnectionChangeListener.h"
#include "MixerChangeListener.h"
#include "ConnectionMenu.h"
#include "NodeMenu.h"
#include "SamplePicker.h"
#include "MixerMenu.h"
#include "CPGTriggerComponent.h"


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
                        public ValueTree::Listener,
                        public ChangeListener,
                        private juce::OSCReceiver,
                        private juce::OSCReceiver::ListenerWithOSCAddress<juce::OSCReceiver::MessageLoopCallback>
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
    void componentMovedOrResized(Component& movedComp, bool wasMoved, bool wasResized);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
private:
    void makeNode(int x, int y);
    void deleteNode(int nodeId);
    void makeConnection(CPGNode* from, CPGNode* to);
    void deleteConnection(int conId);
    int getConnectionIndex(int from, int to);

    void savePreset(String fileName);
    void loadPreset(String fileName);
    void resetState();
    void sendStateToDSP();

    void oscMessageReceived(const juce::OSCMessage& message) override;
    void showConnectionErrorMessage(const juce::String& messageText);

    ValueTree makeNodeValueTree(int nodeId);
    ValueTree makeConnectionValueTree(int connectionIndex);

    CPGLookAndFeel LandF;
    Component nodePanel;
    TextButton toggleMixer, addPreset;
    ToggleButton DSPButton;
    Slider mainFreqSlider;
    std::unique_ptr<FilenameComponent> fileComp;
    Label mFreqLabel, fileLabel, DSPLabel, toggleMixerLabel;
    bool DSPOn{ false };
    bool mixerShowing{ false };

    std::unique_ptr<OSCParamSetter> setter;
    ValueTree paramTree{ "params" };
    ValueTree nodeParams{ "nodeParams" };
    ValueTree conParams{ "conParams" };
    std::unique_ptr<CPGNode> nodes[NUM_NODES];
    std::unique_ptr<CPGConnection> cons[NUM_CONNECTIONS];
    int clickedNode{ -1 };
    int clickedCon{ -1 };
    std::unique_ptr<NodeChangeListener> nodeChangeListener;
    std::unique_ptr<ConnectionChangeListener> connectionChangeListener;
    std::unique_ptr<MixerChangeListener> mixerChangeListener;
    std::unique_ptr<ConnectionMenu> conMenu;
    std::unique_ptr<NodeMenu> nodeMenu;
    std::unique_ptr<MixerMenu> mixerMenu;
    std::unique_ptr<SamplePicker> samplePanel;
    String colours[NUM_NODES] = { 
       "ff2EC4B6",
       "fff3722c",
       "ffED5034",
       "fff9c74f"
    };
    juce::ComponentAnimator animator;
    OwnedArray<CPGTriggerComponent> triggers;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
    