//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <3rdParty/pugixml/pugixml.hpp>

class Boundary {
public:
        int Frame;
        int X;
        int Y;
        int Width;
        int Height;
        bool Active; // Editor only, not used in game
        bool Deserialize(const pugi::xml_node &node);
        Boundary() = default;
};

#endif //BOUNDARY_H
