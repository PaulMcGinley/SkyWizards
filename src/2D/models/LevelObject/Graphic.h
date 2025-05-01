//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <3rdParty/pugixml/pugixml.hpp>
#include <string>
#include <vector>

#include "Boundary.h"

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

        int currentFrame;
        float nextFrameTime;
        bool deserialize(const pugi::xml_node &node);

        std::vector<Boundary>* Boundaries = nullptr; // Pointer to the boundaries for this graphic
};

#endif //GRAPHIC_H
