//
// Created by Paul McGinley on 26/01/2025.
//

#include "GameSettingsFile.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "os/GetExecutionDirectory.h"

GameSettingsFile::GameSettingsFile()
        : resolutionWidth(1920)
        , resolutionHeight(1080)
        , fullscreenMode(false)
        , vsyncEnabled(true) { /* Nothing in the constructor */ }

void GameSettingsFile::LoadSettings() {
        std::ifstream settingsFile(getExecutableDirectory() + "/" + SETTINGS_FILE_PATH);
        if (!settingsFile.is_open()) {
                std::cerr << "Warning: Could not open settings file: " << SETTINGS_FILE_PATH << std::endl;
                std::cout << "Using default settings: " << std::endl;
                SaveSettings(); // Create a new settings file with default values
                return;
        }

        std::string line;
        std::string WHITE_SPACE = " \t\n\r\f\v"; // Whitespace characters
        while (std::getline(settingsFile, line)) {
                std::stringstream stringStream(line);
                std::string key;
                std::string value;

                if (std::getline(stringStream, key, '=') && std::getline(stringStream, value)) {
                        key.erase(0, key.find_first_not_of(WHITE_SPACE));       // Remove leading whitespace
                        key.erase(key.find_last_not_of(WHITE_SPACE) + 1);       // Remove trailing whitespace
                        value.erase(0, value.find_first_not_of(WHITE_SPACE));   // Remove leading whitespace
                        value.erase(value.find_last_not_of(WHITE_SPACE) + 1);   // Remove trailing whitespace

                        if (key == "ResolutionWidth") {
                                try {
                                        resolutionWidth = std::stoi(value); // Convert value to int
                                } catch (const std::invalid_argument& _) {
                                        std::cerr << "Warning: Invalid value for ResolutionWidth: " << value << std::endl;
                                } catch (const std::out_of_range& _) {
                                        std::cerr << "Warning: Value for ResolutionWidth out of range: " << value << std::endl;
                                }
                        } else if (key == "ResolutionHeight") {
                                try {
                                        resolutionHeight = std::stoi(value);    // Convert value to int
                                } catch (const std::invalid_argument& _) {
                                        std::cerr << "Warning: Invalid value for ResolutionHeight: " << value << std::endl;
                                } catch (const std::out_of_range& _) {
                                        std::cerr << "Warning: Value for ResolutionHeight out of range: " << value << std::endl;
                                }
                        } else if (key == "FullscreenMode") {
                                fullscreenMode = (value != "false");
                        } else if (key == "VsyncEnabled") {
                                vsyncEnabled = (value != "false");
                        }
                }
        }
        settingsFile.close();
}
void GameSettingsFile::SaveSettings() const {
        std::ofstream settingsFile(getExecutableDirectory() + "/" + SETTINGS_FILE_PATH);
        if (!settingsFile.is_open()) {
                std::cerr << "Error: Could not open settings file for writing: " << SETTINGS_FILE_PATH << std::endl;
                return;
        }

        settingsFile << "ResolutionWidth=" << resolutionWidth << std::endl;
        settingsFile << "ResolutionHeight=" << resolutionHeight << std::endl;
        settingsFile << "FullscreenMode=" << (fullscreenMode ? "true" : "false") << std::endl;
        settingsFile << "VsyncEnabled=" << (vsyncEnabled ? "true" : "false") << std::endl;

        settingsFile.close();
}
int GameSettingsFile::GetResolutionWidth() const { return resolutionWidth; }
int GameSettingsFile::GetResolutionHeight() const { return resolutionHeight; }
bool GameSettingsFile::IsFullscreenMode() const { return fullscreenMode; }
bool GameSettingsFile::IsVsyncEnabled() const { return vsyncEnabled; }
