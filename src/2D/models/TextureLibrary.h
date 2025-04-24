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

private:
        std::string path;
        std::streampos* fat; // File Allocation Table (kinda)
        void UnloadIndices(const std::vector<int>& requiredIndices);
};

#endif // TEXTURELIBRARY_H