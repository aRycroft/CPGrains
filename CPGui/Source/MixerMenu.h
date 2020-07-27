/*
  ==============================================================================

    MixerMenu.h
    Created: 27 Jul 2020 11:46:34am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class MixerMenu :public Component {
public:
    MixerMenu(ValueTree mixerParams)
        : mixerParams(mixerParams)
    {
        for (int i = 0; i < mixerParams.getNumChildren(); i++) {
            Slider* vol = sliders.add(new Slider{});
            vol->setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
            vol->onValueChange = [this, i, mixerParams, vol] {
                mixerParams.getChild(i).setProperty(volume, vol->getValue(), nullptr);
            };
            vol->setValue(mixerParams.getChild(i).getProperty(volume));
            vol->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
            vol->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
            Slider* panSlider = sliders.add(new Slider{});
            panSlider->setNormalisableRange(NormalisableRange<double>(0.0f, 1.0f, 0.001f, 1.0f));
            panSlider->setValue(mixerParams.getChild(i).getProperty(pan));
            panSlider->onValueChange = [this, i, mixerParams, panSlider] {
                mixerParams.getChild(i).setProperty(pan, panSlider->getValue(), nullptr);
            };
            panSlider->setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
            panSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 0, 0);
            addAndMakeVisible(vol);
            addAndMakeVisible(panSlider);
        }
    }

    void paint(Graphics& g) override
    {
        g.fillAll(Colours::slateblue);
        int numChildren = mixerParams.getNumChildren();
        for (int i = 0; i < numChildren; i++) {
            g.setColour(juce::Colour::fromString(mixerParams.getChild(i).getProperty(colour).toString()));
            g.fillRect(Rectangle<int>{i *  getWidth() / numChildren, 0, getWidth() / numChildren, getHeight()});
        }
    }

    void resized() override
    {
        Grid grid;

        using Track = Grid::TrackInfo;

        Array<Track> tracks;
        Array<GridItem> items;
        for (auto* s : sliders) {
            tracks.add(Track(1_fr));
            items.add(GridItem{ s });
        }
        grid.templateRows = { Track(1_fr) };
        grid.templateColumns = tracks;
        grid.items = items;
        grid.performLayout(getLocalBounds());
    }
private:
    OwnedArray<Slider> sliders;
    OwnedArray<Rectangle<int>> outlineRects;
    ValueTree mixerParams, nodeParams;
    Identifier pan{ "pan" };
    Identifier volume{ "volume" };
    Identifier colour{ "colour" };
};