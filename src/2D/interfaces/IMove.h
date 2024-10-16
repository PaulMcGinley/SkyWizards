//
// Created by Paul McGinley on 05/10/2024.
//

#ifndef IMOVE_H
#define IMOVE_H

class IMove {
    float walkSpeed = 0;
    float runSpeed = 0;
    float jumpHeight = 0;
    float gravity = 0;
    float terminalVelocity = 0;
    float airControl = 0;
    float friction = 0;
    float acceleration = 0;
    float deceleration = 0;
    float maxSpeed = 0;
    float slopeLimit = 0;
    float slopeSlide = 0;
    float slopeSlideSpeed = 0;
};

#endif //IMOVE_H
