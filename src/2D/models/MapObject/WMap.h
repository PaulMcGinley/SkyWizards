//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef WMAP_H
#define WMAP_H

#include <string>
#include <vector>
#include "WMObject.h"
// #include "interfaces/IDraw.h"
// #include "interfaces/IUpdate.h"

class WMap /*: public IDraw, public IUpdate*/{
public:
        std::string FileName;

        int ParallaxBackgroundIndex;
        int MountainsBackgroundIndex;
        std::vector<WMObject> LevelObjects;
        std::vector<std::string> Scripts;
        bool deserialize(const pugi::xml_node &node);

        // void Update(GameTime gameTime) override;
        // void LateUpdate(GameTime gameTime) override;
        // void Draw(sf::RenderWindow &window, GameTime gameTime) override;
};

#endif //WMAP_H
