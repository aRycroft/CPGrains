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
#include <stack>

#define NUM_NODES 4

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
                               private ChangeListener{

        controlsContainer(Slider::Listener* listener, LookAndFeel_V4* LandF)
            :
            waveformCache(5),
            waveform(512, waveformManager, waveformCache)
        {
            waveformManager.registerBasicFormats();
            waveform.addChangeListener(this);
            paramTree = ValueTree(Identifier("nodeParams"));
            addSliders(listener, LandF);
        }

        void paint(Graphics& g) override
        {
            g.fillAll(Colours::darkslategrey);

            waveform.drawChannels(g,                                      // [9]
                Rectangle<int>(100, 100),
                0.0,                                    // start time
                waveform.getTotalLength(),             // end time
                1.0f);

        }

        void resized() override
        {
            Grid grid;

            using Track = Grid::TrackInfo;
            grid.templateRows = { Track(1_fr)};
            grid.templateColumns = { Track(1_fr), Track(3_fr), 
                                     Track(1_fr), Track(3_fr), 
                                     Track(1_fr), Track(3_fr),
                                     Track(1_fr), Track(3_fr),
                                     Track(1_fr), Track(3_fr) };
            for (auto* s : sliders) {
                GridItem g1 = GridItem(s);
                //g1.withAlignSelf(GridItem::AlignSelf::stretch);
                //g1.minWidth = 130.0f;
                //g1.minHeight = 150.0f;
                grid.items.add(GridItem());
                grid.items.add(g1);
                grid.performLayout(getLocalBounds());
            }
        }

        void addSliders(Slider::Listener* listener, LookAndFeel_V4* LandF) {
            addChildComponent(sliders.add(new Slider("grainLength")));
            sliders.getLast()->setNormalisableRange(NormalisableRange<double>(5.0f, 5000.0f, 0.1f, 0.25f));
            addChildComponent(labels.add(new Label()));
            Label* l = labels.getLast();
            l->setText("Grain Size", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("startTime")));
            sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1000.0f, 0.01f, 1.0f));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Position", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("frequency")));
            sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.03125f, 16.0f, 0.01f, 1.0f));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Freq", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("pan")));
            sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Pan", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            addChildComponent(sliders.add(new Slider("volume")));
            sliders.getLast()->setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
            addChildComponent(labels.add(new Label()));
            l = labels.getLast();
            l->setText("Volume", dontSendNotification);
            l->attachToComponent(sliders.getLast(), true);

            for (auto* slider : sliders) {
                slider->setLookAndFeel(LandF);
                slider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxAbove, false, 40, 40);
                slider->setNumDecimalPlacesToDisplay(1);
                slider->setSliderStyle(Slider::Rotary);
                slider->addListener(listener);
            }
        }

        /*
        void addSlider(String name, Slider::Listener* listener, double rangeStart, double rangeEnd, double skewFactor) {
            addChildComponent(sliders.add(new Slider(name)));
            Slider* slider = sliders.getLast();
            slider->setSliderStyle(Slider::Rotary);
            slider->setNormalisableRange(NormalisableRange<double>(rangeStart, rangeEnd, 0.01f, skewFactor));
            slider->addListener(listener);
        }

        void addFrequencySlider(Slider::Listener* listener) {
            addChildComponent(sliders.add(new Slider("frequency")));
            Slider* frequencySlider = sliders.getLast();
            frequencySlider->setNormalisableRange(NormalisableRange<double>(-1.5f, 1.0f, 0.25f));
            frequencySlider->onValueChange = [this] {
                DBG("Yah");
            };
            frequencySlider->addListener(listener);
        }*/

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
            for (auto& slider : sliders) {
                slider->setValue(paramTree.getChildWithName(componentId)[slider->getName()]);
                slider->setVisible(true);
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
        ValueTree paramTree;
        OwnedArray<Slider> sliders;
        OwnedArray<Label> labels;
        bool displayingControls{ false };
        bool displayingWaveform{ false };
    };

    
    CPGLookAndFeel LandF;
    nodeContainer nodePanel;
    controlsContainer controlsPanel;
    TextButton addNodeButton;
    Slider mainFreqSlider;
    std::unique_ptr<FilenameComponent> fileComp;
    Label addButtonLabel, mFreqLabel, fileLabel;
    OSCParamSetter setter{ 8000 };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
