//
// Created by Paul McGinley on 28/04/2025.
//

#ifndef DEBUGOVERLAY_H
#define DEBUGOVERLAY_H
#include <SFML/Graphics/Text.hpp>


#include "interfaces/IScene.h"


class DebugOverlay : public IScene {
public:
        DebugOverlay();
        ~DebugOverlay() override = default;

        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;

        void AddInfoLeft(const std::string& key, const std::string_view& value) {
                debugInfo_Left[key] = value;
        }
        void AddInfoRight(const std::string& key, const std::string_view& value) {
                debugInfo_Right[key] = value;
        }

private:
        std::map<std::string, std::string> debugInfo_Right;
        std::map<std::string, std::string> debugInfo_Left;
        sf::Font *fpsFont;
        sf::Text fpsText;

        float fpsTimer = 0.f;
        int fpsCounter = 0;
        int currentFps = 0;
};



#endif //DEBUGOVERLAY_H
