//
// Created by Paul McGinley on 18/05/2025.
//

#ifndef WMCOLLECTABLE_H
#define WMCOLLECTABLE_H
#include <3rdParty/pugixml/pugixml.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>


class WMCollectable {
public:
        std::string CollectableName;
        sf::Vector2f Position;
        std::string Action;
        std::string CollectedSound;
        bool Deserialize(const pugi::xml_node &node);
};



#endif //WMCOLLECTABLE_H
