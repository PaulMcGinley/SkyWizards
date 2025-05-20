//
// Created by Paul McGinley on 30/09/2024.
//

#include "IAnimate.h"

#include "models/TextureEntry.h"

// Move to the next frame in the animation sequence
// If we reach the end of the sequence, loop back to the start
void IAnimate::TickAnimation(const GameTime gameTime) {
        // Check if the animation is ready for the next frame
        if (!gameTime.TimeElapsed(nextFrameTime))
                return;

        // Increment the current frame
        currentAnimationFrame++;

        // Check if we have reached the end of the sequence
        if (currentAnimationFrame >= sequences[currentAnimation].length) {
                // Loop back to the start
                currentAnimationFrame = 0;

                // Call the onComplete function if it exists
                if (sequences[currentAnimation].Sequence_OnComplete != nullptr) {
                        sequences[currentAnimation].Sequence_OnComplete();
                }
        }

        // Update the next frame time
        nextFrameTime = gameTime.NowAddMilliseconds(sequences[currentAnimation].interval);
}
// Change the current animation sequence
bool IAnimate::ChangeAnimation(const AnimationType nextAnimation, const GameTime gameTime, const bool forceNewAnimation) {
        // Check if the next animation should be played
        if (!gameTime.TimeElapsed(nextAnimationTime) && !forceNewAnimation)
                return false;

        // Check if the next animation is the same as the current one
        if (currentAnimation == nextAnimation)
                return false;

        // Change the current animation sequence
        currentAnimation = nextAnimation;
        // Reset the current frame
        currentAnimationFrame = 0;
        // Reset the next frame time
        nextFrameTime = 0;

        // Update the next animation time
        const float newNextAnimationTime = gameTime.NowAddMilliseconds(GetAnimationDuration());

        // Update the next animation time
        nextAnimationTime = newNextAnimationTime;

        return true;
}
// Change the current animation sequence
bool IAnimate::ChangeAnimation(const AnimationType nextAnimation, bool forceNewAnimation) {
        // Check if the next animation should be played
        if (!forceNewAnimation)
                return false;

        // Check if the next animation is the same as the current one
        if (currentAnimation == nextAnimation)
                return false;

        // Change the current animation sequence
        currentAnimation = nextAnimation;
        currentAnimationFrame = 0;
        nextFrameTime = 0;

        return true;
}
std::map<AnimationType, AnimationSequence> IAnimate::GetAnimationSequences() const { return sequences; }
AnimationSequence IAnimate::GetCurrentAnimationSequence() const { return sequences.at(currentAnimation); }
AnimationType IAnimate::GetCurrentAnimation() const { return currentAnimation; }
int IAnimate::GetCurrentAnimationFrame() const { return currentAnimationFrame; }
int IAnimate::GetCurrentAnimationFrameCount() const { return sequences.at(currentAnimation).length; }
float IAnimate::GetNextFrameTime() const { return nextFrameTime; }
float IAnimate::GetNextAnimationTime() const { return nextAnimationTime; }
int IAnimate::GetTextureDrawIndex() const { return sequences.at(currentAnimation).startFrame + currentAnimationFrame; }
int IAnimate::GetAnimationDuration() const { return GetCurrentAnimationSequence().interval * GetCurrentAnimationSequence().length; }
void IAnimate::SetAnimationSequences(const std::map<AnimationType, AnimationSequence> &animationSequences) { sequences = animationSequences; }
void IAnimate::SetAnimationFrame(int frame) { currentAnimationFrame = frame; }
