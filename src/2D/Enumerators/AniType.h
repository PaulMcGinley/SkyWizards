//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef ANITYPE_H
#define ANITYPE_H

enum class AnimationType {
        ANIMATION_CONSUME = 0,
        ANIMATION_DAMAGED = 1,
        ANIMATION_DEATH = 2,
        ANIMATION_DIZZY = 3,
        ANIMATION_FIRE = 4,
        ANIMATION_IDLE = 5,
        ANIMATION_IDLE2 = 6,
        ANIMATION_INTERACT = 7,
        ANIMATION_JUMP_AIR = 8,
        ANIMATION_JUMP_END = 9,
        ANIMATION_JUMP_START = 10,
        ANIMATION_JUMP_UP = 11,
        ANIMATION_PICKUP = 12,
        ANIMATION_RUN = 13,
        ANIMATION_WALK = 14,
        ANIMATION_TAUNT = 15,
        ANIMATION_BATTLE_IDLE = 16,
        ANIMATION_SENSE_SOMETHING = 17,
        ANIMATION_VICTORY = 18,
        ANIMATION_Dead = 19,
        ANIMATION_ATTACK = 50,
        ANIMATION_ATTACK2 = 51,
        ANIMATION_STATIC = 255
};

#endif //ANITYPE_H
