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

        int entryCount; // Store the number of cells in the file
        file.read(reinterpret_cast<char*>(&entryCount), sizeof(entryCount)); // Read the number of cells from the file
        // retinterpret_cast is used to convert the pointer type to another pointer type

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
            // entry.texQuad[0].position = {static_cast<float>(entry.xOffset), static_cast<float>(entry.yOffset)};
            // entry.texQuad[1].position = {static_cast<float>(entry.xOffset + entry.texture.getSize().x), static_cast<float>(entry.yOffset)};
            // entry.texQuad[2].position = {static_cast<float>(entry.xOffset + entry.texture.getSize().x), static_cast<float>(entry.yOffset + entry.texture.getSize().y)};
            // entry.texQuad[3].position = {static_cast<float>(entry.xOffset), static_cast<float>(entry.yOffset + entry.texture.getSize().y)};
            //
            // entry.texQuad[0].texCoords = {0, 0};
            // entry.texQuad[1].texCoords = {static_cast<float>(entry.texture.getSize().x), 0};
            // entry.texQuad[2].texCoords = {static_cast<float>(entry.texture.getSize().x), static_cast<float>(entry.texture.getSize().y)};
            // entry.texQuad[3].texCoords = {0, static_cast<float>(entry.texture.getSize().y)};

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