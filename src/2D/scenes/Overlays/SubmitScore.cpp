//
// Created by Paul McGinley on 30/05/2025.
//

#include "SubmitScore.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <string>

void SubmitScore::Update(GameTime gameTime) {
        if (inputManager.MoveRightPressed()) {
                index = (index + 1) % 5;
        }

        if (inputManager.MoveLeftPressed()) {
                index = (index - 1 + 5) % 5;
        }

        if (inputManager.NavigateUpPressed()) {
                if (name[index] >= 'A' && name[index] < 'Z') {
                        name[index]++;
                } else if (name[index] == 'Z') {
                        name[index] = ' ';
                } else if (name[index] == ' ') {
                        name[index] = 'A';
                }
        }

        if (inputManager.NavigateDownPressed()) {
                if (name[index] > 'A' && name[index] <= 'Z') {
                        name[index]--;
                } else if (name[index] == 'A') {
                        name[index] = ' ';
                } else if (name[index] == ' ') {
                        name[index] = 'Z';
                }
        }
}
void SubmitScore::LateUpdate(GameTime gameTime) {}
void SubmitScore::Draw(sf::RenderWindow &window, GameTime gameTime) {
        IDraw::Draw(window, "PrgUse", 0 , {0,0});
        IDraw::Draw(window, "PrgUse", 1 , {0,static_cast<float>((gameManager.getResolutionHeight()/2) - assetManager.TextureLibraries["PrgUse"].get()->entries[1].texture.getSize().y/2)});

        IDraw::DrawText(window, "OpenSans-Bold", "Enter Your Name:", sf::Vector2f(960, 150), Align::CENTER, 50, sf::Color::White, 2.0f);

        float screenCenterX = gameManager.getResolutionWidth()/2;
        float charY = gameManager.getResolutionHeight()/2 - 200;
        int charSize = 200;
        float kerning = 150.0f;

        // Calculate the X position for the center of the first character
        // to center the whole group of 5 characters
        float firstCharCenterX = screenCenterX - ((5 - 1) * kerning) / 2.0f;

        for (int i = 0; i < 5; ++i) {

                char charBuffer[2] = {name[i], '\0'}; // Create a null-terminated string for the single character
                sf::Vector2f charPos(firstCharCenterX + i * kerning, charY);

                IDraw::DrawText(window, "OpenSans-ExtraBold", charBuffer, charPos, Align::CENTER, charSize, sf::Color::White, 2.f);

                // Draw the karat under the character
                if (i == index) {
                        sf::RectangleShape highlightBox;
                        float boxWidth = static_cast<float>(charSize) * 0.9f;
                        float boxHeight = 25;

                        highlightBox.setSize(sf::Vector2f(boxWidth, boxHeight));
                        highlightBox.setOrigin(boxWidth / 2.0f, boxHeight / 2.0f);
                        highlightBox.setPosition(charPos+ sf::Vector2f{0,300});

                        highlightBox.setFillColor(sf::Color::Blue);
                        window.draw(highlightBox);
                }
        }

        IDraw::DrawText(window, "OpenSans-ExtraBold", "Use the stick to move and change letters", sf::Vector2f(screenCenterX, gameManager.getResolutionHeight() - 300), Align::CENTER, 30, sf::Color::Black, 0.0f);
        IDraw::DrawText(window, "OpenSans-ExtraBold", "Press CONFIRM to submit your score", sf::Vector2f(screenCenterX, gameManager.getResolutionHeight() - 245), Align::CENTER, 30, sf::Color::White, 1.0f);
}
void SubmitScore::InitializeScene() {
        IScene::InitializeScene();
        // Initialize name to "AAAAA"
        for (int i = 0; i < 5; ++i) {
                name[i] = 'A';
        }

        index = 0;
}
void SubmitScore::DestroyScene() {}
void SubmitScore::OnScene_Activate() {}
void SubmitScore::OnScene_Deactivate() {}