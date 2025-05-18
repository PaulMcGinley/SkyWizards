//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef WMAP_H
#define WMAP_H

#include <string>
#include <vector>

#include "WMCollectable.h"
#include "WMMob.h"
#include "WMObject.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"

class WMap : public IDraw, public IUpdate {
public:
        virtual ~WMap();
        std::string FileName;

        int ParallaxBackgroundIndex;
        int MountainsBackgroundIndex;
        std::vector<WMObject> LevelObjects;
        std::vector<WMMob> Mobs;
        std::vector<WMCollectable> Collectables;
        std::vector<std::string> Scripts;
        bool deserialize(const pugi::xml_node &node);

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;

        sf::Vector2f startPosition = {0, 0};
        sf::RectangleShape endPosition;
};

#endif //WMAP_H
