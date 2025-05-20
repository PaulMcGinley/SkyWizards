//
// Created by Paul McGinley on 11/05/2025.
//

#ifndef WMMOB_H
#define WMMOB_H

#include <3rdParty/pugixml/pugixml.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

class WMMob {
public:
        std::string MonsterName;
        sf::Vector2f Position;
        float MoveSpeed;
        float ViewRange;
        int Health;
        bool Deserialize(const pugi::xml_node &node);
};



#endif //WMMOB_H
