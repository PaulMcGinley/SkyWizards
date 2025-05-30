//
// Created by Paul McGinley on 30/05/2025.
//

#include "Leaderboard.h"

#include <fstream>
#include <sstream>

#include "os/GetExecutionDirectory.h"

void Leaderboard::AddScore(const Score &newScore) {
        scores.push_back(newScore);
        SortScores();
        Save();
}
void Leaderboard::SortScores() {
        std::sort(scores.begin(), scores.end(), [](const Score& a, const Score& b) {
                return a.score > b.score; // Sort score in descending order (highest first)
        });
}
const std::vector<Score> &Leaderboard::GetScores() { return scores; }
const std::vector<Score> &Leaderboard::GetTopNScores(size_t n) {
        if (n > scores.size()) {
                n = scores.size();
        }
        return std::vector<Score>(scores.begin(), scores.begin() + n);
}
void Leaderboard::Load() {
        std::string filePath = getExecutableDirectory() +  "/leaderboard.txt";
        std::ifstream file(filePath);

        if (!file.is_open()) {
                return;
        }

        scores.clear(); // Clear existing scores before loading

        std::string line;
        while (std::getline(file, line)) {
                std::stringstream stringStream(line);
                std::string name;
                std::string scoreStr;
                std::string date;

                if (std::getline(stringStream, name, ',') &&
                    std::getline(stringStream, scoreStr, ',') &&
                    std::getline(stringStream, date)) {
                        try {
                                int scoreVal = std::stoi(scoreStr);
                                scores.push_back({name, scoreVal, date});
                        } catch (const std::invalid_argument& ia) {
                                std::cerr << "Invalid argument: " << ia.what() << " for score string: " << scoreStr << std::endl;
                        } catch (const std::out_of_range& oor) {
                                std::cerr << "Out of Range: " << oor.what() << " for score string: " << scoreStr << std::endl;
                        }
                    } else {
                            std::cerr << "Warning: Malformed line in leaderboard file: " << line << std::endl;
                    }
        }

        file.close();
        SortScores(); // Sort scores after loading them
}
void Leaderboard::Save() const {
        std::string filePath = getExecutableDirectory() +  "/leaderboard.txt";
        std::ofstream file(filePath);

        if (!file.is_open()) {
                std::cerr << "Error: Leaderboard file is open by another process: " << filePath << std::endl;
                return;
        }

        for (const auto& score : scores) {
                file << score.name << "," << score.score << "," << score.date << std::endl;
        }

        file.close();
}
