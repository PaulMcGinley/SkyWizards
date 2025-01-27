//
// Created by Paul McGinley on 22/10/2024.
//

#ifndef ISLAND_H
#define ISLAND_H

#include <map>

#include "interfaces/IUpdate.h"
#include "interfaces/IDraw.h"

#include <SFML/Graphics/Sprite.hpp>

#include "enums/AniType.h"
#include "interfaces/IAnimate.h"

class Island : public IUpdate, public IDraw, public IAnimate {
public:
        Island(TextureLibrary &library, int startIndex, int length, int tick, sf::Vector2f position) {
                this->library = library;
                this->position = position;
                sequence = AnimationSequence(startIndex, length, tick);
                sprite.setTexture(library.GetTexture(AnimationType::Island));
                sprite.setPosition(position);
        }
        ~Island() override = default;


        void update(GameTime gameTime) override {
                // Middle image in animation is front, eather shide should have an equal amount of frames
                // These frames should be drawn based on the middle of the image being centered on screen
        }

        void draw(sf::RenderWindow& window, GameTime gameTime) override = 0;

        void lateUpdate(GameTime gameTime) override {
                tickAnimation(gameTime);
        }

private:
        sf::Vector2f position;
        sf::Sprite sprite;
};

#endif //ISLAND_H
