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
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;

        void AddInfoTopLeft(const std::string& key, const std::string_view& value) { debugInfo_TopLeft[key] = value; }
        void AddInfoTopRight(const std::string& key, const std::string_view& value) { debugInfo_TopRight[key] = value; }
        void AddInfoBottomLeft(const std::string& key, const std::string_view& value) { debugInfo_BottomLeft[key] = value; }
        void AddInfoBottomRight(const std::string& key, const std::string_view& value) { debugInfo_BottomRight[key] = value; }

        void RemoveInfoTopLeft(const std::string& key) { debugInfo_TopLeft.erase(key); }
        void RemoveInfoTopRight(const std::string& key) { debugInfo_TopRight.erase(key); }
        void RemoveInfoBottomLeft(const std::string& key) { debugInfo_BottomLeft.erase(key); }
        void RemoveInfoBottomRight(const std::string& key) { debugInfo_BottomRight.erase(key); }

private:
        std::map<std::string, std::string> debugInfo_TopRight;
        std::map<std::string, std::string> debugInfo_TopLeft;
        std::map<std::string, std::string> debugInfo_BottomLeft;
        std::map<std::string, std::string> debugInfo_BottomRight;
        sf::Font *font;
        sf::Text text;

        float fpsTimer = 0.f;
        int fpsCounter = 0;
        int currentFps = 0;
};



#endif //DEBUGOVERLAY_H
