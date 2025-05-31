//
// Created by Paul McGinley on 30/05/2025.
//

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "Score.h"

#include <vector>

class Leaderboard {
public:
        Leaderboard() = default;

        void AddScore(const Score& newScore);
        void SortScores();
        const std::vector<Score> & GetScores();
        const std::vector<Score> GetTopNScores(size_t n);

        void Load();
        void Save() const;

private:
        std::vector<Score> scores;
};

#endif //LEADERBOARD_H
