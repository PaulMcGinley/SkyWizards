//
// Created by Paul McGinley on 23/05/2025.
//

#ifndef SCORE_H
#define SCORE_H

#include <string>



struct Score {
        Score();
        Score(std::string levelName, int playerScore, int totalTimeMs, float percentComplete);
        Score(std::string levelName, int playerScore, int mins, int secs, int ms, float percentComplete);

        // Format time as a string (MM:SS.mmm)
        [[nodiscard]] std::string GetTimeString(int decimals) const;

        [[nodiscard]] int GetMinutes() const;
        [[nodiscard]] int GetSeconds() const;
        [[nodiscard]] int GetMilliseconds() const;
        [[nodiscard]] int GetTotalMilliseconds() const;

        void AddToScore(int score, int timeInMilliseconds);


private:
        std::string levelName;
        int score;
        float percentComplete;
        int totalTimeInMilliseconds;
};

#endif //SCORE_H
