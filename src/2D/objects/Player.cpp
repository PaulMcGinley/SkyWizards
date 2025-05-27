//
// Created by Paul McGinley on 30/09/2024.
//

#include "Player.h"
#include "scenes/GameScene.h"

#include <iostream>
#include "managers/AssetManager.h"
#include "models/Projectiles/FireBall.h"
#include <math.h>

Player::Player(GameScene* game_scene) : gameScene(game_scene) {
        // Set the animation sequences for the player
        // Note to future self:
        // Past you was an absolute genius for the animation events, but let's not forget what they are for again xD
        // The jump ani has multiple parts to the animation based on the state of the jump
        // Once one part of the animation is complete, it can change the ani to the next part of the animation
        // .OnComplete [](){currentAni = AniType::JumpEnd;}
        SetAnimationSequences({
                {AnimationType::ANIMATION_CONSUME, {0, 32, 100, nullptr /*[](){changeAnimation(AnimationType::ANIMATION_JUMP_AIR, nullptr, false);}*/, nullptr, nullptr}},
                {AnimationType::ANIMATION_DAMAGED, {32, 13, 60, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_IDLE, true);}, nullptr}},
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
        });
}

void Player::CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) {
        // Get current world position of collision box
        sf::FloatRect playerBox(
            position.x + collisionBox.left,
            position.y + collisionBox.top,
            collisionBox.width,
            collisionBox.height
        );

        // Store the previous position before applying velocity
        sf::FloatRect prevPlayerBox = playerBox;
        prevPlayerBox.left -= velocity.x * gameTime.deltaTime;
        prevPlayerBox.top -= velocity.y * gameTime.deltaTime;

        // Calculate predicted next position based on velocity
        sf::FloatRect nextPlayerBox = playerBox;
        nextPlayerBox.left += velocity.x * gameTime.deltaTime;
        nextPlayerBox.top += velocity.y * gameTime.deltaTime;

        bool onGround = false;
        bool wasInAir = isFalling || isJumping; // Track if player was in air before collision checks


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
                if (wasInAir) {
                        std::string landSFX = oddStepSound ? "Wizard/Land/land-1" : "Wizard/Land/land-2";
                        assetManager.PlaySoundEffect(landSFX, 20.f, 1.f);
                        oddStepSound = !oddStepSound; // Toggle for next time
                }

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

        float shadowX = (position.x +250) - (assetManager.TextureLibraries["PrgUse"]->entries[9].texture.getSize().x/2);

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
        // Always update the health first
        health.Update(gameTime);

        // If dead, play the static frame sequence
        if (isDead) {
                ChangeAnimation(AnimationType::ANIMATION_DEAD);
                return;
        }

        // If health has just dropped to 0, play the death animation
        // Once animation is complete it switches to ANIMATION_DEAD
        // which cannot have its animation overridden by ANIMATION_DEATH
        if (health.GetTargetHealth() == 0 ) {
                ChangeAnimation(AnimationType::ANIMATION_DEATH, gameTime, true);
                UpdateQuads();
                return;
        }

        if (GetCurrentAnimation() == AnimationType::ANIMATION_DAMAGED) {
                UpdateQuads();
                return;
        }

        if (inputManager.IsFirePressed() && gameTime.TimeElapsed(nextMagicTime) && !isFalling && !isJumping) {
                ChangeAnimation(AnimationType::ANIMATION_FIRE, gameTime, true);
        }

        // Reset acceleration
        acceleration = {0, 0};

        // Handle jumping input (actual physics applied in CalculatePhysicsState)
        if (inputManager.IsJumpPressed() && !isFalling && !isJumping) {
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

        if (GetCurrentAnimation() != AnimationType::ANIMATION_FIRE) { // Prevent movement during fire animation
                if (inputManager.MoveLeftPressed()) {
                        faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
                        isMoving = true;
                        // Always target running speed, but will take time to reach it
                        targetSpeed = -RUNNING_SPEED;

                        if (gameTime.TimeElapsed(nextStepSoundTime) && !isJumping && !isFalling) {
                                nextStepSoundTime = gameTime.NowAddMilliseconds(350);
                                assetManager.PlaySoundEffect("Wizard/step", 10.f, (oddStepSound ? 1.f : 0.9f));
                                oddStepSound = !oddStepSound; // Toggle sound variation
                        }
                } else if (inputManager.MoveRightPressed() ) {
                        faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_PLAYER;
                        isMoving = true;
                        // Always target running speed, but will take time to reach it
                        targetSpeed = RUNNING_SPEED;

                        if (gameTime.TimeElapsed(nextStepSoundTime) && !isJumping && !isFalling) {
                                nextStepSoundTime = gameTime.NowAddMilliseconds(350);
                                assetManager.PlaySoundEffect("Wizard/step", 10.f, (oddStepSound ? 1.f : 0.9f));
                                oddStepSound = !oddStepSound; // Toggle sound variation
                        }
                }
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
                                velocity.x = std::max(0.0f, velocity.x - deceleration.x * gameTime.deltaTime);
                        } else if (velocity.x < 0) {
                                velocity.x = std::min(0.0f, velocity.x + deceleration.x * gameTime.deltaTime);
                        }
                }
        }

        // Update velocity based on acceleration
        velocity.x += acceleration.x * gameTime.deltaTime;
        velocity.y += acceleration.y * gameTime.deltaTime;

        // Clamp velocity to maximum
        velocity.x = std::max(-maxVelocity.x, std::min(velocity.x, maxVelocity.x));
        velocity.y = std::max(-maxVelocity.y, std::min(velocity.y, maxVelocity.y));

        // Update position based on velocity
        position.x += velocity.x * gameTime.deltaTime;
        position.y += velocity.y * gameTime.deltaTime;

        // clamp position to int
        position.x = std::round(position.x);
        position.y = std::round(position.y);
        shadowDrawPosition.x = (position.x +250) - (assetManager.TextureLibraries["PrgUse"]->entries[9].texture.getSize().x/2);

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
                if (GetCurrentAnimation() != AnimationType::ANIMATION_FIRE)
                        ChangeAnimation(AnimationType::ANIMATION_IDLE, gameTime, true);
        }

        UpdateQuads();
}

