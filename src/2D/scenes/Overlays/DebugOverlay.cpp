//
// Created by Paul McGinley on 28/04/2025.
//

#include "DebugOverlay.h"

#include <ostream>

DebugOverlay::DebugOverlay() {
        fpsFont = asset_manager.Fonts["OpenSans-Bold"].get();
        fpsText.setFont(*fpsFont);
        fpsText.setCharacterSize(24);
        fpsText.setFillColor(sf::Color::Black);
        fpsText.setPosition(1920-100, 5);
}
void DebugOverlay::Update(GameTime gameTime) {
        fpsTimer += gameTime.delta_time;
        fpsCounter++;
        if (fpsTimer >= 1.0f) {
                currentFps = fpsCounter;
                fpsCounter = 0;
                fpsTimer -= 1.0f;
                fpsText.setString("FPS: " + std::to_string(currentFps));
                fpsText.setPosition(game_manager.getResolutionWidth() - fpsText.getGlobalBounds().width - 10, 5);
        }

        AddInfoTopRight("FPS", std::to_string(currentFps));
}
void DebugOverlay::LateUpdate(GameTime gameTime) {}
void DebugOverlay::Draw(sf::RenderWindow &window, GameTime gameTime) {

        // Draw the debug info
        int yOffset = 0;
        for (const auto& [key, value] : debugInfo_TopLeft) {
                sf::Text text;
                text.setFont(*fpsFont);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                text.setString(key + ": " + value);
                text.setPosition(10, 10 + yOffset);
                window.draw(text);
                yOffset += 30;
        }

        yOffset = 0;
        for (const auto& [key, value] : debugInfo_TopRight) {
                sf::Text text;
                text.setFont(*fpsFont);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                text.setString(key + ": " + value);
                text.setPosition(game_manager.getResolutionWidth() - text.getGlobalBounds().width - 10, 10 + yOffset);
                window.draw(text);
                yOffset += 30;
        }

        yOffset = game_manager.getResolutionHeight() - 30;
        for (const auto& [key, value] : debugInfo_BottomLeft) {
                sf::Text text;
                text.setFont(*fpsFont);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                text.setString(key + ": " + value);
                text.setPosition(10, yOffset - 30);
                window.draw(text);
                yOffset -= 30;
        }

        yOffset = game_manager.getResolutionHeight() - 30;
        for (const auto& [key, value] : debugInfo_BottomRight) {
                sf::Text text;
                text.setFont(*fpsFont);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                text.setString(key + ": " + value);
                text.setPosition(game_manager.getResolutionWidth() - text.getGlobalBounds().width - 10, yOffset - 30);
                window.draw(text);
                yOffset -= 30;
        }
}
void DebugOverlay::InitializeScene() { IScene::InitializeScene(); }
void DebugOverlay::DestroyScene() {}
void DebugOverlay::OnScene_Active() {}
void DebugOverlay::OnScene_Deactivate() {}
