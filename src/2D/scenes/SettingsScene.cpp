//
// Created by Paul McGinley on 16/05/2025.
//

#include "SettingsScene.h"
#include "managers/InputManager.cpp"

SettingsScene::SettingsScene() {}
void SettingsScene::Update(GameTime gameTime) {
        if (InputManager::getInstance().IsKeyPressed(sf::Keyboard::Num7 )|| InputManager::getInstance().IsKeyPressed(sf::Keyboard::I)) {
                sceneManager.ChangeScene(SceneType::SCENE_MAIN_MENU);
        }

        // Check if the user is pressing the up or down arrow keys to change the selected option
        if (InputManager::getInstance().IsKeyPressed(sf::Keyboard::Up)) {
                selectedOption = (selectedOption - 1 + 4) % 4; // Wrap around to the last option
        } else if (InputManager::getInstance().IsKeyPressed(sf::Keyboard::Down)) {
                selectedOption = (selectedOption + 1) % 4; // Wrap around to the first option
        }


        if(InputManager::getInstance().IsKeyPressed(sf::Keyboard::Left) || InputManager::getInstance().IsKeyPressed(sf::Keyboard::Right) || InputManager::getInstance().IsKeyPressed(sf::Keyboard::A) || InputManager::getInstance().IsKeyPressed(sf::Keyboard::D)) {
                if (selectedOption == 0) {
                        tempIsFullscreen = !tempIsFullscreen;
                }
                else if (selectedOption == 1) {
                        tempIsVSync = !tempIsVSync;
                }
                else if (selectedOption == 2) {
                        if(InputManager::getInstance().IsKeyPressed(sf::Keyboard::Left) || InputManager::getInstance().IsKeyPressed(sf::Keyboard::A) ) {
                                tempMusicPercent -= 0.05f;
                                if (tempMusicPercent < 0.0f) {
                                        tempMusicPercent = 0.0f;
                                }
                        }
                        else if(InputManager::getInstance().IsKeyPressed(sf::Keyboard::Right) || InputManager::getInstance().IsKeyPressed(sf::Keyboard::D)) {
                                tempMusicPercent += 0.05f;
                                if (tempMusicPercent > 1.0f) {
                                        tempMusicPercent = 1.0f;
                                }
                        }
                }
                else if (selectedOption == 3) {
                        if(InputManager::getInstance().IsKeyPressed(sf::Keyboard::Left) || InputManager::getInstance().IsKeyPressed(sf::Keyboard::A) ) {
                                tempSfxPercent -= 0.05f;
                                if (tempSfxPercent < 0.0f) {
                                        tempSfxPercent = 0.0f;
                                }
                        }
                        else if(InputManager::getInstance().IsKeyPressed(sf::Keyboard::Right) || InputManager::getInstance().IsKeyPressed(sf::Keyboard::D)) {
                                tempSfxPercent += 0.05f;
                                if (tempSfxPercent > 1.0f) {
                                        tempSfxPercent = 1.0f;
                                }
                        }
                }
        }

        // Update music percentage display
        int musicPercentInt = static_cast<int>(tempMusicPercent * 100.0f);
        musicPercentDigits[0].setTexture(*numbersTexture[musicPercentInt / 100 % 10], true);
        musicPercentDigits[1].setTexture(*numbersTexture[musicPercentInt / 10 % 10], true);
        musicPercentDigits[2].setTexture(*numbersTexture[musicPercentInt % 10], true);

        // Update sfx percentage display
        int sfxPercentInt = static_cast<int>(tempSfxPercent * 100.0f);
        sfxPercentDigits[0].setTexture(*numbersTexture[sfxPercentInt / 100 % 10], true);
        sfxPercentDigits[1].setTexture(*numbersTexture[sfxPercentInt / 10 % 10], true);
        sfxPercentDigits[2].setTexture(*numbersTexture[sfxPercentInt % 10], true);
}
void SettingsScene::LateUpdate(GameTime gameTime) {}
void SettingsScene::Draw(sf::RenderWindow &window, GameTime gameTime) {
        window.draw(backgroundSprite);
        window.draw(regionSprite);

        window.draw(modeSprite);
        window.draw(vsyncSprite);
        window.draw(musicSprite);
        window.draw(sfxSprite);

        window.draw(modeOptionArrowLeftSprite[selectedOption == 0 ? 1 : 0]);
        window.draw(modeOptionArrowRightSprite[selectedOption == 0 ? 1 : 0]);
        window.draw(tempIsFullscreen ? modeOptionSprite[0] : modeOptionSprite[1]);

        window.draw(vsyncOptionArrowLeftSprite[selectedOption == 1 ? 1 : 0]);
        window.draw(vsyncOptionArrowRightSprite[selectedOption == 1 ? 1 : 0]);
        window.draw(tempIsVSync ? vsyncOptionSprite[0] : vsyncOptionSprite[1]);

        window.draw(musicArrowLeftSprite[selectedOption == 2 ? 1 : 0]);
        window.draw(musicArrowRightSprite[selectedOption == 2 ? 1 : 0]);
        window.draw(sfxArrowLeftSprite[selectedOption == 3 ? 1 : 0]);
        window.draw(sfxArrowRightSprite[selectedOption == 3 ? 1 : 0]);

        // Draw music percentage digits and symbol
        for (auto& digit : musicPercentDigits) {
                window.draw(digit);
        }
        window.draw(musicPercentSymbol);

        // Draw sfx percentage digits and symbol
        for (auto& digit : sfxPercentDigits) {
                window.draw(digit);
        }
        window.draw(sfxPercentSymbol);
}
void SettingsScene::OnScene_Activate() {
        tempIsFullscreen = gameManager.isFullscreen();
        tempIsVSync = gameManager.isVSyncEnabled();


        backgroundTexture = &assetManager.TextureLibraries["PrgUse"].get()->entries[0].texture;                // Background
        regionTexture = &assetManager.TextureLibraries["PrgUse"].get()->entries[1].texture;                    // Region

        backgroundSprite.setTexture(*backgroundTexture, true);
        regionSprite.setTexture(*regionTexture, true);
        regionSprite.setOrigin(regionSprite.getTexture()->getSize().x / 2, (regionSprite.getTexture()->getSize().y / 2) - 50);
        regionSprite.setColor(sf::Color(255, 255, 255, 128));
        regionSprite.setPosition(static_cast<sf::Vector2f>(gameManager.window->getSize()) / 2.0f);

        for(int i = 0; i < 10; ++i) {
                numbersTexture[i] = &assetManager.TextureLibraries["PrgUse"].get()->entries[32+i].texture;     // 32 is the offset for number 0 in PrgUse
        }
        percentTexture = &assetManager.TextureLibraries["PrgUse"].get()->entries[42].texture;                  // 42 is the offset for the percent sign in PrgUse
        checkBoxTexture[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[21].texture;              // Off
        checkBoxTexture[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[20].texture;              // On
        modeTexture = &assetManager.TextureLibraries["PrgUse"].get()->entries[26].texture;                     // Mode
        modeOptionTexture[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[27].texture;            // Windowed
        modeOptionTexture[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[28].texture;            // Fullscreen (Exclusive)
        vsyncTexture = &assetManager.TextureLibraries["PrgUse"].get()->entries[29].texture;                    // VSync
        vsyncOptionTexture[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[3].texture;           // VSync (On)
        vsyncOptionTexture[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[4].texture;           // VSync (Off)
        musicTexture = &assetManager.TextureLibraries["PrgUse"].get()->entries[30].texture;                    // Music
        sfxTexture = &assetManager.TextureLibraries["PrgUse"].get()->entries[31].texture;                      // SFX
        arrowLeftTexture[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[22].texture;             // Left Arrow (Off)
        arrowLeftTexture[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[23].texture;             // Left Arrow (On)
        arrowRightTexture[0] = &assetManager.TextureLibraries["PrgUse"].get()->entries[24].texture;            // Right Arrow (Off)
        arrowRightTexture[1] = &assetManager.TextureLibraries["PrgUse"].get()->entries[25].texture;            // Right Arrow (On)

        // Gap between left label and right values
        // Applied to both sides
        constexpr int middleSpace = 50;

        // Left side
        modeSprite.setTexture(*modeTexture, true);
        modeSprite.setOrigin(modeSprite.getTexture()->getSize().x + middleSpace, modeSprite.getTexture()->getSize().y / 2);

        vsyncSprite.setTexture(*vsyncTexture, true);
        vsyncSprite.setOrigin(vsyncSprite.getTexture()->getSize().x + middleSpace, vsyncSprite.getTexture()->getSize().y / 2);

        musicSprite.setTexture(*musicTexture, true);
        musicSprite.setOrigin(musicSprite.getTexture()->getSize().x + middleSpace, musicSprite.getTexture()->getSize().y / 2);

        sfxSprite.setTexture(*sfxTexture, true);
        sfxSprite.setOrigin(sfxSprite.getTexture()->getSize().x + middleSpace, sfxSprite.getTexture()->getSize().y / 2);

        constexpr int rowSpace = 100;
        sf::Vector2f screenSize = static_cast<sf::Vector2f>(gameManager.window->getSize());

        modeSprite.setPosition(screenSize.x/2, screenSize.y/2 - rowSpace);
        vsyncSprite.setPosition(screenSize.x/2, screenSize.y/2);
        musicSprite.setPosition(screenSize.x/2, screenSize.y/2 + rowSpace);
        sfxSprite.setPosition(screenSize.x/2, screenSize.y/2 + rowSpace*2);

        // Right side
        modeOptionSprite[0].setTexture(*modeOptionTexture[0], true);
        modeOptionSprite[0].setOrigin(modeOptionSprite[0].getTexture()->getSize().x/2, modeOptionSprite[0].getTexture()->getSize().y / 2);
        modeOptionSprite[1].setTexture(*modeOptionTexture[1], true);
        modeOptionSprite[1].setOrigin(modeOptionSprite[1].getTexture()->getSize().x/2, modeOptionSprite[1].getTexture()->getSize().y / 2);

        modeOptionArrowLeftSprite[0].setTexture(*arrowLeftTexture[0], true);
        modeOptionArrowLeftSprite[0].setOrigin(-middleSpace, modeOptionArrowLeftSprite[0].getTexture()->getSize().y / 2);
        modeOptionArrowLeftSprite[1].setTexture(*arrowLeftTexture[1], true);
        modeOptionArrowLeftSprite[1].setOrigin(-middleSpace, modeOptionArrowLeftSprite[1].getTexture()->getSize().y / 2);

        modeOptionArrowRightSprite[0].setTexture(*arrowRightTexture[0], true);
        modeOptionArrowRightSprite[0].setOrigin(0, modeOptionArrowRightSprite[0].getTexture()->getSize().y / 2);
        modeOptionArrowRightSprite[1].setTexture(*arrowRightTexture[1], true);
        modeOptionArrowRightSprite[1].setOrigin(0, modeOptionArrowRightSprite[1].getTexture()->getSize().y / 2);

        modeOptionSprite[0].setPosition(1300, screenSize.y/2 - rowSpace);
        modeOptionSprite[1].setPosition(1300, screenSize.y/2 - rowSpace);

        modeOptionArrowLeftSprite[0].setPosition(screenSize.x/2, screenSize.y/2 - rowSpace);
        modeOptionArrowLeftSprite[1].setPosition(screenSize.x/2, screenSize.y/2 - rowSpace);
        modeOptionArrowRightSprite[0].setPosition(screenSize.x/2 + 600, screenSize.y/2 - rowSpace);
        modeOptionArrowRightSprite[1].setPosition(screenSize.x/2 + 600, screenSize.y/2 - rowSpace);

        vsyncOptionSprite[0].setTexture(*vsyncOptionTexture[0], true);
        vsyncOptionSprite[0].setOrigin(vsyncOptionSprite[0].getTexture()->getSize().x/2, vsyncOptionSprite[0].getTexture()->getSize().y / 2);
        vsyncOptionSprite[1].setTexture(*vsyncOptionTexture[1], true);
        vsyncOptionSprite[1].setOrigin(vsyncOptionSprite[1].getTexture()->getSize().x/2, vsyncOptionSprite[1].getTexture()->getSize().y / 2);
        vsyncOptionArrowLeftSprite[0].setTexture(*arrowLeftTexture[0], true);
        vsyncOptionArrowLeftSprite[0].setOrigin(-middleSpace, vsyncOptionArrowLeftSprite[0].getTexture()->getSize().y / 2);
        vsyncOptionArrowLeftSprite[1].setTexture(*arrowLeftTexture[1], true);
        vsyncOptionArrowLeftSprite[1].setOrigin(-middleSpace, vsyncOptionArrowLeftSprite[1].getTexture()->getSize().y / 2);
        vsyncOptionArrowRightSprite[0].setTexture(*arrowRightTexture[0], true);
        vsyncOptionArrowRightSprite[0].setOrigin(0, vsyncOptionArrowRightSprite[0].getTexture()->getSize().y / 2);
        vsyncOptionArrowRightSprite[1].setTexture(*arrowRightTexture[1], true);
        vsyncOptionArrowRightSprite[1].setOrigin(0, vsyncOptionArrowRightSprite[1].getTexture()->getSize().y / 2);
        vsyncOptionSprite[0].setPosition(1300, screenSize.y/2);
        vsyncOptionSprite[1].setPosition(1300, screenSize.y/2);
        vsyncOptionArrowLeftSprite[0].setPosition(screenSize.x/2, screenSize.y/2);
        vsyncOptionArrowLeftSprite[1].setPosition(screenSize.x/2, screenSize.y/2);
        vsyncOptionArrowRightSprite[0].setPosition(screenSize.x/2 + 600, screenSize.y/2);
        vsyncOptionArrowRightSprite[1].setPosition(screenSize.x/2 + 600, screenSize.y/2);

        musicArrowLeftSprite[0].setTexture(*arrowLeftTexture[0], true);
        musicArrowLeftSprite[0].setOrigin(-middleSpace, musicArrowLeftSprite[0].getTexture()->getSize().y / 2);
        musicArrowLeftSprite[1].setTexture(*arrowLeftTexture[1], true);
        musicArrowLeftSprite[1].setOrigin(-middleSpace, musicArrowLeftSprite[1].getTexture()->getSize().y / 2);
        musicArrowRightSprite[0].setTexture(*arrowRightTexture[0], true);
        musicArrowRightSprite[0].setOrigin(0, musicArrowRightSprite[0].getTexture()->getSize().y / 2);
        musicArrowRightSprite[1].setTexture(*arrowRightTexture[1], true);
        musicArrowRightSprite[1].setOrigin(0, musicArrowRightSprite[1].getTexture()->getSize().y / 2);

        sfxArrowLeftSprite[0].setTexture(*arrowLeftTexture[0], true);
        sfxArrowLeftSprite[0].setOrigin(-middleSpace, sfxArrowLeftSprite[0].getTexture()->getSize().y / 2);
        sfxArrowLeftSprite[1].setTexture(*arrowLeftTexture[1], true);
        sfxArrowLeftSprite[1].setOrigin(-middleSpace, sfxArrowLeftSprite[1].getTexture()->getSize().y / 2);
        sfxArrowRightSprite[0].setTexture(*arrowRightTexture[0], true);
        sfxArrowRightSprite[0].setOrigin(0, sfxArrowRightSprite[0].getTexture()->getSize().y / 2);
        sfxArrowRightSprite[1].setTexture(*arrowRightTexture[1], true);
        sfxArrowRightSprite[1].setOrigin(0, sfxArrowRightSprite[1].getTexture()->getSize().y / 2);

        musicArrowLeftSprite[0].setPosition(screenSize.x/2, screenSize.y/2 + rowSpace);
        musicArrowLeftSprite[1].setPosition(screenSize.x/2, screenSize.y/2 + rowSpace);
        musicArrowRightSprite[0].setPosition(screenSize.x/2 + 600, screenSize.y/2 + rowSpace);
        musicArrowRightSprite[1].setPosition(screenSize.x/2 + 600, screenSize.y/2 + rowSpace);

        sfxArrowLeftSprite[0].setPosition(screenSize.x/2, screenSize.y/2 + rowSpace*2);
        sfxArrowLeftSprite[1].setPosition(screenSize.x/2, screenSize.y/2 + rowSpace*2);
        sfxArrowRightSprite[0].setPosition(screenSize.x/2 + 600, screenSize.y/2 + rowSpace*2);
        sfxArrowRightSprite[1].setPosition(screenSize.x/2 + 600, screenSize.y/2 + rowSpace*2);


        constexpr int KERNING = 50;
        // music percentage display
        for (int i = 0; i < 3; i++) {
                musicPercentDigits[i].setTexture(*numbersTexture[0], true); // Default to 0
                musicPercentDigits[i].setOrigin(musicPercentDigits[i].getTexture()->getSize().x/2, musicPercentDigits[i].getTexture()->getSize().y/2);
                musicPercentDigits[i].setPosition(screenSize.x/2 + 300 + (i * KERNING), screenSize.y/2 + rowSpace);
        }

        musicPercentSymbol.setTexture(*percentTexture, true);
        musicPercentSymbol.setOrigin(musicPercentSymbol.getTexture()->getSize().x/2, musicPercentSymbol.getTexture()->getSize().y/2);
        musicPercentSymbol.setPosition(screenSize.x/2 + 450, screenSize.y/2 + rowSpace);

        // sfx percentage display
        for (int i = 0; i < 3; i++) {
                sfxPercentDigits[i].setTexture(*numbersTexture[0], true); // Default to 0
                sfxPercentDigits[i].setOrigin(sfxPercentDigits[i].getTexture()->getSize().x/2, sfxPercentDigits[i].getTexture()->getSize().y/2);
                sfxPercentDigits[i].setPosition(screenSize.x/2 + 300 + (i * KERNING), screenSize.y/2 + rowSpace*2);
        }

        sfxPercentSymbol.setTexture(*percentTexture, true);
        sfxPercentSymbol.setOrigin(sfxPercentSymbol.getTexture()->getSize().x/2, sfxPercentSymbol.getTexture()->getSize().y/2);
        sfxPercentSymbol.setPosition(screenSize.x/2 + 450, screenSize.y/2 + rowSpace*2);

}
void SettingsScene::OnScene_Deactivate() {}
void SettingsScene::DestroyScene() {}
void SettingsScene::InitializeScene() { IScene::InitializeScene(); }
