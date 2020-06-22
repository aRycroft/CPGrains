/*
  ==============================================================================

    TriggerDetector.h
    Created: 19 Jun 2020 2:45:24pm
    Author:  Alex

  ==============================================================================
*/

#pragma once
class TriggerDetector {
public:
    TriggerDetector() {
        previousSample = 1.0;
    }
    bool checkTrigger(double val) {
        
    };
    double previousSample;
};
