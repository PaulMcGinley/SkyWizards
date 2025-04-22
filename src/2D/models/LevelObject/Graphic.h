//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <string>

class Graphic {
public:
        std::string BackImageLibrary;
        int BackIndex;
        int BackEndIndex;
        int BackAnimationSpeed;

        int BackImageCurrentFrame;
        int X;
        int Y;
        int DrawLayer;
};

#endif //GRAPHIC_H
