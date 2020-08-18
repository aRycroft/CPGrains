/*
  ==============================================================================

    Identifiers.h
    Created: 5 Aug 2020 1:41:19pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
/*enum ColourId {
    node1 = 5000,
    node2 = 5001,
    node3 = 5002,
    node4 = 5003
} ColourId;*/
namespace Ids
{
    #define MAKE_ID(name) const Identifier name (#name)
    /*Shared*/
    MAKE_ID(active);
    MAKE_ID(colour);
    /*Node*/
    MAKE_ID(grainLength);
    MAKE_ID(startTime);
    MAKE_ID(frequency);
    MAKE_ID(x);
    MAKE_ID(y);
    /*Connection*/
    MAKE_ID(weight);
    MAKE_ID(weightDir);
    MAKE_ID(lengthMod);
    MAKE_ID(lengthModDir);
    MAKE_ID(startMod);
    MAKE_ID(startModDir);
    MAKE_ID(from);
    MAKE_ID(to);
    /*Mixer*/
    MAKE_ID(pan);
    MAKE_ID(volume);
}

/*enum ColourId {
    node1 = 5000,
    node2 = 5001,
    node3 = 5002,
    node4 = 5003
} ColourIDS;*/