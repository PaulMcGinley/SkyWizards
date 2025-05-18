//
// Created by Paul McGinley on 18/05/2025.
//

#ifndef COLLECTABLE_H
#define COLLECTABLE_H

#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"

#include <3rdParty/pugixml/pugixml.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>



class Collectable : public IAnimate, public IUpdate, public IDraw {
public:
        Collectable();
        Collectable(const Collectable& other);


        void Draw(sf::RenderWindow &window, GameTime gameTime) override;
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void deserialize(const pugi::xml_node &node);
        [[nodiscard]] sf::FloatRect GetCollisionBox() const;
        void SetPosition(float x, float y);
        int GetCurrentFrame();
        sf::Vector2f GetPosition() const;

private:
        std::string Library;
        int startIndex;
        int endIndex;
        int animationTick;
        sf::Vector2f position;
        sf::FloatRect collisionBox;
};



#endif //COLLECTABLE_H
