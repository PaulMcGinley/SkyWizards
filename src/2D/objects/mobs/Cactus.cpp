//
// Created by Paul McGinley on 22/05/2025.
//

#include "Cactus.h"
#include <SFML/Graphics/CircleShape.hpp>

#include "objects/Player.h"
Cactus::Cactus(Player *player, sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(player, spawnPosition, viewRange, moveSpeed, health)
        , canMoveLeft(false)
        , canMoveRight(false)
        , onGround(false)
        , nextCryTime(0)
        , nextAttackTime(0) {
        // Define the animation sequences for the Cactus
        SetAnimationSequences( {
                {AnimationType::ANIMATION_ATTACK, {0, 8, 90}},
                {AnimationType::ANIMATION_ATTACK2, {9, 10, 90,}},
                {AnimationType::ANIMATION_BATTLE_IDLE, {62, 10, 100}},
                {AnimationType::ANIMATION_DAMAGED, {44, 9, 100, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_IDLE);},nullptr}},
                {AnimationType::ANIMATION_DEATH, {19, 9, 100, [this](){assetManager.PlaySoundEffect("Cactus/die",100.f,1.f);}, [this](){ChangeAnimation(AnimationType::ANIMATION_DEAD);},nullptr}},
                {AnimationType::ANIMATION_DEAD, {27, 1, 1000}},
                {AnimationType::ANIMATION_DIZZY, {28, 16, 100}},
                {AnimationType::ANIMATION_IDLE, {60, 16, 50}},
                {AnimationType::ANIMATION_BATTLE_IDLE, {53, 7, 50}},
                {AnimationType::ANIMATION_IDLE2, {76, 8, 50}}, //PLANT
                {AnimationType::ANIMATION_STATIC, {60, 1, 100}},
                {AnimationType::ANIMATION_RUN, {92, 6, 80}},
                {AnimationType::ANIMATION_SENSE_SOMETHING, {98, 48, 40}},
                {AnimationType::ANIMATION_SENSE_SOMETHING_START, {146, 9, 40}},
                {AnimationType::ANIMATION_TAUNT, {155, 28, 100}},
                {AnimationType::ANIMATION_VICTORY, {183, 48, 80}},
                {AnimationType::ANIMATION_WALK, {231, 12, 80}}
        });

        // Set the size of the collision box
        collisionBox.width = 100;
        collisionBox.height = 90;

        score = 750;
}
void Cactus::Update(GameTime gameTime) {
                // Update quads in being damaged but perform no other logic
        if (IsDead() || GetCurrentAnimation() == AnimationType::ANIMATION_DAMAGED) {
                UpdateQuads();
                return;
        }

        // Get a reference to the player to save on dereferencing calls
        Player const & player = *this->player;

        // Change animation to the static sequence if the player is dead.
        if (player.GetIsDead()) {
                ChangeAnimation(AnimationType::ANIMATION_STATIC, gameTime, false);
                UpdateQuads(); // Update the texture quads and perform no other logic
                return;
        }

        faceDirection = (player.position.x < position.x) ? FaceDirection::FACE_DIRECTION_LEFT : FaceDirection::FACE_DIRECTION_RIGHT_CACTUS;

        // Euclidean distance calculation
        // Ref: https://study.com/academy/lesson/euclidean-distance-calculation-formula-examples.html#:~:text=The%20formula%20for%20Euclidean%20distance,coordinates%20of%20the%20two%20points.
        // Using multiplication instead of pow for performance but yields the same result
        const float distanceX = player.position.x - position.x;
        const float distanceY = player.position.y - position.y;
        const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        // State Logic
        if (distance < viewRange && GetCurrentAnimation() != AnimationType::ANIMATION_ATTACK) {
                // Player is within CQB range
                ChangeAnimation(AnimationType::ANIMATION_RUN, gameTime, true); // Bite
        } else {
                // Player is out of view range
                ChangeAnimation(AnimationType::ANIMATION_IDLE, gameTime, false);
        }

        if (distance <= viewRange && distance > 100 && gameTime.TimeElapsed(nextCryTime)) {
                int index = static_cast<int>(player.position.x);
                index = (index % 2) +1;
                // Play a sound effect when the cactus is close to the player
                assetManager.PlaySoundEffect("Cactus/cry" + std::to_string(index), 100.f, 1.f);
                nextCryTime = gameTime.NowAddMilliseconds(4500); // Reset cry timer
        }

        if (distance <= 150 && gameTime.TimeElapsed(nextAttackTime) && GetCurrentAnimation() != AnimationType::ANIMATION_ATTACK) {
                assetManager.PlaySoundEffect("Cactus/attack", 100.f, 1.f);
                ChangeAnimation(AnimationType::ANIMATION_ATTACK, gameTime, true);
        }

        // Movement Logic
        if (GetCurrentAnimation() == AnimationType::ANIMATION_RUN) {
                // Calculate the distance to move based on the speed and delta time
                const float moveDistance = walkSpeed * gameTime.deltaTime;

                if (faceDirection == FaceDirection::FACE_DIRECTION_LEFT && canMoveLeft) {
                        // Move Left
                        position.x -= moveDistance;
                } else if (faceDirection == FaceDirection::FACE_DIRECTION_RIGHT_CACTUS && canMoveRight) {
                        // Move Right
                        position.x += moveDistance;
                } else {
                        // Return to idle if no movement is possible
                        ChangeAnimation(AnimationType::ANIMATION_IDLE);
                }
        }

        // Update collision box position factoring the sprite offset (250)
        collisionBox.left = position.x + 250 - (collisionBox.width / 2);
        collisionBox.top = position.y + 250 - (collisionBox.height / 2) + 80; // +40 for better alignment

        UpdateQuads();
}
void Cactus::LateUpdate(GameTime gameTime) { TickAnimation(gameTime);}
void Cactus::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Draw Shadow
        float shadowX = collisionBox.left + (collisionBox.width/2) - assetManager.TextureLibraries["PrgUse"]->entries[9].texture.getSize().x / 2;
        float shadowY = position.y + 350; // +310 for better alignment
        IDraw::Draw(window, "PrgUse", 9, sf::Vector2f(shadowX, shadowY));

        // Draw Monster
        window.draw(texQuads, &assetManager.TextureLibraries["Cactus"]->entries[GetTextureDrawIndex() + faceDirection].texture);

        if (GameManager::GetInstance().ShowCollisions()) {
                // Collision box
                sf::RectangleShape rect(sf::Vector2f(collisionBox.width, collisionBox.height));
                rect.setPosition(collisionBox.left, collisionBox.top);
                rect.setFillColor(sf::Color(0, 0, 255, 128));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(1.0f);
                window.draw(rect);

                // DEBUG: Draw detector circles
                sf::CircleShape leftDropDetector(5);
                leftDropDetector.setFillColor(sf::Color::Green);
                leftDropDetector.setPosition(GetLeftDropDetectorPosition() - sf::Vector2f({3,0}));
                window.draw(leftDropDetector);
                sf::CircleShape rightDropDetector(5);
                rightDropDetector.setFillColor(sf::Color::Red);
                rightDropDetector.setPosition(GetRightDropDetectorPosition()- sf::Vector2f({3,0}));
                window.draw(rightDropDetector);
        }
}
void Cactus::CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) {
        // If not on ground, check for collisions with boundaries
        // We only need this to place the monster on the ground
        if (!onGround) {
                // Create a temporary elongated collision box that extends downward
                // This is to test for collisions beneath the monster
                sf::FloatRect tempCollisionBox = collisionBox;
                tempCollisionBox.height = 1000; // Extend downward by 1000 pixels

                // Find all boundaries that the temp collision box intersects with
                std::vector<Boundary*> intersectingBoundaries;
                for (auto& boundary : boundaries) {
                        sf::FloatRect boundaryRect(boundary.X, boundary.Y, boundary.Width, boundary.Height);
                        if (tempCollisionBox.intersects(boundaryRect)) {
                                intersectingBoundaries.push_back(&boundary);
                        }
                }

                // Find the boundary with the shortest distance to the bottom of the monster's collision box
                float shortestDistance = std::numeric_limits<float>::max(); // Initialize to the maximum possible float value
                Boundary const * closestBoundary = nullptr;
                const float collisionBoxBottom = collisionBox.top + collisionBox.height;

                for (auto const * boundary : intersectingBoundaries) {
                        // Only consider boundaries below the monster
                        if (boundary->Y >= collisionBoxBottom) {
                                float distance = boundary->Y - collisionBoxBottom;
                                if (distance < shortestDistance) {
                                        shortestDistance = distance;
                                        closestBoundary = boundary;
                                }
                        }
                }

                // If we found a boundary below, pop the monster on it and set onGround to true
                if (closestBoundary != nullptr) {
                        // Calculate adjustment needed to place monster on the boundary
                        float adjustment = closestBoundary->Y - collisionBoxBottom;
                        position.y += adjustment; // add the distance to the monster's position
                        onGround = true;
                }
        }

        // Default the movement flags to false
        canMoveLeft = false;
        canMoveRight = false;
        for (const auto &boundary: boundaries) {
                sf::FloatRect boundaryRect(boundary.X, boundary.Y, boundary.Width, boundary.Height);
                if (boundaryRect.contains(GetLeftDropDetectorPosition())) { canMoveLeft = true; }
                if (boundaryRect.contains(GetRightDropDetectorPosition())) { canMoveRight = true; }
        }


        // Update collision box position
        collisionBox.left = position.x + 250 - (collisionBox.width / 2);
        collisionBox.top = position.y + 250 - (collisionBox.height / 2) + 40;
}
void Cactus::TickAnimation(GameTime gameTime) {
        const float distanceX = player->position.x - position.x;
        const float distanceY = player->position.y - position.y;
        const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        if (distance <= 150 && GetCurrentAnimation() == AnimationType::ANIMATION_ATTACK && GetCurrentAnimationFrame() == 3  && gameTime.TimeElapsed(nextAttackTime)) {
                player->TakeDamage(2);
                float xKnockBack = (faceDirection == FaceDirection::FACE_DIRECTION_LEFT) ? -800.f : 800.f;
                player->KnockBack({xKnockBack,-800});
                nextAttackTime = gameTime.NowAddMilliseconds(1250);
        }

        Mob::TickAnimation(gameTime);
}
void Cactus::Damaged(int amount, GameTime gameTime) {
        if (!IsDead()) {
                int index = (static_cast<int>(position.x) % 2) + 1;
                assetManager.PlaySoundEffect("Cactus/damaged"+std::to_string(index),100.f,1.f);
                ChangeAnimation(AnimationType::ANIMATION_DAMAGED,true);
        }

        Mob::Damaged(amount, gameTime);
}
void Cactus::DamagePlayer(int amount) {
        // Apply damage to the player
        player->health.Damage(amount);
}
sf::Vector2f Cactus::GetLeftDropDetectorPosition() {
        // Calculate the left drop detector position based on the collision box
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos =  collisionBox.left - 5;

        return {xPos, yPos};
}
sf::Vector2f Cactus::GetRightDropDetectorPosition() {
        // Calculate the right drop detector position based on the collision box
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos = collisionBox.left + collisionBox.width + 5;

        return {xPos, yPos};
}
void Cactus::UpdateQuads() {
        // Get the current animation frame
        TextureEntry* entry = &assetManager.TextureLibraries["Cactus"]->entries[GetTextureDrawIndex() + faceDirection];

        // Update the texture quads
        for (int i = 0; i < 4; ++i) {
                texQuads[i].texCoords = entry->texQuad[i].texCoords;
                texQuads[i].position = entry->texQuad[i].position + position;
        }
}
