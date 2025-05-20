//
// Created by Paul McGinley on 28/04/2025.
//

#include "DebugOverlay.h"

#include <ostream>

DebugOverlay::DebugOverlay() {
        font = assetManager.Fonts["OpenSans-Bold"].get();
        text.setFont(*font);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);
        text.setPosition(1920-100, 5);
}
void DebugOverlay::Update(GameTime gameTime) {
        fpsTimer += gameTime.deltaTime;
        fpsCounter++;
        if (fpsTimer >= 1.0f) {
                currentFps = fpsCounter;
                fpsCounter = 0;
                fpsTimer -= 1.0f;
                text.setString("FPS: " + std::to_string(currentFps));
                text.setPosition(gameManager.getResolutionWidth() - text.getGlobalBounds().width - 10, 5);
        }

        AddInfoTopRight("FPS", std::to_string(currentFps));
}
void DebugOverlay::LateUpdate(GameTime gameTime) {}
void DebugOverlay::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Draw the debug info
        int yOffset = 0;
        for (const auto& [key, value] : debugInfo_TopLeft) {
                sf::Text text;
                text.setFont(*font);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                std::string tempString = key;
                tempString += ": ";
                tempString += value;
                text.setString(tempString);
                text.setOutlineColor(sf::Color::White);
                text.setOutlineThickness(2.f);
                text.setPosition(10, 10 + yOffset);
                window.draw(text);
                yOffset += 30;
        }

        yOffset = 0;
        for (const auto& [key, value] : debugInfo_TopRight) {
                sf::Text text;
                text.setFont(*font);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                std::string tempString = key;
                tempString += ": ";
                tempString += value;
                text.setString(tempString);
                text.setOutlineColor(sf::Color::White);
                text.setOutlineThickness(2.f);
                text.setPosition(gameManager.getResolutionWidth() - text.getGlobalBounds().width - 10, 10 + yOffset);
                window.draw(text);
                yOffset += 30;
        }

        yOffset = gameManager.getResolutionHeight() - 30;
        for (const auto& [key, value] : debugInfo_BottomLeft) {
                sf::Text text;
                text.setFont(*font);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                std::string tempString = key;
                tempString += ": ";
                tempString += value;
                text.setString(tempString);
                text.setOutlineColor(sf::Color::White);
                text.setOutlineThickness(2.f);
                text.setPosition(10, yOffset - 30);
                window.draw(text);
                yOffset -= 30;
        }

        yOffset = gameManager.getResolutionHeight() - 30;
        for (const auto& [key, value] : debugInfo_BottomRight) {
                sf::Text text;
                text.setFont(*font);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                std::string tempString = key;
                tempString += ": ";
                tempString += value;
                text.setString(tempString);
                text.setOutlineColor(sf::Color::White);
                text.setOutlineThickness(2.f);
                text.setPosition(gameManager.getResolutionWidth() - text.getGlobalBounds().width - 10, yOffset - 30);
                window.draw(text);
                yOffset -= 30;
        }
}
void DebugOverlay::InitializeScene() { IScene::InitializeScene(); }
void DebugOverlay::DestroyScene() {}
void DebugOverlay::OnScene_Activate() {}
void DebugOverlay::OnScene_Deactivate() {}
