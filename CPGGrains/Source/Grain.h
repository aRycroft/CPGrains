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
    Grain(std::tuple<int, int> startAndEnd){ 
        auto indices = startAndEnd;
        currentIndex = std::get<0>(indices);
        maxIndex = std::get<1>(indices);
    }

    int nextSampleIndex() {
        if (currentIndex > maxIndex) {
            return -1;
        }
        else {
            return currentIndex++;
        }
    }
private:
    int currentIndex, maxIndex;
};
