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
#include "NodePanel.h"
#include "NodeControls.h"
#include "ConnectionControls.h"
#include <stack>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public Component,
                        public ComponentListener,
                        public Button::Listener,
                        public Slider::Listener,
                        public FilenameComponentListener
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
private:
    void makeNode(int x, int y);
    void makeConnection(CPGNode* from, CPGNode* to);
    void componentMovedOrResized(Component& movedComp, bool wasMoved, bool wasResized);
    void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;

    CPGLookAndFeel LandF;
    nodeContainer nodePanel;
    NodeControls controlsPanel;
    ConnectionControls connectionPanel;
    ControlInterface* currentPanel;
    TextButton addNodeButton;
    ToggleButton DSPButton;
    Slider mainFreqSlider;
    std::unique_ptr<FilenameComponent> fileComp;
    Label addButtonLabel, mFreqLabel, fileLabel, DSPLabel;
    OSCParamSetter setter{ 8000 };
    bool DSPOn{ false };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
