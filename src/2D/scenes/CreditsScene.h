//
// Created by Paul McGinley on 29/05/2025.
//

#ifndef CREDITSSCENE_H
#define CREDITSSCENE_H
#include <SFML/Graphics/RenderTexture.hpp>


#include "interfaces/IScene.h"


class CreditsScene : public IScene {
public:
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;

private:
        sf::View viewport;
        sf::Vector2f cameraStartPosition = {0, -(1080/2)};
        sf::Vector2f cameraEndPosition = {0, 2550 + (1080/2)};
        float cameraSpeed = 100.0f; // Speed of the camera movement
        //float initialDelay = 3500.f; // Initial delay before the camera starts moving

        // Render target for the credits
        sf::RenderTexture creditsTexture;
        bool creditsTextureInitialized = false;
};



#endif //CREDITSSCENE_H
