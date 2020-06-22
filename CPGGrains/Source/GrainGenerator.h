/*
  ==============================================================================

    GrainGenerator.h
    Created: 18 Jun 2020 4:38:59pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include <queue>
#include <list>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CPGLib/MatsuokaEngine.h"
#include "TriggerDetector.h"

class GrainGenerator {
public:
    void checkTrigger(int nodeNo, double sampleValue) {
        if ((sampleValue > 0 && previousSample < 0)) {
            toBeGenerated.push(nodeNo);
        }
        previousSample = sampleValue;

        /*int result;
        if ((sampleValue > 0 && previousSample < 0)) {
            toBeGenerated.push(nodeNo);
        }
        previousSample = sampleValue;
        return;
        //if (tick++ % 1000 == 0) toBeGenerated.push(0);        
        
        if (sampleValue >= 0.46739999999999999) {
            toBeGenerated.push(nodeNo);
        }
        
        if (detectors[nodeNo].checkTrigger(sampleValue)) {
            toBeGenerated.push(nodeNo);
        }*/
    }

    int getGrain() {
        if (toBeGenerated.empty()) {
            return -1;
        }
        int ret = toBeGenerated.front();
        toBeGenerated.pop();
        return ret;
    }

private:
    std::queue<int, std::list<int>> toBeGenerated;
    TriggerDetector detector;
    double previousSample = 1.0;
};
