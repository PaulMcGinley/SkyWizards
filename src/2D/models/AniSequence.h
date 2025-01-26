//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef ANISEQUENCE_H
#define ANISEQUENCE_H

struct AniSequence{
    int StartFrame; // first frame of the sequence in the library
    int Length;     // number of frames in the sequence
    int Interval;   // time between frames (in ms)
    std::function<void()> OnBegin; // function to call when the sequence starts
    std::function<void()> OnComplete; // function to call when the sequence completes
    std::function<void(int)> OnFrame; // function to call on each frame
};

#endif //ANISEQUENCE_H
