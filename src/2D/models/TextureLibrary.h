#ifndef TEXTURELIBRARY_H
#define TEXTURELIBRARY_H

#include <string>
#include <vector>
#include "TextureEntry.h"

class TextureLibrary {
public:
        TextureLibrary(const std::string& path);

        std::vector<TextureEntry> entries;
};

#endif // TEXTURELIBRARY_H