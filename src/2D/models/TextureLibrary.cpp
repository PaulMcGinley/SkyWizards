//
// Created by Paul McGinley on 24/04/2025.
//

#include "TextureLibrary.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

// Load TextureLibrary from a file with no specific indices
// This constructor will call the other constructor with an empty vector
TextureLibrary::TextureLibrary(const std::string &path)
        : entries(nullptr)
        , entryCount(0)
        , path(path)
        , fat(nullptr) {
        if (path.empty())
                return;

        // Open the file in binary mode
        std::ifstream file(path, std::ios::binary);

        // Check if the file failed to open
        if (!file.is_open()) {
                std::cerr << "Failed to open file: " << path << std::endl;
                return;
        }

        // Read and validate the 'Version' byte
        uint8_t version;
        file.read(reinterpret_cast<char *>(&version), sizeof(version));
        if (version != 1) {
                std::cerr << "Invalid version: " << static_cast<int>(version) << std::endl;
                return;
        }

        // Read and validate the 'Header' string
        char header[25];
        file.read(header, sizeof(header) - 1);
        header[24] = '\0'; // Null-terminate the string
        if (std::string(header) != "Sky Wizard Asset Library") {
                std::cerr << "Invalid header: " << header << std::endl;
                return;
        }

        // Read and validate the 'Library Type' byte
        uint8_t type;
        file.read(reinterpret_cast<char *>(&type), sizeof(type));
        if (type != 1) { // Type 1 is the TextureLibrary type
                std::cerr << "Invalid type: " << static_cast<int>(type) << std::endl;
                return;
        }

        // Read the entry count (number of cells) in the library
        file.read(reinterpret_cast<char *>(&entryCount), sizeof(entryCount));
        std::cout << "Library: " << path << "Entry count: " << entryCount << std::endl;

        // if (entries != nullptr) // Check if the entries array already exists
        //         delete[] entries; // Delete the old entries array if it exists
        // Allocate memory for the entries array
        entries = new TextureEntry[entryCount];

        // if (fat != nullptr) // Check if the FAT array already exists
        //         delete[] fat; // Delete the old FAT array if it exists
        // Allocate memory for the FAT array
        fat = new std::streampos[entryCount];

        // Go through the file and add all the positions to the FAT array
        for (int i = 0; i < entryCount; ++i) {
                fat[i] = file.tellg(); // Store the position of the current entry
                file.seekg(2 * sizeof(int32_t), std::ios::cur); // Skip past X and Y offsets
                int32_t dataLength;
                file.read(reinterpret_cast<char *>(&dataLength), sizeof(dataLength)); // Read the length of the data
                file.seekg(dataLength, std::ios::cur); // Skip past the data
        }
}

// Deconstructor for TextureLibrary
TextureLibrary::~TextureLibrary() {
        // Clean up the entries array
        // delete[] entries;
        // entries = nullptr;

        // Clean up the FAT array
        // delete[] fat;
        // fat = nullptr;

        entryCount = 0;
}

void TextureLibrary::LoadIndices(const std::vector<int> &indices) {
        if (path.empty())
                return;

        std::cout << "Loading indices from: " << path << std::endl;

        // Open the file in binary mode
        std::ifstream file(path, std::ios::binary);

        // Check if the file failed to open
        if (!file.is_open()) {
                std::cerr << "Failed to open file: " << path << std::endl;
                return;
        }

        // Jump to the offset where the entries start
        // constexpr as we know the size of the headers at compile time
        // constexpr int offset =
        //              sizeof(uint8_t)                    // Version
        //            + 24                                 // Header string (fixed 24 bytes) "Sky Wizard Asset Library"
        //            + sizeof(uint8_t)                    // Type
        //            + sizeof(int);                       // Entry count
        // file.seekg(offset, std::ios::beg);              // Move the file pointer to the start of the entries (offset from beginning)

        // Determine whether to load all entries or specific indices
        const bool loadAll = indices.empty();
        std::cout << "Loading " << (loadAll ? "all" : "specific") << " entries." << std::endl;

        // If loading all, process as before for all entries
        if (loadAll) {
                if (fullyLoaded)
                        return;

                for (int i = 0; i < entryCount; ++i) {
                        file.seekg(fat[i], std::ios::beg);

                        TextureEntry entry;
                        file.read(reinterpret_cast<char *>(&entry.xOffset), sizeof(entry.xOffset));
                        file.read(reinterpret_cast<char *>(&entry.yOffset), sizeof(entry.yOffset));

                        int dataLength;
                        file.read(reinterpret_cast<char *>(&dataLength), sizeof(dataLength));

                        entry.data.resize(dataLength);
                        file.read(reinterpret_cast<char *>(entry.data.data()), dataLength);

                        entry.texture.loadFromMemory(entry.data.data(), entry.data.size());
                        entry.CalculateQuads();
                        entry.data.clear();

                        entries[i] = entry;
                }

                fullyLoaded = true; // Mark as fully loaded
        } else {
                // Only load specified indices
                for (int idx : indices) {
                        if (idx < 0 || idx >= entryCount) continue; // Bounds check
                        file.seekg(fat[idx], std::ios::beg);

                        TextureEntry entry;
                        file.read(reinterpret_cast<char *>(&entry.xOffset), sizeof(entry.xOffset));
                        file.read(reinterpret_cast<char *>(&entry.yOffset), sizeof(entry.yOffset));

                        int dataLength;
                        file.read(reinterpret_cast<char *>(&dataLength), sizeof(dataLength));

                        entry.data.resize(dataLength);
                        file.read(reinterpret_cast<char *>(entry.data.data()), dataLength);

                        entry.texture.loadFromMemory(entry.data.data(), entry.data.size());
                        entry.CalculateQuads();
                        entry.data.clear();

                        entries[idx] = entry;
                }
        }
}

void TextureLibrary::UnloadIndices(const std::vector<int> &indices) {
        for (int idx : indices) {
                if (idx < 0 || idx >= entryCount)
                        continue;
                entries[idx].texture = sf::Texture(); // Replace with a new, empty texture
        }
        fullyLoaded = false;
}