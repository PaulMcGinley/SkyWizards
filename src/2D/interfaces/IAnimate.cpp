//
// Created by Paul McGinley on 30/09/2024.
//

#include "IAnimate.h"

#include "models/TextureEntry.h"

// Move to the next frame in the animation sequence
// If we reach the end of the sequence, loop back to the start
void IAnimate::TickAnimation(const GameTime gameTime) {

    if (!gameTime.TimeElapsed(nextFrameTime))
        return;

    // Increment the current frame
    currentFrame++;

    // Loop back to the start of the sequence if we reach the end
    if (currentFrame >= sequences[currentAni].Length)
        currentFrame = 0;

    nextFrameTime = gameTime.NowAddMilliseconds(sequences[currentAni].Interval);
}

// Change the current animation sequence
bool IAnimate::ChangeAni(const AniType nextAni, const GameTime gameTime, const bool force) {

    if (!gameTime.TimeElapsed(nextAniTime) && !force)
        return false;

    if(currentAni == nextAni)
        return false;

    // Change the current animation sequence
    currentAni = nextAni;
    currentFrame = 0;
    nextFrameTime = 0;

    float newNextAniTime = gameTime.NowAddMilliseconds(sequences[currentAni].Interval * sequences[currentAni].Length);

    if (nextAni == AniType::Attack) {
        nextAniTime = newNextAniTime;
    }

    return true;
}