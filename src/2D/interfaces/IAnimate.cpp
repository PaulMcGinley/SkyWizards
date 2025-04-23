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
        currentAnimationFrame++;

        // Loop back to the start of the sequence if we reach the end
        if (currentAnimationFrame >= sequences[currentAnimation].length)
                currentAnimationFrame = 0;

        nextFrameTime = gameTime.NowAddMilliseconds(sequences[currentAnimation].interval);
}

// Change the current animation sequence
bool IAnimate::ChangeAnimation(const AnimationType next_animation, const GameTime game_time, const bool force_new_animation) {

        if (!game_time.TimeElapsed(nextAnimationTime) && !force_new_animation)
                return false;

        if(currentAnimation == next_animation)
                return false;

        // Change the current animation sequence
        currentAnimation = next_animation;
        currentAnimationFrame = 0;
        nextFrameTime = 0;

        const float new_next_animation_time = game_time.NowAddMilliseconds(sequences[currentAnimation].interval * sequences[currentAnimation].length);

        // if (next_animation == AnimationType::ANIMATION_ATTACK) {
        //         next_animation_time = new_next_animation_time;
        // }

        return true;
}
