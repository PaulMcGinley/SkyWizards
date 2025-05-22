//
// Created by Paul McGinley on 21/05/2025.
//

#include "EyeBall.h"

#include "objects/Player.h"

EyeBall::EyeBall(Player *player, sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(player, spawnPosition, viewRange, moveSpeed, health)
        , nextAttackTime(0)
        , spawnPosition(spawnPosition)
        , nextMoveTime(0) {
        // Define the animation sequences for the ChestMonster
        SetAnimationSequences( {
                        {AnimationType::ANIMATION_ATTACK, {0, 9, 90}},
                        {AnimationType::ANIMATION_ATTACK2_START, {9, 6, 100,}},
                        {AnimationType::ANIMATION_ATTACK2_REPEAT, {15, 8, 100,}},
                        {AnimationType::ANIMATION_ATTACK3, {23, 9, 90}},
                        {AnimationType::ANIMATION_BATTLE_IDLE, {19, 9, 100}},
                        {AnimationType::ANIMATION_DAMAGED, {57, 9, 100, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_DIZZY, true);},nullptr}},
                        {AnimationType::ANIMATION_DEATH, {32, 9, 100, nullptr, [this](){ChangeAnimation(AnimationType::ANIMATION_DEAD);},nullptr}},
                        {AnimationType::ANIMATION_DEAD, {40, 1, 100}},
                        {AnimationType::ANIMATION_DIZZY, {41, 16, 100}},
                        {AnimationType::ANIMATION_IDLE2, {66, 8, 100}},
                        {AnimationType::ANIMATION_IDLE, {74, 16, 60}},
                        {AnimationType::ANIMATION_STATIC, {74, 1, 100}},
                        {AnimationType::ANIMATION_RUN, {90, 7, 100}},
                        {AnimationType::ANIMATION_SENSE_SOMETHING_START, {97, 9, 100}},
                        {AnimationType::ANIMATION_SENSE_SOMETHING, {106, 44, 100}},
                        {AnimationType::ANIMATION_TAUNT, {150, 12, 100}},
                        {AnimationType::ANIMATION_VICTORY, {162, 24, 100}},
                        {AnimationType::ANIMATION_WALK, {186, 11, 100}},
                        {AnimationType::ANIMATION_FLOAT_LEFT, {208, 11, 100}},
                        {AnimationType::ANIMATION_FLOAT_RIGHT, {197, 11, 100}},
        });

        ChangeAnimation(AnimationType::ANIMATION_IDLE, true);

        // Set the size of the collision box
        collisionBox.width = 175;
        collisionBox.height = 175;
}
void EyeBall::Update(const GameTime gameTime) {
        // Update quads in being damaged but perform no other logic
        if (IsDead() || GetCurrentAnimation() == AnimationType::ANIMATION_DAMAGED || (GetCurrentAnimation() == AnimationType::ANIMATION_DIZZY && !gameTime.TimeElapsed(nextMoveTime))) {
                UpdateQuads();
                return;
        }

        // Get a reference to the player to save on dereferencing calls
        Player const & player = *this->player;

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
        if (distance > 500 && distance < viewRange) {
                // Move to player
                sf::Vector2f directionToPlayer = (player.position - position) / distance;
                if (directionToPlayer.x < 0) {
                        ChangeAnimation(AnimationType::ANIMATION_FLOAT_LEFT, gameTime, true);
                } else {
                        ChangeAnimation(AnimationType::ANIMATION_FLOAT_RIGHT, gameTime, true);
                }
                position += directionToPlayer * walkSpeed * gameTime.deltaTime;

        } else if (distance < 500 && gameTime.TimeElapsed(nextAttackTime)) {
                // Teleport player to last checkpoint
        } else {
                // Move to spawn position
                sf::Vector2f directionToSpawn = (spawnPosition - position) / distance;
                if (directionToSpawn.x < 0) {
                        ChangeAnimation(AnimationType::ANIMATION_FLOAT_LEFT, gameTime, true);
                } else {
                        ChangeAnimation(AnimationType::ANIMATION_FLOAT_RIGHT, gameTime, true);
                }
                position += directionToSpawn * walkSpeed * gameTime.deltaTime;
        }

        // NOTE: This may cause issues later, but we shall see
        if (position.y - player.position.y > 400)
                position.y -=800;


        // Update collision box position factoring the sprite offset (375)
        collisionBox.left = position.x + 350 - (collisionBox.width / 2);
        collisionBox.top = position.y + 350 - (collisionBox.height / 2)+40;

        UpdateQuads();
}
void EyeBall::LateUpdate(const GameTime gameTime) { TickAnimation(gameTime); }
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
void EyeBall::CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gameTime) { /* TODO: Add a shadow maybe? */ }
void EyeBall::DamagePlayer(int amount) { /* */ }
void EyeBall::TickAnimation(GameTime gameTime) { Mob::TickAnimation(gameTime); }
void EyeBall::Damaged(int amount, GameTime gameTime) {
        ChangeAnimation(AnimationType::ANIMATION_DAMAGED, true);
        nextMoveTime = gameTime.NowAddMilliseconds(3000); // Stuntime

        // Don't call base Damaged as Eye-Ball cannot take damage
        /* Mob::Damaged(amount, gameTime); */
}
void EyeBall::UpdateQuads() {
        // Get the current animation frame
        TextureEntry* entry = &assetManager.TextureLibraries["Eye-Ball"]->entries[GetTextureDrawIndex()];

        // Update the texture quads
        for (int i = 0; i < 4; ++i) {
                texQuads[i].texCoords = entry->texQuad[i].texCoords;
                texQuads[i].position = entry->texQuad[i].position + position;
        }
}
