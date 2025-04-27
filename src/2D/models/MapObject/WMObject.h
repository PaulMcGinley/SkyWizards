//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef WMOBJECT_H
#define WMOBJECT_H

#include <3rdParty/pugixml/pugixml.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
// #include "models/LevelObject/OLibrary.h"

class WMObject {
public:
        std::string ObjectLibraryFile;
        // OLibrary ObjectLibrary; // This will be a copy of the OLibrary for indipendant use
        sf::Vector2f Position;
        bool deserialize(const pugi::xml_node &node);
};

#endif //WMOBJECT_H
