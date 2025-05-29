//
// Created by Paul McGinley on 29/05/2025.
//

#include "CreditsScene.h"
void CreditsScene::Update(GameTime gameTime) {
        if (inputManager.IsCancelPressed())
                sceneManager.ChangeScene(SceneType::SCENE_MAIN_MENU); // Go back to the main menu


        // If the camera has reached the end position
        if (viewport.getCenter().y >= cameraEndPosition.y) {
                return;
        }

        // Pause camera movement if jump button is pressed
        if (inputManager.IsJumpPressed())
                return;

        viewport.move(0, cameraSpeed * gameTime.deltaTime); // Move the camera downwards
}
void CreditsScene::LateUpdate(GameTime gameTime) {}
void CreditsScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Set background color
        window.clear(sf::Color(0, 0, 0)); // Clear the window with black color

        sf::View previousView = window.getView(); // Store the previous view
        window.setView(viewport);

        float centreX = gameManager.getResolutionWidth() / 2.0f;
        float leftX = centreX - 50.0f;
        float rightX = centreX + 50.0f;


        // Title
        DrawText(window, "OpenSans-Bold", "Legend of Sky Wizards", sf::Vector2f(centreX, 0), Align::CENTER, 50, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "(But from the Side)", sf::Vector2f(centreX, 50), Align::CENTER, 45, sf::Color::White);

        // Developer
        DrawText(window, "OpenSans-ExtraBold", "- Developers -", sf::Vector2f(centreX, 250), Align::CENTER, 45, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Game & Tools", sf::Vector2f(leftX, 300), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Paul F. McGinley", sf::Vector2f(rightX, 300), Align::LEFT, 35, sf::Color::White);

        // Testers
        DrawText(window, "OpenSans-ExtraBold", "- Testers -", sf::Vector2f(centreX, 500), Align::CENTER, 45, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Lorelai McGinley", sf::Vector2f(centreX, 550), Align::CENTER, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Oscar McGinley", sf::Vector2f(centreX, 600), Align::CENTER, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Sebastain McGinley", sf::Vector2f(centreX, 650), Align::CENTER, 35, sf::Color::White);

        // Assets
        // - Music
        DrawText(window, "OpenSans-ExtraBold", "- Assets -", sf::Vector2f(centreX, 850), Align::CENTER, 45, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Level Music", sf::Vector2f(leftX, 900), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Brevyn", sf::Vector2f(rightX, 900), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Guifrog", sf::Vector2f(rightX, 950), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Xennial", sf::Vector2f(rightX, 1000), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Regular", "Songs sourced from FMA - freemusicarchive.org", sf::Vector2f(centreX, 1050), Align::CENTER, 35, sf::Color::White);
        // - Sound Effects
        DrawText(window, "OpenSans-Bold", "Sound Effects", sf::Vector2f(leftX, 1150), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Various Artists", sf::Vector2f(rightX, 1150), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Regular", "Sound effects sourced from Pixabay - pixabay.com", sf::Vector2f(centreX, 1200), Align::CENTER, 35, sf::Color::White);
        // - Assets
        DrawText(window, "OpenSans-Bold", "Collectable Coins", sf::Vector2f(leftX, 1300), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Vicki Wenderlich (GameArtGuppy)", sf::Vector2f(rightX, 1300), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Input Icons", sf::Vector2f(leftX, 1350), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Kenny (kenny.nl)", sf::Vector2f(rightX, 1350), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Heart Icons", sf::Vector2f(leftX, 1400), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "VampireGirl (fliflifly.itch.io)", sf::Vector2f(rightX, 1400), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Wizards", sf::Vector2f(leftX, 1450), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Wizard for Battle by Dungeon Mason", sf::Vector2f(rightX, 1450), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Monsters", sf::Vector2f(leftX, 1500), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Monster for Survival by Dungeon Mason", sf::Vector2f(rightX, 1500), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Scenery", sf::Vector2f(leftX, 1550), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Low Poly Nature by Pure Poly", sf::Vector2f(rightX, 1550), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Regular", "To all of the artists who graciously offer free content, thank you!", sf::Vector2f(centreX, 1600), Align::CENTER, 35, sf::Color::White);

        // Guides
        DrawText(window, "OpenSans-ExtraBold", "- Guides -", sf::Vector2f(centreX, 1800), Align::CENTER, 45, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Sprite Sheet Rendering", sf::Vector2f(leftX, 1850), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "dukeofdevon (YouTube)", sf::Vector2f(rightX, 1850), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Regular", "UE5 - Rendering 3D Models as 2D Sprite Sheets! | https://youtu.be/jNmhBlUPtUA", sf::Vector2f(centreX, 1900), Align::CENTER, 35, sf::Color::White);

        // Lecturers
        DrawText(window, "OpenSans-ExtraBold", "- Lecturers -", sf::Vector2f(centreX, 2100), Align::CENTER, 45, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Sarah Hertzog", sf::Vector2f(leftX, 2150), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Mobile Game Development (Unity)", sf::Vector2f(rightX, 2150), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Object Orientated Programming (C++, SFML)", sf::Vector2f(rightX, 2200), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Applied Mathematics", sf::Vector2f(rightX, 2250), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Marion Barrie", sf::Vector2f(leftX, 2300), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Game Scripting & Customisation (Unreal Engine)", sf::Vector2f(rightX, 2300), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Game Interface Design", sf::Vector2f(rightX, 2350), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Bold", "Marion Gerrie", sf::Vector2f(leftX, 2400), Align::RIGHT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Games Development (C#, MonoGame)", sf::Vector2f(rightX, 2400), Align::LEFT, 35, sf::Color::White);
        DrawText(window, "OpenSans-Semibold", "Project Management", sf::Vector2f(rightX, 2450), Align::LEFT, 35, sf::Color::White);

        // Special Thanks
        //

        // Thanks for playing
        DrawText(window, "OpenSans-Regular", "Thanks for playing <3", sf::Vector2f(centreX, 2450 + (1080/2)), Align::CENTER, 35, sf::Color::White);
        DrawText(window, "OpenSans-Regular", "https://skywizards.co.uk", sf::Vector2f(centreX, 2500 + (1080/2)), Align::CENTER, 35, sf::Color::White);

        window.setView(previousView); // Restore the previous view
}
void CreditsScene::InitializeScene() { IScene::InitializeScene(); }
void CreditsScene::DestroyScene() {}
void CreditsScene::OnScene_Activate() {
        cameraStartPosition.x = gameManager.getResolutionWidth() / 2.0f; // Center the camera horizontally
        viewport.setSize(sf::Vector2f(gameManager.getResolutionWidth(), gameManager.getResolutionHeight())); // Set the view size to the window size
        viewport.setCenter(cameraStartPosition); // Set the initial camera position
}
void CreditsScene::OnScene_Deactivate() {}
