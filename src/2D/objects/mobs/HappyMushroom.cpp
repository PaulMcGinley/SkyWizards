//
// Created by Paul McGinley on 22/05/2025.
//

#include "HappyMushroom.h"

#include "objects/Player.h"
HappyMushroom::HappyMushroom(Player *player, sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(player, spawnPosition, viewRange, moveSpeed, health)
        , nextBounceTime(0)
        , onGround(false) {

        SetAnimationSequences({
                {AnimationType::ANIMATION_IDLE, {8, 15, 100}},
                {AnimationType::ANIMATION_ATTACK, {24, 10, 50}}
                });

        collisionBox.width = 100;
        collisionBox.height = 90;

        ChangeAnimation(AnimationType::ANIMATION_IDLE, true);
}
void HappyMushroom::Update(GameTime gameTime) {
        // Get a reference to the player to save on dereferencing calls
        Player & player = *this->player;

        // Change animation to the static sequence if the player is dead.
        if (player.GetIsDead()) {
                ChangeAnimation(AnimationType::ANIMATION_IDLE, gameTime, false);
                UpdateQuads(); // Update the texture quads and perform no other logic
                return;
        }


        // Euclidean distance calculation
        // Ref: https://study.com/academy/lesson/euclidean-distance-calculation-formula-examples.html#:~:text=The%20formula%20for%20Euclidean%20distance,coordinates%20of%20the%20two%20points.
        // Using multiplication instead of pow for performance but yields the same result
        const float distanceX = player.position.x - position.x;
        const float distanceY = player.position.y - position.y;
        const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        // State Logic
        if (distance < 100 && gameTime.TimeElapsed(nextBounceTime)) {
                // Bounce range
                ChangeAnimation(AnimationType::ANIMATION_ATTACK, gameTime, true);
        } else {
                // Idle
                ChangeAnimation(AnimationType::ANIMATION_IDLE, gameTime, false);
        }

        // Update collision box position factoring the sprite offset (250)
        collisionBox.left = position.x + 250 - (collisionBox.width / 2);
        collisionBox.top = position.y + 250 - (collisionBox.height / 2) +0; // +40 for better alignment

        UpdateQuads();
}
void HappyMushroom::LateUpdate(GameTime gameTime) { TickAnimation(gameTime); }
void HappyMushroom::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Draw Shadow
        float shadowX = collisionBox.left + (collisionBox.width/2) - assetManager.TextureLibraries["PrgUse"]->entries[9].texture.getSize().x / 2;
        float shadowY = position.y + 280; // +310 for better alignment
        IDraw::Draw(window, "PrgUse", 9, sf::Vector2f(shadowX, shadowY));

        // Draw Monster
        window.draw(texQuads, &assetManager.TextureLibraries["HappyMushroom"]->entries[GetTextureDrawIndex()].texture);

        if (GameManager::getInstance().ShowCollisions()) {
                // Collision box
                sf::RectangleShape rect(sf::Vector2f(collisionBox.width, collisionBox.height));
                rect.setPosition(collisionBox.left, collisionBox.top);
                rect.setFillColor(sf::Color(0, 0, 255, 128));
                rect.setOutlineColor(sf::Color::Black);
                rect.setOutlineThickness(1.0f);
                window.draw(rect);
        }
}
void HappyMushroom::CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) {
        // Create a temporary elongated collision box that extends downward
        // This it to test for collisions beneath the monster
        sf::FloatRect tempCollisionBox = collisionBox;
        tempCollisionBox.height = 1000; // Extend downward by 1000 pixels

        // Find all boundaries that the temp collision box intersects with
        std::vector<Boundary *> intersectingBoundaries;
        for (auto &boundary: boundaries) {
                sf::FloatRect boundaryRect(boundary.X, boundary.Y, boundary.Width, boundary.Height);
                if (tempCollisionBox.intersects(boundaryRect)) {
                        intersectingBoundaries.push_back(&boundary);
                }
        }

        // Find the boundary with the shortest distance to the bottom of the monster's collision box
        float shortestDistance = std::numeric_limits<float>::max(); // Initialize to the maximum possible float value
        Boundary *closestBoundary = nullptr;
        float collisionBoxBottom = collisionBox.top + collisionBox.height;

        for (auto *boundary: intersectingBoundaries) {
                // Only consider boundaries below the monster
                if (boundary->Y >= collisionBoxBottom) {
                        float distance = boundary->Y - collisionBoxBottom;
                        if (distance < shortestDistance) {
                                shortestDistance = distance;
                                closestBoundary = boundary;
                        }
                }
        }

        // If we found a boundary below, pop the monster on it
        // No more pseudo gravity
        if (closestBoundary != nullptr) {
                // Calculate adjustment needed to place monster on the boundary (top of boundary to bottom of collision
                // box)
                float adjustment = closestBoundary->Y - collisionBoxBottom;
                position.y += adjustment; // add the distance to the monster's position
                onGround = true;
        } else {
                onGround = false;
        }
}
void HappyMushroom::TickAnimation(GameTime gameTime) {
        if (GetCurrentAnimation() == AnimationType::ANIMATION_ATTACK && GetCurrentAnimationFrame() == 2 && gameTime.TimeElapsed(nextBounceTime)) {
                std::cout << "HappyMushroom::TickAnimation: Bouncing player!" << std::endl;
                // Check if the player is dead
                Player & player = *this->player;
                if (player.GetIsDead())
                        return;

                // Euclidean distance calculation
                const float distanceX = player.position.x - position.x + 250; // +250 for better alignment
                const float distanceY = player.position.y - position.y + 250; // +250 for better alignment
                const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

                // Check if the player is within attack range to deal damage
                if (distance <= 400) {
                        player.BounceUp(1000.f);
                        nextBounceTime = gameTime.NowAddMilliseconds(BOUNCE_COOLDOWN);
                }
        }

        Mob::TickAnimation(gameTime);
}
void HappyMushroom::Damaged(int amount, GameTime gameTime) { /* Mob::Damaged(amount, gameTime); */ }
void HappyMushroom::DamagePlayer(int amount) { /* */ }
void HappyMushroom::UpdateQuads() {
        // Get the current animation frame
        TextureEntry* entry = &assetManager.TextureLibraries["HappyMushroom"]->entries[GetTextureDrawIndex()];


        // Update the texture quads
        for (int i = 0; i < 4; ++i) {
                texQuads[i].texCoords = entry->texQuad[i].texCoords;
                texQuads[i].position = entry->texQuad[i].position + position;
        }
}
