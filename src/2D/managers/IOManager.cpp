//
// Created by Paul McGinley on 26/01/2025.
//

#include "IOManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string IOManager::ReadFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
                throw std::runtime_error("Could not open file for reading: " + filePath);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
}

std::string IOManager::WriteFile(const std::string& filePath, const std::string& content) {
        std::ofstream file(filePath);
        if (!file.is_open()) {
                throw std::runtime_error("Could not open file for writing: " + filePath);
        }

        file << content;
        file.close();
        return "File written successfully to: " + filePath;
}