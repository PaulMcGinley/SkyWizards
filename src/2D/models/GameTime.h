//
// Created by Paul McGinley on 30/09/2024.
//

#ifndef GAMETIME_H
#define GAMETIME_H

struct GameTime {
        float deltaTime = 0.0f;            // seconds
        float totalGameTime = 0.0f;       // seconds

        // Used to check if a certain amount of time has passed
        // if (TimeElapsed(NextAnimationTime)) ...
        [[nodiscard]] bool TimeElapsed(const float time) const {
                return totalGameTime > time;
        }

        // Used to check if a certain amount of time has passed in milliseconds
        // int nextUpdate = NowAddMilliseconds(1000);
        [[nodiscard]] float NowAddMilliseconds(const float ms) const {

                // Convert the total game time to milliseconds from seconds
                float newTime = totalGameTime * 1000;

                // Add the delta time to the new time (in milliseconds)
                newTime += ms;

                // Divide the new time by 1000 to convert it back to seconds
                newTime /= 1000;

                // Return the new time in seconds
                return newTime;
        }

        // Overload the + operator to add the delta time to the total game time
        // gt = gameTime object
        // dt = delta time in milliseconds
        friend GameTime operator+(const GameTime& gt, const float dt) {

                // Create a new GameTime object to work with
                GameTime result = gt;

                // Set the delta time and total game time
                result.deltaTime = dt;

                // Add the delta time to the total game time
                result.totalGameTime += dt;

                // Return the new GameTime object
                return result;
        }

        // Overload the += operator to add the delta time to the total game time
        // dt = delta time in milliseconds
        GameTime& operator+=(const float dt) {

                // Set the delta time to the new value
                this->deltaTime = dt;

                // Add the delta time to the total game time
                this->totalGameTime += dt;

                // Return the current GameTime object
                return *this;
        }
};

#endif //GAMETIME_H
