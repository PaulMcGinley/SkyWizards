//
// Created by Paul McGinley on 16/10/2024.
//

#ifndef GETEXECUTIONDIRECTORY_H
#define GETEXECUTIONDIRECTORY_H


#include <string>
#include <iostream>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
#elif __linux__
    #include <unistd.h>
#endif

// Function to get the full executable path
inline std::string getExecutablePath() {
    char path[PATH_MAX];

#ifdef _WIN32
    // Windows specific method
    GetModuleFileName(NULL, path, PATH_MAX);

#elif __APPLE__
    // macOS specific method
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) {
        std::cerr << "Buffer too small; need size " << size << std::endl;
        return "";
    }

#elif __linux__
    // Linux specific method
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count == -1) {
        std::cerr << "Error reading the executable path." << std::endl;
        return "";
    }
    path[count] = '\0';  // Null-terminate the string

#else
    // Unsupported OS
    std::cerr << "Unsupported OS." << std::endl;
    return "";
#endif

    return std::string(path);
}

// Function to get the executable's directory
inline std::string getExecutableDirectory() {
    std::string execPath = getExecutablePath();
    if (execPath.empty()) return "";

    // Remove the executable name to get the directory
    return execPath.substr(0, execPath.find_last_of("/\\"));
}


#endif //GETEXECUTIONDIRECTORY_H
