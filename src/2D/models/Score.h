//
// Created by Paul McGinley on 23/05/2025.
//

#ifndef SCORE_H
#define SCORE_H

#include <string>

struct Score {
        Score();
        Score(std::string playerName, int playerScore, int totalTimeMs);
        Score(std::string playerName, int playerScore, int mins, int secs, int ms);

        // Format time as a string (MM:SS.mmm)
        [[nodiscard]] std::string GetTimeString(int decimals) const;

        [[nodiscard]] int GetMinutes() const;
        [[nodiscard]] int GetSeconds() const;
        [[nodiscard]] int GetMilliseconds() const;
        [[nodiscard]] int GetTotalMilliseconds() const;

        void AddToScore(int score, int timeInMilliseconds);

        std::string name;
        int score;
private:
        int totalTimeInMilliseconds;
};

#endif //SCORE_H
