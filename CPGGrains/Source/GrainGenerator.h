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
    void setSampleRateAndBufferSize(int SR, int bufferSize) {
        sampleRate = SR;
        numSamplesInBuffer = bufferSize;
    }

    void setStatePointers(float* lengthPtr, float* startPtr, float* panPtr, float* volPtr) {
        length = lengthPtr;
        start = startPtr;
        pan = panPtr;
        volume = volPtr;
    }

    void checkTrigger(int nodeNo, double sampleValue) {
        if ((sampleValue > 0 && previousSample < 0) && grainList.size() < maxGrains) {
            grainList.push_back(new Grain(getStartAndEndSample(), *pan, *volume));
        }
        previousSample = sampleValue;
    }


    void synthesizeGrains(const float* sampleBuffer, float* LPointer, float* RPointer, int currentIndex) {
        std::list<Grain*>::iterator it = grainList.begin();
        while (it != grainList.end())
        {
            int grainSample = (*it)->getSampleIndex();
            if (grainSample == -1)
            {
                it = grainList.erase(it);
            }
            else
            {
                processGrain(sampleBuffer[(*it)->getSampleIndex()], LPointer, RPointer, currentIndex);
                (*it)->step();
                ++it;
            }
        }
    }
private:
    void processGrain(float sampleValue, float* LPointer, float* RPointer, int i) {
        float leftOut = sampleValue * std::fmin(1.0 - (*pan), 1.0);
        float rightOut = sampleValue * std::fmin(1.0 + (*pan), 1.0);
        LPointer[i] += leftOut * (*volume);
        RPointer[i] += rightOut * (*volume);
    }
    std::tuple<int, int> getStartAndEndSample()
    {
        int startSample = *start / 1000 * sampleRate;
        int endSample = startSample + (*length / 1000 * sampleRate);
        endSample = std::min(endSample, numSamplesInBuffer);
        return std::make_tuple<>(startSample, endSample);
    }
    std::list<Grain*> grainList;
    unsigned maxGrains = 50;
    double previousSample = 1.0;
    float *length, *start, *pan, *volume;
    int sampleRate, numSamplesInBuffer;
};
