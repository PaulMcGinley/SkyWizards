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

        virtual void TickAnimation(GameTime gameTime);
        bool ChangeAnimation(AnimationType next_animation, GameTime game_time, bool force_new_animation = false);
};

#endif //IANIMATE_H
