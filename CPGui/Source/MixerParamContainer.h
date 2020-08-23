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
        grid.templateColumns = { Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr), Track(1_fr) };
        auto children = this->getChildren();
        grid.items = {  
            GridItem{children[1]}.withArea(1, 2),
            GridItem{children[3]}.withArea(1, 4),
            GridItem{children[5]}.withArea(1, 6),
            GridItem{children[10]}.withArea(2, 1),
            GridItem{children[11]}.withArea(2, 2),
            GridItem{children[7]}.withArea(2, 4),
            GridItem{children[9]}.withArea(2, 6),

        };
        grid.performLayout(getLocalBounds());
    }
private:
};