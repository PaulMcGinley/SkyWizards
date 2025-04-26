//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef OLIBRARY_H
#define OLIBRARY_H

#include <3rdParty/pugixml/pugixml.hpp>
#include <string>

#include "BoundaryGroup.h"
#include "Graphic.h"
// #include "interfaces/IDraw.h"
// #include "interfaces/IUpdate.h"

class OLibrary /*: public IDraw, public IUpdate*/ {
public:
        std::string FileName;
        std::vector<Graphic> Images;
        std::vector<BoundaryGroup> BoundaryGroups;
        bool deserialize(const pugi::xml_document &doc);

        // void Update(GameTime gameTime) override;
        // void LateUpdate(GameTime gameTime) override;
        // void Draw(sf::RenderWindow &window, GameTime gameTime) override;
};

#endif //OLIBRARY_H
