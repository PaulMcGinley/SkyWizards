//
// Created by Paul McGinley on 23/05/2025.
//

#include "Score.h"
#include <sstream>
#include <iomanip>
#include <utility>

Score::Score()
        : levelName("Player")
        , score(0)
        , totalTimeInMilliseconds(0) { /* Nothing in the constructor */ }
Score::Score(std::string levelName, int playerScore, int totalTimeMs, float scorePercentage)
        : levelName(std::move(levelName))
        , score(playerScore)
        , percentComplete(scorePercentage)
        , totalTimeInMilliseconds(totalTimeMs) { /* Nothing in the constructor */ }
Score::Score(std::string levelName, int playerScore, int mins, int secs, int ms, float scorePercentage)
        : levelName(std::move(levelName))
        , score(playerScore)
        , percentComplete(scorePercentage) {
        totalTimeInMilliseconds = (mins * 60 * 1000) + (secs * 1000) + ms;
}
std::string Score::GetTimeString(int decimals = 8) const {
        const int minutes = GetMinutes();
        const int seconds = GetSeconds();
        const int milliseconds = GetMilliseconds();

        std::stringstream stringStream;
        stringStream << std::setfill('0') << std::setw(2) << minutes << ":"
                     << std::setfill('0') << std::setw(2) << seconds << "."
                     << std::setfill('0') << std::setw(decimals) << milliseconds;
        return stringStream.str();
}
int Score::GetMinutes() const { return totalTimeInMilliseconds / (60 * 1000); }
int Score::GetSeconds() const { return (totalTimeInMilliseconds % (60 * 1000)) / 1000; }
int Score::GetMilliseconds() const { return totalTimeInMilliseconds % 1000; }
int Score::GetTotalMilliseconds() const { return totalTimeInMilliseconds; }
void Score::AddToScore(int score, int timeInMilliseconds) {
        this->score += score;
        totalTimeInMilliseconds += timeInMilliseconds;
}
