//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef ANITYPE_H
#define ANITYPE_H

enum class AniType {
    Consume = 0,
    Damaged = 1,
    Death = 2,
    Dizzy = 3,
    Fire = 4,
    Idle = 5,
    Idle2 = 6,
    Interact = 7,
    JumpAir = 8,
    JumpEnd = 9,
    JumpStart = 10,
    JumpUp = 11,
    Pickup = 12,
    Run = 13,
    Walk = 14,
    Taunt = 15,
    BattleIdle = 16,
    SenseSomething = 17,
    Victory = 18,

    Attack = 50,
    Attack2 = 51,

    Static = 255
};

#endif //ANITYPE_H