// Update Robe and Staff texture quads (position, texture coordinates)
void Player::UpdateQuads() {
        TextureEntry &robe = assetManager.TextureLibraries[robeLibrary]->entries[GetTextureDrawIndex() + faceDirection];
        TextureEntry &staff = assetManager.TextureLibraries[staffLibrary]->entries[GetTextureDrawIndex() + faceDirection];

        for (int i = 0; i < 4; ++i) {
                robeQuad[i].texCoords = robe.texQuad[i].texCoords;
                robeQuad[i].position = robe.texQuad[i].position + position;

                staffQuad[i].texCoords = staff.texQuad[i].texCoords;
                staffQuad[i].position = staff.texQuad[i].position + position;
        }
}

void Player::LateUpdate(const GameTime gameTime) {
        health.LateUpdate(gameTime);
        TickAnimation(gameTime);
}

void Player::Draw(sf::RenderWindow& window, GameTime gameTime) {
        if (!visible) return;

        // Draw shadow
        IDraw::Draw(window, "PrgUse", 9, shadowDrawPosition);

        // Get the current animation frame for robe and staff
        TextureEntry &robe = assetManager.TextureLibraries[robeLibrary]->entries[DrawFrame()];
        TextureEntry &staff = assetManager.TextureLibraries[staffLibrary]->entries[DrawFrame()];

        // Draw Robe and Staff
        window.draw(robeQuad, &robe.texture);
        window.draw(staffQuad, &staff.texture);

        // Debug: Draw collision box
        if (GameManager::GetInstance().ShowCollisions()) {
                sf::RectangleShape collisionBoxShape(sf::Vector2f(collisionBox.width, collisionBox.height));
                collisionBoxShape.setPosition(collisionOffset());
                collisionBoxShape.setFillColor(sf::Color(255, 0, 0, 100)); // Transparent fill
                collisionBoxShape.setOutlineThickness(1);
                collisionBoxShape.setOutlineColor(sf::Color::Red);
                window.draw(collisionBoxShape);
        }
}

void Player::TickAnimation(GameTime gameTime) {
        IAnimate::TickAnimation(gameTime);

        if (GetCurrentAnimationSequence().Sequence_OnFrameTick != nullptr)
                GetCurrentAnimationSequence().Sequence_OnFrameTick();

        if (GetCurrentAnimation() == AnimationType::ANIMATION_FIRE && gameTime.TimeElapsed(nextMagicTime) &&
            GetCurrentAnimationFrame() == 6) {
                CastMagic(gameTime);
        }
}
void Player::IncrementCoins(const int amount = 1) { coins += amount; }
void Player::UpdateScore(const std::string &levelName, const int score) { scores[levelName] += score; }
int Player::GetScore(const std::string &levelName) { return scores[levelName]; }
int Player::GetTotalScore() {
        int totalScore = 0;
        for (const auto &[level, score]: scores) {
                totalScore += score;
        }
        return totalScore;
}
void Player::BounceUp(float amount) {
        velocity.y = -amount;
        position.y -= 5.f; // Just to get player off ground
        isJumping = true;
        isFalling = false;
        ChangeAnimation(AnimationType::ANIMATION_JUMP_START, GameTime(), true);
}
void Player::TakeDamage(int amount) {
        velocity = {0,0};
        health.Damage(amount);
        ChangeAnimation(AnimationType::ANIMATION_DAMAGED, true);
}

void Player::CastMagic(const GameTime gameTime) {
        sf::Vector2f projectileVelocity = {32, 0};
        if (faceDirection == FaceDirection::FACE_DIRECTION_LEFT) {
                projectileVelocity.x *= -1.f;
        }

        sf::Vector2f projectileSpawnPosition = position - sf::Vector2f(0, 200);
        if (faceDirection == FaceDirection::FACE_DIRECTION_LEFT) {
                projectileSpawnPosition.x += 50;
        } else if (faceDirection == FaceDirection::FACE_DIRECTION_RIGHT_PLAYER) {
                projectileSpawnPosition.x -= 350;
        }

        gameScene->AddProjectile(
            std::make_unique<FireBall>(
                projectileSpawnPosition,
                projectileVelocity,
                1.0f,
                50.0f,
                2000.0f,
                20.f
            )
        );

        nextMagicTime = gameTime.NowAddMilliseconds(1000);
        assetManager.PlaySoundEffect("Wizard/Magic/fire-cast", 50.f, 3.f); // Play fireball cast sound
}

