//
// Created by Paul McGinley on 30/09/2024.
//

#include "IAnimate.h"

#include "models/TextureEntry.h"

// Move to the next frame in the animation sequence
// If we reach the end of the sequence, loop back to the start
void IAnimate::TickAnimation(const GameTime gameTime) {

        if (!gameTime.TimeElapsed(next_frame_time))
                return;

        // Increment the current frame
        current_animation_frame++;

        // Loop back to the start of the sequence if we reach the end
        if (current_animation_frame >= sequences[current_animation].Length)
                current_animation_frame = 0;

        next_frame_time = gameTime.NowAddMilliseconds(sequences[current_animation].Interval);
}

// Change the current animation sequence
bool IAnimate::ChangeAnimation(const AnimationType next_animation, const GameTime game_time, const bool force_new_animation) {

        if (!game_time.TimeElapsed(next_animation_time) && !force_new_animation)
                return false;

        if(current_animation == next_animation)
                return false;

        // Change the current animation sequence
        current_animation = next_animation;
        current_animation_frame = 0;
        next_frame_time = 0;

        float newNextAniTime = game_time.NowAddMilliseconds(sequences[current_animation].Interval * sequences[current_animation].Length);

        if (next_animation == AnimationType::ANIMATION_ATTACK) {
                next_animation_time = newNextAniTime;
        }

        return true;
}
