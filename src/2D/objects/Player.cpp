//
// Created by Paul McGinley on 30/09/2024.
//

#include "Player.h"
#include "scenes/GameScene.h"

#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include "managers/AssetManager.h"
#include "models/Projectiles/FireBall.h"

Player::Player(GameScene* game_scene) : gameScene(game_scene) {
        // Set the animation sequences for the player
        // Note to future self:
        // Past you was an absolute genius for the animation events, but let's not forget what they are for again xD
        // The jump ani has multiple parts to the animation based on the state of the jump
        // Once one part of the animation is complete, it can change the ani to the next part of the animation
        // .OnComplete [](){currentAni = AniType::JumpEnd;}
        sequences = {
                {AnimationType::ANIMATION_CONSUME, {0, 32, 100, nullptr /*[](){changeAnimation(AnimationType::ANIMATION_JUMP_AIR, nullptr, false);}*/, nullptr, nullptr}},
                {AnimationType::ANIMATION_DAMAGED, {32, 13, 100, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_IDLE);}, nullptr}},
                {AnimationType::ANIMATION_DEATH, {45, 11, 100, nullptr, [this](){isDead=true;}, nullptr}},
                {AnimationType::ANIMATION_DEAD, {55, 1, 1000, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_DIZZY, {56, 20, 100, nullptr, nullptr, nullptr}},
                {AnimationType::ANIMATION_FIRE, {76, 16, 100, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_IDLE);}, nullptr}},
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
        // Get current world position of collision box
        sf::FloatRect playerBox(
            position.x + collisionBox.left,
            position.y + collisionBox.top,
            collisionBox.width,
            collisionBox.height
        );



        // Store the previous position before applying velocity
        sf::FloatRect prevPlayerBox = playerBox;
        prevPlayerBox.left -= velocity.x * gametime.delta_time;
        prevPlayerBox.top -= velocity.y * gametime.delta_time;

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

                // Check if player is horizontally aligned with this boundary
                bool horizontalOverlap = (playerBox.left + playerBox.width > boundaryRect.left) &&
                                         (playerBox.left < boundaryRect.left + boundaryRect.width);

                if (horizontalOverlap) {
                        // Check for tunneling through - when feet were above platform in previous frame but below it now
                        float prevFeetY = prevPlayerBox.top + prevPlayerBox.height;
                        float currentFeetY = playerBox.top + playerBox.height;

                        if (prevFeetY <= boundaryRect.top && currentFeetY >= boundaryRect.top) {
                                // Player has tunneled through the platform - place them on top
                                onGround = true;
                                position.y = boundaryRect.top - collisionBox.top - collisionBox.height;
                                velocity.y = 0;
                                isFalling = false;
                                isJumping = false;
                                break;
                        }

                        // Regular ground detection
                        float feetY = playerBox.top + playerBox.height;
                        float groundThreshold = 2.0f;

                        if (feetY >= boundaryRect.top - groundThreshold && feetY <= boundaryRect.top + groundThreshold) {
                                onGround = true;
                                position.y = boundaryRect.top - collisionBox.top - collisionBox.height;
                                velocity.y = 0;
                                break;
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
                                isFalling = true;
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

                // Check for tunneling through walls
                else if (!playerBox.intersects(boundaryRect) && prevPlayerBox.intersects(boundaryRect)) {
                        // Detect which side was crossed
                        if (velocity.x > 0 && prevPlayerBox.left + prevPlayerBox.width <= boundaryRect.left + 2) {
                                position.x = boundaryRect.left - collisionBox.left - collisionBox.width;
                                velocity.x = 0;
                        }
                        else if (velocity.x < 0 && prevPlayerBox.left >= boundaryRect.left + boundaryRect.width - 2) {
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

        float shadowX = position.x +250 - asset_manager.TextureLibraries["PrgUse"]->entries[9].texture.getSize().x/2;

        if (!isFalling && !isJumping) {
                // player on ground
                float shadowY = position.y + collisionBox.top + collisionBox.height - 25;
                shadowDrawPosition = sf::Vector2f(shadowX, shadowY);
        } else {
                // player in ait
                float playerFeetX = shadowX + collisionBox.left + collisionBox.width / 2;
                float playerFeetY = position.y + collisionBox.top + collisionBox.height - 25;

                // check for ground collision
                std::optional<float> closestY; // optional allows us to check if a vlaue is valid
                for (const auto& boundary : boundaries) {
                        if (playerFeetX >= boundary.X && playerFeetX <= boundary.X + boundary.Width) {
                                if (boundary.Y >= playerFeetY) {
                                        if (!closestY || boundary.Y < *closestY) {
                                                closestY = boundary.Y - 25;
                                        }
                                }
                        }
                }

                if (closestY) { // we found a ground collision
                        shadowDrawPosition = sf::Vector2f(shadowX, *closestY);
                } else { // we didnt find a ground collision so send the shadow away
                        shadowDrawPosition = sf::Vector2f(shadowX, 9001);
                }
        }
}

void Player::Update(GameTime gameTime) {
        health.Update(gameTime);

        if (isDead) {
                ChangeAnimation(AnimationType::ANIMATION_DEAD);
                return;
        }

        if (health.getCurrentHealth() ==0 ) {
                ChangeAnimation(AnimationType::ANIMATION_DEATH, gameTime);
                UpdateQuads();
                return;
        }

        if (currentAnimation == AnimationType::ANIMATION_DAMAGED) {
                UpdateQuads();
                return;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5) && gameTime.TimeElapsed(nextMagicTime) && !isFalling && !isJumping) {

                ChangeAnimation(AnimationType::ANIMATION_FIRE, gameTime, true);
        }





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
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))&& !isFalling && !isJumping) {
                isJumping = true;
                velocity.y = -JUMPING_SPEED;
                ChangeAnimation(AnimationType::ANIMATION_JUMP_START, gameTime, true);
        }

        // Apply gravity if falling or jumping
        if (isFalling || isJumping) {
                acceleration.y += FALLING_SPEED;
                ChangeAnimation(AnimationType::ANIMATION_JUMP_UP, gameTime, true);
        }

        // Handle horizontal movement
        bool isMoving = false;
        float targetSpeed = 0.0f;
        float directionChangeMultiplier = 1.0f;
        const float runThreshold = WALKING_SPEED * 2.f; // Speed threshold to transition to running

        if (currentAnimation != AnimationType::ANIMATION_FIRE)
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                        faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
                        isMoving = true;
                        // Always target running speed, but will take time to reach it
                        targetSpeed = -RUNNING_SPEED;
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                        faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_GENERIC;
                        isMoving = true;
                        // Always target running speed, but will take time to reach it
                        targetSpeed = RUNNING_SPEED;
                }

        // Apply acceleration or deceleration to horizontal movement
        if (isMoving) {
                // Calculate acceleration based on target speed with a gradual ramp-up
                float accelRate = 800.0f;

                // Check if changing direction
                if ((velocity.x > 0 && targetSpeed < 0) || (velocity.x < 0 && targetSpeed > 0)) {
                        // Apply a multiplier to acceleration when changing direction to simulate inertia
                        directionChangeMultiplier = 2.5f;
                } else {
                        directionChangeMultiplier = 1.0f;
                        // If we're already moving, maintain a steady acceleration
                        if ((velocity.x > 0 && targetSpeed > 0) || (velocity.x < 0 && targetSpeed < 0)) {
                                // Slightly increase acceleration as speed increases to simulate momentum
                                accelRate += std::abs(velocity.x) * 0.5f;
                        }
                }

                acceleration.x = accelRate * directionChangeMultiplier * ((targetSpeed - velocity.x) / maxVelocity.x);
        } else {
                // Apply deceleration when not moving
                if (!isFalling && !isJumping) {
                        deceleration.x = 2000.0f;
                        if (velocity.x > 0) {
                                velocity.x = std::max(0.0f, velocity.x - deceleration.x * gameTime.delta_time);
                        } else if (velocity.x < 0) {
                                velocity.x = std::min(0.0f, velocity.x + deceleration.x * gameTime.delta_time);
                        }
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
                ChangeAnimation(AnimationType::ANIMATION_JUMP_UP, gameTime, true);
        } else if (isFalling) {
                ChangeAnimation(AnimationType::ANIMATION_JUMP_UP, gameTime, true);
        } else if (std::abs(velocity.x) > 0.1f) {
                // If speed is above runThreshold, switch to run animation
                if (std::abs(velocity.x) > runThreshold) {
                        ChangeAnimation(AnimationType::ANIMATION_RUN, gameTime, true);
                } else {
                        ChangeAnimation(AnimationType::ANIMATION_WALK, gameTime, true);
                }
        } else {
                if (currentAnimation != AnimationType::ANIMATION_FIRE)
                        ChangeAnimation(AnimationType::ANIMATION_IDLE, gameTime, true);
        }

        UpdateQuads();
}

