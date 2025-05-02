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
                {AnimationType::ANIMATION_CONSUME,
                 {0, 32, 100, nullptr /*[](){changeAnimation(AnimationType::ANIMATION_JUMP_AIR, nullptr, false);}*/, nullptr, nullptr}},
                {AnimationType::ANIMATION_DAMAGED, {32, 13, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_DEATH, {45, 11, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_DIZZY, {56, 20, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_FIRE, {76, 16, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_IDLE, {92, 16, 100, nullptr, nullptr, nullptr}}, // Randomly play idle2
                {AnimationType::ANIMATION_IDLE2, {108, 64, 100, nullptr, nullptr, nullptr}}, // once played go back to idle
                {AnimationType::ANIMATION_INTERACT, {172, 21, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_AIR, {193, 14, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_END, {207, 10, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_START, {217, 8, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_JUMP_UP, {225, 4, 100, nullptr, [this]() { ChangeAnimation(AnimationType::ANIMATION_JUMP_AIR); }, nullptr}},
                {AnimationType::ANIMATION_PICKUP, {229, 20, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_RUN, {249, 10, 80, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_WALK, {259, 16, 65, nullptr, nullptr, nullptr /*[](const int _frame){std::cout << "Walking frame: " << _frame << std::endl;}*/}} // Debug output
        };
}

void Player::CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gametime) {
        // Get world position of collision box
        sf::FloatRect playerBox(
            position.x + collisionBox.left,
            position.y + collisionBox.top,
            collisionBox.width,
            collisionBox.height
        );

        // Calculate predicted next position based on velocity
        sf::FloatRect nextPlayerBox = playerBox;
        nextPlayerBox.left += velocity.x * gametime.delta_time;
        nextPlayerBox.top += velocity.y * gametime.delta_time;

        bool onGround = false;

        // Check for ground collisions
        for (const auto& boundary : boundaries) {
                sf::FloatRect boundaryRect(
                    boundary.X,
                    boundary.Y,
                    boundary.Width,
                    boundary.Height
                );

                // Check if standing on this boundary
                if (((playerBox.left + playerBox.width) > boundaryRect.left) && (playerBox.left < (boundaryRect.left + boundaryRect.width))) {

                        // Check for feet touching or slightly above the boundary (within a small threshold)
                        float feetY = playerBox.top + playerBox.height;
                        float groundThreshold = 2.0f; // Small threshold to avoid floating issues

                        if (feetY >= boundaryRect.top - groundThreshold && feetY <= boundaryRect.top + groundThreshold) {
                                onGround = true;
                                position.y = boundaryRect.top - collisionBox.top - collisionBox.height;
                                velocity.y = 0;
                                break; // Landed
                            }
                    }
        }

        // Update falling/jumping state based on ground detection
        if (onGround) {
                isFalling = false;
                isJumping = false;
        } else if (velocity.y >= 0 && !isJumping) {
                isFalling = true;
        }

        // Handle wall collisions
        for (const auto& boundary : boundaries) {
                sf::FloatRect boundaryRect(
                    boundary.X,
                    boundary.Y,
                    boundary.Width,
                    boundary.Height
                );

                // Update player box after any position changes
                playerBox.left = position.x + collisionBox.left;
                playerBox.top = position.y + collisionBox.top;

                // Check if player is intersecting with a boundary
                if (playerBox.intersects(boundaryRect)) {
                        // Calculate overlap
                        float overlapLeft = (playerBox.left + playerBox.width) - boundaryRect.left;
                        float overlapRight = (boundaryRect.left + boundaryRect.width) - playerBox.left;
                        float overlapTop = (playerBox.top + playerBox.height) - boundaryRect.top;
                        float overlapBottom = (boundaryRect.top + boundaryRect.height) - playerBox.top;

                        // Find the minimum overlap and resolve accordingly
                        float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});

                        if (minOverlap == overlapTop && velocity.y >= 0) {
                                // Push up (landing)
                                position.y = boundaryRect.top - collisionBox.top - collisionBox.height;
                                velocity.y = 0;
                                isFalling = false;
                                isJumping = false;
                                onGround = true;
                        }
                        else if (minOverlap == overlapBottom && velocity.y <= 0) {
                                // Push down (hitting head)
                                position.y = boundaryRect.top + boundaryRect.height - collisionBox.top;
                                velocity.y = 0;
                                isJumping = false;
                        }
                        else if (minOverlap == overlapLeft && velocity.x >= 0) {
                                // Push left
                                position.x = boundaryRect.left - collisionBox.left - collisionBox.width;
                                velocity.x = 0;
                        }
                        else if (minOverlap == overlapRight && velocity.x <= 0) {
                                // Push right
                                position.x = boundaryRect.left + boundaryRect.width - collisionBox.left;
                                velocity.x = 0;
                        }
                }

                // Check for predicted collisions (for next frame)
                else if (nextPlayerBox.intersects(boundaryRect)) {
                        // Moving right and hitting left edge of boundary
                        if (velocity.x > 0 && (playerBox.left + playerBox.width) <= boundaryRect.left) {
                                position.x = boundaryRect.left - collisionBox.left - collisionBox.width;
                                velocity.x = 0;
                        }
                        // Moving left and hitting right edge of boundary
                        else if (velocity.x < 0 && playerBox.left >= (boundaryRect.left + boundaryRect.width)) {
                                position.x = boundaryRect.left + boundaryRect.width - collisionBox.left;
                                velocity.x = 0;
                        }
                        // Moving up and hitting bottom edge of boundary
                        else if (velocity.y < 0 && playerBox.top >= (boundaryRect.top + boundaryRect.height)) {
                                position.y = boundaryRect.top + boundaryRect.height - collisionBox.top;
                                velocity.y = 0;
                                isJumping = false;
                        }
                }
        }
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

        // Reset acceleration
        acceleration = {0, 0};

        // Handle jumping input (actual physics applied in CalculatePhysicsState)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isFalling && !isJumping) {
                isJumping = true;
                velocity.y = -JUMPING_SPEED;
                ChangeAnimation(AnimationType::ANIMATION_JUMP_START, gameTime, true);
        }

        // Apply gravity if falling or jumping
        if (isFalling || isJumping) {
                acceleration.y += FALLING_SPEED;
        }

        // Handle horizontal movement
        bool isMoving = false;
        float targetSpeed = 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
                isMoving = true;
                targetSpeed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !isFalling && !isJumping)
                                              ? -RUNNING_SPEED
                                              : -WALKING_SPEED;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_GENERIC;
                isMoving = true;
                targetSpeed = (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !isFalling && !isJumping)
                                              ? RUNNING_SPEED
                                              : WALKING_SPEED;
        }

        // Apply acceleration or deceleration to horizontal movement
        if (isMoving) {
                // Calculate acceleration based on target speed
                const float accelRate = 800.0f;
                acceleration.x = accelRate * ((targetSpeed - velocity.x) / maxVelocity.x);
        } else {
                // Apply deceleration when not moving
                deceleration.x = 1500.0f;
                if (velocity.x > 0) {
                        velocity.x = std::max(0.0f, velocity.x - deceleration.x * gameTime.delta_time);
                } else if (velocity.x < 0) {
                        velocity.x = std::min(0.0f, velocity.x + deceleration.x * gameTime.delta_time);
                }
        }

        // Update velocity based on acceleration
        velocity.x += acceleration.x * gameTime.delta_time;
        velocity.y += acceleration.y * gameTime.delta_time;

        // Clamp velocity to maximum
        velocity.x = std::max(-maxVelocity.x, std::min(velocity.x, maxVelocity.x));
        velocity.y = std::max(-maxVelocity.y, std::min(velocity.y, maxVelocity.y));

        // Update position based on velocity
        position.x += velocity.x * gameTime.delta_time;
        position.y += velocity.y * gameTime.delta_time;

        // Update animation based on movement state
        if (isJumping && velocity.y < 0) {
                ChangeAnimation(AnimationType::ANIMATION_JUMP_UP, gameTime, false);
        } else if (isFalling) {
                ChangeAnimation(AnimationType::ANIMATION_JUMP_UP, gameTime, false);
        } else if (std::abs(velocity.x) > 0.1f) {
                if (std::abs(velocity.x) > WALKING_SPEED + 50) {
                        ChangeAnimation(AnimationType::ANIMATION_RUN, gameTime);
                } else {
                        ChangeAnimation(AnimationType::ANIMATION_WALK, gameTime);
                }
        } else {
                ChangeAnimation(AnimationType::ANIMATION_IDLE, gameTime);
        }


        UpdateQuads();
        health.Update(gameTime);
}

// Update Robe and Staff texture quads (position, texture coordinates)
void Player::UpdateQuads() {
        // Update texture quads
        TextureEntry &robe = *asset_manager.getRobeFrame_ptr(robeLibrary, getCurrentFrame());
        robeQuad[0].texCoords = robe.texQuad[0].texCoords;
        robeQuad[1].texCoords = robe.texQuad[1].texCoords;
        robeQuad[2].texCoords = robe.texQuad[2].texCoords;
        robeQuad[3].texCoords = robe.texQuad[3].texCoords;

        robeQuad[0].position = robe.texQuad[0].position + position;
        robeQuad[1].position = robe.texQuad[1].position + position;
        robeQuad[2].position = robe.texQuad[2].position + position;
        robeQuad[3].position = robe.texQuad[3].position + position;

        TextureEntry &staff = *asset_manager.getStaffFrame_ptr(staffLibrary, getCurrentFrame());
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

        // Draw collision box
        sf::RectangleShape collisionBoxShape(sf::Vector2f(collisionBox.width, collisionBox.height));
        collisionBoxShape.setPosition(collisionOffset());
        collisionBoxShape.setFillColor(sf::Color(255, 0, 0, 100)); // Transparent fill
        collisionBoxShape.setOutlineThickness(1);
        collisionBoxShape.setOutlineColor(sf::Color::Red);
        window.draw(collisionBoxShape);
}

void Player::TickAnimation(GameTime gameTime) {
        IAnimate::TickAnimation(gameTime);

        if (sequences[currentAnimation].onFrame != nullptr)
                sequences[currentAnimation].onFrame(currentAnimationFrame /*<< = _frame*/);
}

