//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef GAMETIME_H
#define GAMETIME_H

struct GameTime {
    float deltaTime = 0.0f; // seconds
    float totalTime = 0.0f; // seconds

    // Used to check if a certain amount of time has passed
    // if (TimeElapsed(NextAnimationTime)) ...
    [[nodiscard]] bool TimeElapsed(const float time) const {
        return totalTime > time;
    }

    // Used to check if a certain amount of time has passed in milliseconds
    // int nextUpdate = NowAddMilliseconds(1000);
    [[nodiscard]] float NowAddMilliseconds (const float ms) const {
        float newTime = totalTime * 1000;
        newTime += ms;
        newTime /= 1000;
        return newTime;
    }

    // Overload the + operator
    friend GameTime operator+(const GameTime& gameTime, const float dt) {
        GameTime result = gameTime;
        result.deltaTime = dt;
        result.totalTime += dt;
        return result;
    }

    // Overload the += operator
    GameTime& operator+=(const float dt) {
        this->deltaTime = dt;
        this->totalTime += dt;
        return *this;
    }
};

#endif //GAMETIME_H
