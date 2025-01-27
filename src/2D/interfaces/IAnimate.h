//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef IANIMATE_H
#define IANIMATE_H

#include <map>
#include "enums/AniType.h"
#include "models/AniSequence.h"
#include "models/GameTime.h"

class IAnimate {
public:
        virtual ~IAnimate() = default;

        // Map of animation sequences, populated by the entity
        std::map<AnimationType, AnimationSequence> sequences;

        // Current animation sequence of the entity
        AnimationType current_animation = AnimationType::ANIMATION_IDLE;

        // Current frame of the animation sequence
        int current_animation_frame = 0;

        // Time until the next frame of the animation
        float next_frame_time = 0;
        // Time until the next animation can be played
        float next_animation_time = 0;

        virtual void tickAnimation(GameTime gameTime);
        bool changeAnimation(AnimationType next_animation, GameTime game_time, bool force_new_animation = false);
};

#endif //IANIMATE_H
