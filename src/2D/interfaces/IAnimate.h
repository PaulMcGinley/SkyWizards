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
    std::map<AniType, AniSequence> sequences;

    // Current animation sequence of the entity
    AniType currentAni = AniType::Idle;

    // Current frame of the animation sequence
    int currentFrame = 0;

    // Time until the next frame of the animation
    float nextFrameTime = 0;
    float nextAniTime = 0; // Time until the next animation can be played

    virtual void TickAnimation(GameTime gameTime);
    bool ChangeAni(AniType nextAni, GameTime GAME_TIME, bool force = false);
};

#endif //IANIMATE_H
