/*
  ==============================================================================

    MixerParamContainer.h
    Created: 6 Aug 2020 5:47:52pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MixerParamContainer : public Component 
{
public:
    void paint(Graphics& g) override 
    {
        g.fillAll(this->findColour(0));
    }
    void resized() override 
    {
        Grid grid;

        using Track = Grid::TrackInfo;
        grid.templateRows = { Track(2_fr), Track(1_fr) };
        grid.templateColumns = { Track(1_fr), Track(2_fr), Track(1_fr), Track(2_fr), Track(1_fr), Track(2_fr) };
        auto children = this->getChildren();
        grid.items = {  GridItem{children[0]}, GridItem{children[1]}, GridItem{children[2]}, GridItem{children[3]}, GridItem{children[4]}, GridItem{children[5]},
                        GridItem{}, GridItem{}, GridItem{children[6]}, GridItem{children[7]}, GridItem{children[8]}, GridItem{children[9]}
        };
        grid.performLayout(getLocalBounds());

    }
private:
};