//
// Created by Paul McGinley on 21/05/2025.
//

#include "EyeBall.h"

#include "objects/Player.h"

EyeBall::EyeBall(Player *player, sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(player, spawnPosition, viewRange, moveSpeed, health)  {
        // Define the animation sequences for the ChestMonster
        SetAnimationSequences( {
                {AnimationType::ANIMATION_IDLE, {0, 10, 60}},
                {AnimationType::ANIMATION_WALK, {0, 10, 100}}
        });

        ChangeAnimation(AnimationType::ANIMATION_IDLE, true);

        // Set the size of the collision box
        collisionBox.width = 150;
        collisionBox.height = 150;
}
void EyeBall::Update(GameTime gameTime) {
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

        // Direction Logic
        if (player.position.x < position.x) {
                faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
        } else {
                faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_CHESTMONSTER;
        }

        // Euclidean distance calculation
        // Ref: https://study.com/academy/lesson/euclidean-distance-calculation-formula-examples.html#:~:text=The%20formula%20for%20Euclidean%20distance,coordinates%20of%20the%20two%20points.
        // Using multiplication instead of pow for performance but yields the same result
        const float distanceX = player.position.x - position.x;
        const float distanceY = player.position.y - position.y;
        const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        // State Logic
        if (distance > 500 ) {
                // Move to player
                sf::Vector2f directionToPlayer = (player.position - position) / distance;
                position += directionToPlayer * walkSpeed * gameTime.deltaTime;
                ChangeAnimation(AnimationType::ANIMATION_WALK, gameTime, true);
        }


        // Update collision box position factoring the sprite offset (375)
        collisionBox.left = position.x + 375 - (collisionBox.width / 2);
        collisionBox.top = position.y + 375 - (collisionBox.height / 2);

        UpdateQuads();
}
void EyeBall::LateUpdate(GameTime gameTime) { TickAnimation(gameTime); }
void EyeBall::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Draw Monster
        window.draw(texQuads, &assetManager.TextureLibraries["Eye-Ball"]->entries[GetTextureDrawIndex()].texture);

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
void EyeBall::CalculatePhysicsState(std::vector<Boundary> boundaries,
                                    GameTime gameTime) { /* TODO: Add a shadow maybe? */
}
void EyeBall::DamagePlayer(int amount) { /* */ }
void EyeBall::TickAnimation(GameTime gameTime) { Mob::TickAnimation(gameTime); }
void EyeBall::Damaged(int amount) { Mob::Damaged(amount); }
void EyeBall::UpdateQuads() {
        // Get the current animation frame
        TextureEntry* entry = &assetManager.TextureLibraries["Eye-Ball"]->entries[GetTextureDrawIndex() + faceDirection];

        // Update the texture quads
        for (int i = 0; i < 4; ++i) {
                texQuads[i].texCoords = entry->texQuad[i].texCoords;
                texQuads[i].position = entry->texQuad[i].position + position;
        }
}
