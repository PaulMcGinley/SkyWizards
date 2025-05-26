//
// Created by Paul McGinley on 22/05/2025.
//

#include "SlimeMonster.h"

#include <SFML/Graphics/CircleShape.hpp>

#include "objects/Player.h"
SlimeMonster::SlimeMonster(Player *player, sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(player, spawnPosition, viewRange, moveSpeed, health)
        , canMoveLeft(false)
        , canMoveRight(false)
        , onGround(false)
        , nextMoveTime(0)
        , nextPosition(spawnPosition) {
        // Define the animation sequences for the SlimeMonster
        SetAnimationSequences( {
                {AnimationType::ANIMATION_ATTACK, {0, 10, 90}},
                {AnimationType::ANIMATION_ATTACK2, {10, 10, 90,}},
                {AnimationType::ANIMATION_BATTLE_IDLE, {62, 10, 100}},
                {AnimationType::ANIMATION_DAMAGED, {52, 10, 100, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_IDLE);},nullptr}},
                {AnimationType::ANIMATION_DEATH, {20, 16, 100, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_DEAD);},nullptr}},
                {AnimationType::ANIMATION_DEAD, {35, 1, 1000}},
                {AnimationType::ANIMATION_DIZZY, {36, 16, 100}},
                {AnimationType::ANIMATION_IDLE, {72, 20, 50}},
                {AnimationType::ANIMATION_STATIC, {79, 1, 100}},
                {AnimationType::ANIMATION_RUN, {92, 8, 80}},
                {AnimationType::ANIMATION_SENSE_SOMETHING, {100, 28, 40}},
                {AnimationType::ANIMATION_SENSE_SOMETHING_START, {128, 24, 40}},
                {AnimationType::ANIMATION_TAUNT, {152, 8, 100}},
                {AnimationType::ANIMATION_VICTORY, {160, 32, 100}},
                {AnimationType::ANIMATION_WALK, {192, 12, 80}}
        });

        // Set the size of the collision box
        collisionBox.width = 100;
        collisionBox.height = 90;

        score = 500;
}
void SlimeMonster::Update(GameTime gameTime) {
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

        faceDirection = (player.position.x < position.x) ? FaceDirection::FACE_DIRECTION_LEFT : FaceDirection::FACE_DIRECTION_RIGHT_SLIMEMONSTER;

        // Euclidean distance calculation
        // Ref: https://study.com/academy/lesson/euclidean-distance-calculation-formula-examples.html#:~:text=The%20formula%20for%20Euclidean%20distance,coordinates%20of%20the%20two%20points.
        // Using multiplication instead of pow for performance but yields the same result
        const float distanceX = player.position.x - position.x;
        const float distanceY = player.position.y - position.y;
        const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        // State Logic
        if (distance < viewRange) {
                // Player is within CQB range
                ChangeAnimation(AnimationType::ANIMATION_RUN, gameTime, true); // Bite
        } else {
                // Player is out of view range
                ChangeAnimation(AnimationType::ANIMATION_IDLE, gameTime, false);
        }

        // Movement Logic
        if (GetCurrentAnimation() == AnimationType::ANIMATION_RUN && gameTime.TimeElapsed(nextMoveTime)) {
                // Calculate the distance to move based on the speed and delta time
                const float moveDistance = walkSpeed * gameTime.deltaTime;

                if (faceDirection == FaceDirection::FACE_DIRECTION_LEFT && canMoveLeft) {
                        // Move Left
                        position.x -= moveDistance *4;
                        nextMoveTime = gameTime.NowAddMilliseconds(200);
                } else if (faceDirection == FaceDirection::FACE_DIRECTION_RIGHT_SLIMEMONSTER && canMoveRight) {
                        // Move Right
                        position.x += moveDistance *4;
                        nextMoveTime = gameTime.NowAddMilliseconds(200);
                } else {
                        // Return to idle if no movement is possible
                        ChangeAnimation(AnimationType::ANIMATION_IDLE);
                }
        }

        // Update collision box position factoring the sprite offset (250)
        collisionBox.left = position.x + 250 - (collisionBox.width / 2);
        collisionBox.top = position.y + 250 - (collisionBox.height / 2) + 40; // +40 for better alignment

        UpdateQuads();
}
void SlimeMonster::LateUpdate(GameTime gameTime) { TickAnimation(gameTime);}
void SlimeMonster::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Draw Shadow
        float shadowX = collisionBox.left + (collisionBox.width/2) - assetManager.TextureLibraries["PrgUse"]->entries[9].texture.getSize().x / 2;
        float shadowY = position.y + 310; // +310 for better alignment
        IDraw::Draw(window, "PrgUse", 9, sf::Vector2f(shadowX, shadowY));

        // Draw Monster
        window.draw(texQuads, &assetManager.TextureLibraries["SlimeMonster"]->entries[GetTextureDrawIndex() + faceDirection].texture);

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
void SlimeMonster::CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) {
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
void SlimeMonster::TickAnimation(GameTime gameTime) { Mob::TickAnimation(gameTime); }
void SlimeMonster::Damaged(int amount, GameTime gameTime) {
        if (!IsDead())
                ChangeAnimation(AnimationType::ANIMATION_DAMAGED,true);

        Mob::Damaged(amount, gameTime);
}
void SlimeMonster::DamagePlayer(int amount) {
        // Apply damage to the player
        player->health.Damage(amount);
}
sf::Vector2f SlimeMonster::GetLeftDropDetectorPosition() {
        // Calculate the left drop detector position based on the collision box
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos =  collisionBox.left - 5;

        return {xPos, yPos};
}
sf::Vector2f SlimeMonster::GetRightDropDetectorPosition() {
        // Calculate the right drop detector position based on the collision box
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos = collisionBox.left + collisionBox.width + 5;

        return {xPos, yPos};
}
void SlimeMonster::UpdateQuads() {
        // Get the current animation frame
        TextureEntry* entry = &assetManager.TextureLibraries["SlimeMonster"]->entries[GetTextureDrawIndex() + faceDirection];

        // Update the texture quads
        for (int i = 0; i < 4; ++i) {
                texQuads[i].texCoords = entry->texQuad[i].texCoords;
                texQuads[i].position = entry->texQuad[i].position + position;
        }
}
