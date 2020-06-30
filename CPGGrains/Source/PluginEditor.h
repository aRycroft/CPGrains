/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "CPGLookAndFeel.h"
#include "CPGNode.h"
#include "OSCParamSetter.h"
#include <stack>


typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
//==============================================================================
/**
*/
class CpggrainsAudioProcessorEditor  : public AudioProcessorEditor,
    public ComponentListener,
    public Button::Listener,
    public Slider::Listener,
    public FilenameComponentListener
{
public:
    //==============================================================================
    CpggrainsAudioProcessorEditor(CpggrainsAudioProcessor&, AudioProcessorValueTreeState&);
    ~CpggrainsAudioProcessorEditor();

    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
private:
    void makeNode(int x, int y);
    void makeConnection(CPGNode* from, CPGNode* to);
    void componentMovedOrResized(Component& movedComp, bool wasMoved, bool wasResized);
    void mouseDown(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent& event) override;
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;
    void filenameComponentChanged(FilenameComponent* fileComponentThatHasChanged) override;

    struct nodeContainer : public Component {
        nodeContainer(int numNodes) {
            allNodes.ensureStorageAllocated(NUM_NODES);
            allCons.ensureStorageAllocated(NUM_NODES * NUM_NODES);
            for (int i = NUM_NODES; i > 0; i--) {
                availableNodes.push(i);
            }
        }
        void paint(Graphics& g) override
        {
            g.fillAll(Colours::cadetblue);
            for (auto con : allCons) {
                g.strokePath(con->getPath(), PathStrokeType(2));
            }
        }

        void resized() override
        {
            //int nodeSize = std::min(proportionOfHeight(0.12), proportionOfWidth(0.12));
            int nodeSize = 70.0f;
            for (auto node : allNodes) {
                node->setBounds(std::min(getParentWidth() - nodeSize, node->getX()), std::min(getParentHeight() - nodeSize, node->getY()), nodeSize, nodeSize);
            }
            for (auto con : allCons) {
                con->recalculatePath();
            }
        }

        CPGNode* clickedNode{ nullptr };
        std::stack<int> availableNodes;
        OwnedArray<CPGNode> allNodes;
        OwnedArray<CPGConnection> allCons;
    };

    struct controlsContainer : public Component,
        private ChangeListener {

        controlsContainer(AudioProcessorValueTreeState& state, Slider::Listener* listener, LookAndFeel_V4* LandF)
            :
            paramTree(state),
            waveformCache(5),
            waveform(512, waveformManager, waveformCache)
        {
            waveformManager.registerBasicFormats();
            waveform.addChangeListener(this);
            //paramTree = ValueTree(Identifier("nodeParams"));
            addSliders(listener, LandF);
        }

        void paint(Graphics& g) override
        {
            g.fillAll(Colours::darkslategrey);

            waveform.drawChannels(g,                                     
                Rectangle<int>(100, 100),
                0.0,                                    
                waveform.getTotalLength(),            
                1.0f);

        }

        void resized() override
        {
            Grid grid;

            using Track = Grid::TrackInfo;
            grid.templateRows = { Track(1_fr) };
            grid.templateColumns = { Track(1_fr), Track(3_fr),
                                     Track(1_fr), Track(3_fr),
                                     Track(1_fr), Track(3_fr),
                                     Track(1_fr), Track(3_fr),
                                     Track(1_fr), Track(3_fr) };
            for (Slider* s : sliders) {
                GridItem g1 = GridItem(s);
                grid.items.add(GridItem());
                grid.items.add(g1);
                grid.performLayout(getLocalBounds());
            }
        }

        void addSliders(Slider::Listener* listener, LookAndFeel_V4* LandF) {
            addChildComponent(sliders.add(new Slider("grainLength")));
            addChildComponent(labels.add(new Label()));
            Label* l = labels.getLast();
            l->setText("Grain Size", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("startTime")));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Position", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("frequency")));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Freq", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("pan")));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Pan", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("volume")));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Volume", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            for (auto* slider : sliders) {
                slider->setLookAndFeel(LandF);
                slider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, false, 40, 40);
                slider->setSliderStyle(Slider::Rotary);
                slider->addListener(listener);
            }
        }

        void displayControls(String componentId) {
            if (!displayingControls) {
                showSliders(componentId);
            }
            else if (displayingWaveform) {
                hideWaveform();
            }
        }

        void displayWaveform() {
            if (!displayingWaveform) {
                showWaveform();
            }
            if (displayingControls) {
                hideSliders();
            }
        }

        void showSliders(String componentId)
        {
            attachments.clear();
            for (auto* slider : sliders) {
                slider->setVisible(true);
                attachments.add(std::make_unique<SliderAttachment>(
                    paramTree,
                    String(componentId) + slider->getName(),
                    *slider));
            }
            for (auto& label : labels) {
                label->setVisible(true);
            }
            resized();
        }

        void hideSliders() {
            for (auto slider : sliders) {
                slider->setVisible(false);
            }
            for (auto label : labels) {
                label->setVisible(false);
            }
        }

        void showWaveform() {

        }

        void hideWaveform() {

        }

        void changeListenerCallback(ChangeBroadcaster* source) override
        {
            repaint();
        }

        AudioFormatManager waveformManager;
        AudioThumbnailCache waveformCache;
        AudioThumbnail waveform;

        AudioProcessorValueTreeState& paramTree;
        OwnedArray<Slider> sliders;
        OwnedArray<SliderAttachment> attachments;
        OwnedArray<Label> labels;
        bool displayingControls{ false };
        bool displayingWaveform{ false };
    };

    CpggrainsAudioProcessor& proc;
    AudioProcessorValueTreeState& params;
    CPGLookAndFeel LandF;
    nodeContainer nodePanel;
    controlsContainer controlsPanel;

    TextButton addNodeButton;
    ToggleButton DSPButton;
    Slider mainFreqSlider;
    Label addButtonLabel, mFreqLabel, fileLabel, DSPLabel;

    std::unique_ptr<FilenameComponent> fileComp;
    OSCParamSetter setter{ 8000 };
    bool DSPOn{ false };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CpggrainsAudioProcessorEditor)
};
