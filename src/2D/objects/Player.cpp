//
// Created by Paul McGinley on 30/09/2024.
//

#include "Player.h"

#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include "managers/AssetManager.h"

Player::Player() {
        // Set the animation sequences for the player
        // Note to future self:
        // Past you was an absolute genius for the animation events, but let's not forget what they are for again xD
        // The jump ani has multiple parts to the animation based on the state of the jump
        // Once one part of the animation is complete, it can change the ani to the next part of the animation
        // .OnComplete [](){currentAni = AniType::JumpEnd;}
        sequences = {
                {AnimationType::ANIMATION_CONSUME, {0, 32, 100, nullptr /*[](){changeAnimation(AnimationType::ANIMATION_JUMP_AIR, nullptr, false);}*/, nullptr, nullptr}},
                {AnimationType::ANIMATION_DAMAGED, {32, 13, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_DEATH, {45, 11, 100 , nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_DIZZY, {56, 20, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_FIRE, {76, 16, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_IDLE, {92, 16, 100, nullptr, nullptr, nullptr}}, // Randomly play idle2
                {AnimationType::ANIMATION_IDLE2, {108, 64, 100, nullptr, nullptr, nullptr}}, // once played go back to idle
                {AnimationType::ANIMATION_INTERACT, {172, 21, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_AIR, {193, 14, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_END, {207, 10, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_START, {217, 8, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_UP, {225, 4, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_PICKUP, {229, 20, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_RUN, {249, 10, 80, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_WALK, {259, 16, 65, nullptr, nullptr, [](const int _frame){std::cout << "Walking frame: " << _frame << std::endl;}}} // Debug output
        };
        GameTime gameTime;
}

void Player::Update(GameTime gameTime) {


        // DEBUG
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
        // END DEBUG



        // Check if left or right key is pressed
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                faceDirection = FaceDirection::FACE_DIRECTION_LEFT;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    ChangeAnimation(AnimationType::ANIMATION_RUN, gameTime);
                    position.x -= RUNNING_SPEED * gameTime.delta_time;
                } else {
                    ChangeAnimation(AnimationType::ANIMATION_WALK, gameTime);
                    position.x -= WALKING_SPEED * gameTime.delta_time;
                }
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_GENERIC;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    ChangeAnimation(AnimationType::ANIMATION_RUN, gameTime);
                    position.x += RUNNING_SPEED * gameTime.delta_time;
                } else {
                    ChangeAnimation(AnimationType::ANIMATION_WALK, gameTime);
                    position.x += WALKING_SPEED * gameTime.delta_time;
                }
        }

        else {
                ChangeAnimation(AnimationType::ANIMATION_IDLE2, gameTime);
        }

        TextureEntry& robe = *asset_manager.getRobeFrame_ptr(robeLibrary, getCurrentFrame());
        robeQuad[0].texCoords = robe.texQuad[0].texCoords;
        robeQuad[1].texCoords = robe.texQuad[1].texCoords;
        robeQuad[2].texCoords = robe.texQuad[2].texCoords;
        robeQuad[3].texCoords = robe.texQuad[3].texCoords;

        robeQuad[0].position = robe.texQuad[0].position + position;
        robeQuad[1].position = robe.texQuad[1].position + position;
        robeQuad[2].position = robe.texQuad[2].position + position;
        robeQuad[3].position = robe.texQuad[3].position + position;

        TextureEntry& staff = *asset_manager.getStaffFrame_ptr(staffLibrary, getCurrentFrame());
        staffQuad[0].texCoords = staff.texQuad[0].texCoords;
        staffQuad[1].texCoords = staff.texQuad[1].texCoords;
        staffQuad[2].texCoords = staff.texQuad[2].texCoords;
        staffQuad[3].texCoords = staff.texQuad[3].texCoords;

        staffQuad[0].position = staff.texQuad[0].position + position;
        staffQuad[1].position = staff.texQuad[1].position + position;
        staffQuad[2].position = staff.texQuad[2].position + position;
        staffQuad[3].position = staff.texQuad[3].position + position;

        health.Update(gameTime);
}

void Player::LateUpdate(GameTime gameTime) {

        health.LateUpdate(gameTime);
        TickAnimation(gameTime);
}

void Player::Draw(sf::RenderWindow& window, GameTime gameTime) {

        window.draw(
                robeQuad,
                &asset_manager.getRobeFrame_ptr(robeLibrary, getCurrentFrame())->texture
        );

        window.draw(
                staffQuad,
                &asset_manager.getStaffFrame_ptr(staffLibrary, getCurrentFrame())->texture
        );
}

void Player::TickAnimation(GameTime gameTime) {
        IAnimate::TickAnimation(gameTime);

        if (sequences[currentAnimation].onFrame != nullptr)
                sequences[currentAnimation].onFrame(currentAnimationFrame);
}

