//
// Created by Paul McGinley on 30/09/2024.
//

#include "Player.h"

#include <iostream>
#include <SFML/Window/Keyboard.hpp>

#include "managers/AssetManager.cpp"


Player::Player() {
    // Set the animation sequences for the player
    sequences = {
        {AniType::Consume, {0, 32, 100, nullptr, nullptr}},
        {AniType::Damaged, {32, 13, 100, nullptr, nullptr}},
        {AniType::Death, {45, 11, 100 , nullptr, nullptr}},
        {AniType::Dizzy, {56, 20, 100, nullptr, nullptr}},
        {AniType::Fire, {76, 16, 100, nullptr, nullptr}},
        {AniType::Idle, {92, 16, 100, nullptr, nullptr}},
        {AniType::Idle2, {108, 64, 100, nullptr, nullptr}},
        {AniType::Interact, {172, 21, 100, nullptr, nullptr}},
        {AniType::JumpAir, {193, 14, 100, nullptr, nullptr}},
        {AniType::JumpEnd, {207, 10, 100, nullptr, nullptr}},
        {AniType::JumpStart, {217, 8, 100, nullptr, nullptr}},
        {AniType::JumpUp, {225, 4, 100, nullptr, nullptr}},
        {AniType::Pickup, {229, 20, 100, nullptr, nullptr}},
        {AniType::Run, {249, 10, 80, nullptr, nullptr}},
        {AniType::Walk, {259, 16, 65, nullptr, [&](int _frame){std::cout << "Walking frame: " << _frame << std::endl;}}}
    };
}

void Player::Update(GameTime gameTime) {

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
        robeLibrary = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
        robeLibrary = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
        robeLibrary = 2;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        staffLibrary = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        staffLibrary = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        staffLibrary = 2;

    // Check if left or right key is pressed
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        faceDirection = FaceDirection::Left;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            ChangeAni(AniType::Run, gameTime);
            position.x -= 0.1f;
        } else {
            ChangeAni(AniType::Walk, gameTime);
            position.x -= 0.06f;
        }
    }

    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        faceDirection = FaceDirection::Right;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            ChangeAni(AniType::Run, gameTime);
            position.x += 0.1f;
        } else {
            ChangeAni(AniType::Walk, gameTime);
            position.x += 0.04f;
        }
    }

    else {
       ChangeAni(AniType::Idle2, gameTime);
    }



    //AssetManager& assetManager = AssetManager::getInstance();
    // Update implementation
    TextureEntry& robe = *assetManager.GetRobeFrame_ptr(robeLibrary, frame());
    robeQuad[0].texCoords = robe.texQuad[0].texCoords;
    robeQuad[1].texCoords = robe.texQuad[1].texCoords;
    robeQuad[2].texCoords = robe.texQuad[2].texCoords;
    robeQuad[3].texCoords = robe.texQuad[3].texCoords;

    robeQuad[0].position = robe.texQuad[0].position + position;
    robeQuad[1].position = robe.texQuad[1].position + position;
    robeQuad[2].position = robe.texQuad[2].position + position;
    robeQuad[3].position = robe.texQuad[3].position + position;

    TextureEntry& staff = *assetManager.GetStaffFrame_ptr(staffLibrary, frame());
    staffQuad[0].texCoords = staff.texQuad[0].texCoords;
    staffQuad[1].texCoords = staff.texQuad[1].texCoords;
    staffQuad[2].texCoords = staff.texQuad[2].texCoords;
    staffQuad[3].texCoords = staff.texQuad[3].texCoords;

    staffQuad[0].position = staff.texQuad[0].position + position;
    staffQuad[1].position = staff.texQuad[1].position + position;
    staffQuad[2].position = staff.texQuad[2].position + position;
    staffQuad[3].position = staff.texQuad[3].position + position;

}

void Player::LateUpdate(GameTime gameTime) {

    TickAnimation(gameTime);
}

void Player::Draw(sf::RenderWindow& window, GameTime gameTime) {
    // Draw implementation
    window.draw(
        robeQuad,
        &assetManager.GetRobeFrame_ptr(robeLibrary, frame())->texture
        );

    window.draw(
        staffQuad,
        &assetManager.GetStaffFrame_ptr(staffLibrary, frame())->texture
        );
}

void Player::TickAnimation(GameTime gameTime) {
    IAnimate::TickAnimation(gameTime);

    if (sequences[currentAni].OnFrame != nullptr)
        sequences[currentAni].OnFrame(currentFrame);

}

