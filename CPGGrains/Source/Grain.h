/*
  ==============================================================================

    Grain.h
    Created: 18 Jun 2020 12:42:53pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

class Grain {
public:
    Grain(std::tuple<int, int> startAndEnd, float pan, float volume) {
        currentIndex = std::get<0>(startAndEnd);
        maxIndex = std::get<1>(startAndEnd);
        this->pan = pan;
        this->volume = volume;
    }

    void setStartAndEnd(std::tuple<int, int> startAndEnd) {
        currentIndex = std::get<0>(startAndEnd);
        maxIndex = std::get<1>(startAndEnd);
    }

    int getSampleIndex() {
        if (currentIndex > maxIndex) {
            return -1;
        }
        else {
            return currentIndex;
        }
    }

    void step() {
        currentIndex++;
    }

private:
    int currentIndex, maxIndex;
    float pan, volume;
    bool isPlaying;
};
