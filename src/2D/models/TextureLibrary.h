#ifndef TEXTURELIBRARY_H
#define TEXTURELIBRARY_H

#include <string>
#include <vector>
#include "TextureEntry.h"

class TextureLibrary {
public:
        TextureLibrary(const std::string& path);
        ~TextureLibrary();

        TextureEntry* entries;
        int entryCount;

        void LoadIndices(const std::vector<int>& indices);
        void UnloadIndices(const std::vector<int>& requiredIndices);
        bool fullyLoaded = false;
        bool allowUnload = true; // This flag will allow us to use sprites from required libraries without them being unloaded on scene change

private:
        std::string path;
        std::streampos* fat; // File Allocation Table (kinda)
};

#endif // TEXTURELIBRARY_H