// Update Robe and Staff texture quads (position, texture coordinates)
void Player::UpdateQuads() {
        TextureEntry &robe = *asset_manager.getRobeFrame_ptr(robeLibrary, getCurrentFrame());
        TextureEntry &staff = *asset_manager.getStaffFrame_ptr(staffLibrary, getCurrentFrame());

        for (int i = 0; i < 4; ++i) {
                robeQuad[i].texCoords = robe.texQuad[i].texCoords;
                robeQuad[i].position = robe.texQuad[i].position + position;

                staffQuad[i].texCoords = staff.texQuad[i].texCoords;
                staffQuad[i].position = staff.texQuad[i].position + position;
        }
}

void Player::LateUpdate(GameTime gameTime) {
        health.LateUpdate(gameTime);
        TickAnimation(gameTime);
}

void Player::Draw(sf::RenderWindow& window, GameTime gameTime) {

        // Draw shadow
        IDraw::Draw(window, "PrgUse", 9, shadowDrawPosition);

        // Draw Robe
        window.draw(
                robeQuad,
                &asset_manager.getRobeFrame_ptr(robeLibrary, getCurrentFrame())->texture
        );

        // Draw Staff
        window.draw(
                staffQuad,
                &asset_manager.getStaffFrame_ptr(staffLibrary, getCurrentFrame())->texture
        );

        // Draw collision box
        // sf::RectangleShape collisionBoxShape(sf::Vector2f(collisionBox.width, collisionBox.height));
        // collisionBoxShape.setPosition(collisionOffset());
        // collisionBoxShape.setFillColor(sf::Color(255, 0, 0, 100)); // Transparent fill
        // collisionBoxShape.setOutlineThickness(1);
        // collisionBoxShape.setOutlineColor(sf::Color::Red);
        // window.draw(collisionBoxShape);
}

void Player::TickAnimation(GameTime gameTime) {
        IAnimate::TickAnimation(gameTime);

        if (sequences[currentAnimation].onFrame != nullptr)
                sequences[currentAnimation].onFrame(currentAnimationFrame /*<< = _frame*/);

        if(currentAnimation == AnimationType::ANIMATION_FIRE && gameTime.TimeElapsed(nextMagicTime)) {
                if (currentAnimationFrame == 6) {
                        CastMagic(gameTime);
                }
        }
}
void Player::CastMagic(GameTime gameTime) {

        gameScene->AddProjectile(
            std::make_unique<FireBall>(
                position - sf::Vector2f(250, 200),
                sf::Vector2f(32, 0),
                1.0f,
                50.0f,
                10000.0f
            )
        );

        nextMagicTime = gameTime.NowAddMilliseconds(1000);
}

