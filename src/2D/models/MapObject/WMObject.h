//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef WMOBJECT_H
#define WMOBJECT_H

#include <3rdParty/pugixml/pugixml.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

class WMObject {
public:
        std::string ObjectLibraryFile;
        sf::Vector2f Position;
        bool Deserialize(const pugi::xml_node &node);
};

#endif //WMOBJECT_H
