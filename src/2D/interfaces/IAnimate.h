//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IANIMATE_H
#define IANIMATE_H

#include <map>
#include "Enumerators/AniType.h"
#include "models/AniSequence.h"
#include "models/GameTime.h"

class IAnimate {
public:
        virtual ~IAnimate() = default;

        virtual void TickAnimation(GameTime gameTime);
        bool ChangeAnimation(AnimationType nextAnimation, GameTime gameTime, bool forceNewAnimation = false);
        bool ChangeAnimation(AnimationType next_animation, bool forceNewAnimation = true);

        std::map<AnimationType, AnimationSequence> GetAnimationSequences() const;
        AnimationSequence GetCurrentAnimationSequence() const;
        AnimationType GetCurrentAnimation() const;
        int GetCurrentAnimationFrame() const;
        int GetCurrentAnimationFrameCount() const;
        float GetNextFrameTime() const;
        float GetNextAnimationTime() const;
        int GetTextureDrawIndex() const;
        int GetAnimationDuration() const;

        void SetAnimationSequences(const std::map<AnimationType, AnimationSequence>& animationSequences);
        void SetAnimationFrame(int frame);

private:
        // Map of animation sequences, populated by the entity
        std::map<AnimationType, AnimationSequence> sequences;

        // Current animation sequence of the entity
        AnimationType currentAnimation = AnimationType::ANIMATION_IDLE;

        // Current frame of the animation sequence
        int currentAnimationFrame = 0;

        // Time when the next frame of the animation will be played
        float nextFrameTime = 0;

        // Time when the next animation will be played
        float nextAnimationTime = 0;
};

#endif //IANIMATE_H
