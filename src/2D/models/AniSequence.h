//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef ANISEQUENCE_H
#define ANISEQUENCE_H

struct AnimationSequence{

        // First frame index within the library for the animation sequence
        int startFrame;

        // The number of frames in the sequence
        int length;

        // Time between frame ticks (in ms)
        int interval;

        // Function to call when the animation sequence begins
        std::function<void()> onBegin;

        // Function to call when the animation sequence ends
        std::function<void()> onComplete;

        // Function to call on each frame of the animation sequence
        std::function<void(int)> onFrame;
};

#endif //ANISEQUENCE_H
