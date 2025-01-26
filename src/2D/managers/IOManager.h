//
// Created by Paul McGinley on 26/01/2025.
//

#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <string>

class IOManager {
public:
        static std::string ReadFile(const std::string& filePath);
        static std::string WriteFile(const std::string& filePath, const std::string& content);
};

#endif //IOMANAGER_H
