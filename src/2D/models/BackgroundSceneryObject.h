//
// Created by Paul McGinley on 10/10/2024.
//

#ifndef BACKGROUNDSCENERYOBJECT_H
#define BACKGROUNDSCENERYOBJECT_H

#include "interfaces/IAnimate.h"
#include "interfaces/IDraw.h"
#include "interfaces/IUpdate.h"

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>

class BackgroundSceneryObject : public IUpdate, public IDraw, public IAnimate {
public:
        BackgroundSceneryObject(TextureLibrary &library, int start_index, int length, int tick, sf::Vector2f position);
        ~BackgroundSceneryObject() override = default;

        void update(GameTime gameTime) override;
        void lateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;

private:
        sf::Vector2f position;
        sf::Sprite sprite;
        TextureLibrary& library;

        AnimationSequence sequence;
        bool shaderLoaded = false;
        sf::Shader blurShader;
        float blurRadius = 0.0f;
};

#endif //BACKGROUNDSCENERYOBJECT_H
