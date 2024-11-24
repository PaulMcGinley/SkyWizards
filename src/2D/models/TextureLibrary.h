#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include "TextureEntry.h"

class TextureLibrary {
public:
    TextureLibrary(const std::string& path) {

        if (path.empty())
            return;

        // Start the timer
        auto start = std::chrono::high_resolution_clock::now();

        // Open the file in binary mode
        std::ifstream file(path, std::ios::binary);

        // Check if the file failed to open
        if (!file.is_open())
            throw std::runtime_error("Could not open file: " + path); // Throw an exception

        // Read and validate the version byte
        uint8_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (version != 1)
            throw std::runtime_error("Invalid version: " + std::to_string(version));

        // Read and validate the character array
        char header[25];
        file.read(header, sizeof(header) - 1);
        header[24] = '\0'; // Null-terminate the string
        if (std::string(header) != "Sky Wizard Asset Library")
            throw std::runtime_error("Invalid header: " + std::string(header));

        // Read and validate the type byte
        uint8_t type;
        file.read(reinterpret_cast<char*>(&type), sizeof(type));
        if (type != 1)
            throw std::runtime_error("Invalid type: " + std::to_string(type));

        int entryCount; // Store the number of cells in the file
        file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount)); // Read the number of cells from the file

        // Loop through the cells in the file
        for (int i = 0; i < entryCount; ++i) {
            TextureEntry entry;
            file.read(reinterpret_cast<char*>(&entry.xOffset), sizeof(entry.xOffset)); // Read the x offset from the file
            file.read(reinterpret_cast<char*>(&entry.yOffset), sizeof(entry.yOffset)); // Read the y offset from the file

            int dataLength; // Store the length of the data
            file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength)); // Read the image data length from the file

            entry.data.resize(dataLength);  // Resize the data vector to the length of the data
            file.read(reinterpret_cast<char*>(entry.data.data()), dataLength); // Read the image data from the file

            entry.texture.loadFromMemory(entry.data.data(), entry.data.size()); // Load the texture from the image data

            entry.CalculateQuads();
            entry.data.clear();
            entries.push_back(entry); // Add the cell to the Cells vector
        }

        // Stop the timer
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        // Output the duration
        std::cout << "Time taken to load the library: " << duration.count() << " seconds" << std::endl;
    }

    std::vector<TextureEntry> entries;
};