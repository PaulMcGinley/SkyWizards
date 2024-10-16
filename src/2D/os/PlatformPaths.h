//
// Created by Paul McGinley on 16/10/2024.
//

#ifndef PLATFORMPATHS_H
#define PLATFORMPATHS_H

#include <string>
#include <cstdlib>
#include <iostream>
#include <filesystem>  // For creating directories
#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>
#elif __APPLE__
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#elif __linux__
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#endif

class PlatformPaths {
public:
    // Returns the OS-specific path for storing user application data with 'SkyWizardsGame' appended.
    static std::string getUserDataPath() {
        std::string basePath;

        #ifdef _WIN32
            basePath = getWindowsAppDataRoaming();
        #elif __APPLE__
            basePath = getMacOSAppSupportPath();
        #elif __linux__
            basePath = getLinuxUserDataPath();
        #else
            std::cerr << "Unsupported OS." << std::endl;
            return "";
        #endif

        // Append the 'SkyWizardsGame' folder to the base path
        std::string fullPath = basePath + "/SkyWizardsGame";

        // Create the directory if it doesn't exist
        std::filesystem::create_directories(fullPath);

        return fullPath;
    }

private:
    #ifdef _WIN32
    // Windows: Retrieves the AppData\Roaming directory
    static std::string getWindowsAppDataRoaming() {
        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
            return std::string(path);
        } else {
            std::cerr << "Error getting AppData\\Roaming folder." << std::endl;
            return "";
        }
    }
    #endif

    #ifdef __APPLE__
    // macOS: Retrieves the ~/Library/Application Support directory
    static std::string getMacOSAppSupportPath() {
        struct passwd* pw = getpwuid(getuid());
        if (pw != NULL) {
            return std::string(pw->pw_dir) + "/Library/Application Support";
        } else {
            std::cerr << "Error getting Application Support path on macOS." << std::endl;
            return "";
        }
    }
    #endif

    #ifdef __linux__
    // Linux: Retrieves the $XDG_DATA_HOME or ~/.local/share directory
    static std::string getLinuxUserDataPath() {
        const char* xdgDataHome = std::getenv("XDG_DATA_HOME");
        if (xdgDataHome) {
            return std::string(xdgDataHome);
        } else {
            struct passwd* pw = getpwuid(getuid());
            if (pw != NULL) {
                return std::string(pw->pw_dir) + "/.local/share";
            } else {
                std::cerr << "Error getting user data path on Linux." << std::endl;
                return "";
            }
        }
    }
    #endif
};


#endif //PLATFORMPATHS_H
