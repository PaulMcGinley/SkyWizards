//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef ANISEQUENCE_H
#define ANISEQUENCE_H

#include <functional>

struct AnimationSequence {

        // First frame index within the library for the animation sequence
        int startFrame;

        // The number of frames in the sequence
        int length;

        // Time between frame ticks (in ms)
        int interval;

        // Function to call when the animation sequence begins
        std::function<void()> Sequence_OnBegin;

        // Function to call when the animation sequence ends
        std::function<void()> Sequence_OnComplete;

        // Function to call on each frame of the animation sequence
        std::function<void()> Sequence_OnFrameTick;
};

#endif //ANISEQUENCE_H
