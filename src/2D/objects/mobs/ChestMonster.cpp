//
// Created by Paul McGinley on 03/10/2024.
//

#include "ChestMonster.h"

#include <SFML/Graphics/CircleShape.hpp>

#include "models/TextureEntry.h"
#include "scenes/GameScene.h"

ChestMonster::ChestMonster(Player *player, sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(player, spawnPosition, viewRange, moveSpeed, health)
        , nextSmackTime(0)
        , nextBiteTime(0)
        , canMoveLeft(false)
        , canMoveRight(false)
        , onGround(false)
{
        // Define the animation sequences for the ChestMonster
        sequences = {
                {AnimationType::ANIMATION_ATTACK, {0, 10, 90}},
                {AnimationType::ANIMATION_ATTACK2, {10, 9, 100,}},
                {AnimationType::ANIMATION_BATTLE_IDLE, {19, 9, 100}},
                {AnimationType::ANIMATION_DAMAGED, {28, 7, 100}},
                {AnimationType::ANIMATION_DEATH, {35, 12, 100}},
                {AnimationType::ANIMATION_DIZZY, {47, 16, 100}},
                {AnimationType::ANIMATION_IDLE, {63, 16, 60}},
                {AnimationType::ANIMATION_STATIC, {79, 1, 100}},
                {AnimationType::ANIMATION_RUN, {87, 7, 100}},
                {AnimationType::ANIMATION_SENSE_SOMETHING, {95, 56, 100}},
                {AnimationType::ANIMATION_TAUNT, {151, 24, 100}},
                {AnimationType::ANIMATION_VICTORY, {185, 12, 100}},
                {AnimationType::ANIMATION_WALK, {187, 12, 100}}
        };

        // Set the size of the collision box
        collisionBox.width = 100;
        collisionBox.height = 90;
}
void ChestMonster::Update(GameTime gameTime) {
        // Get a reference to the player to save on dereferencing calls
        Player& player = *this->player;

        // Check if the player is dead
        if (player.GetIsDead()) {
                ChangeAnimation(AnimationType::ANIMATION_STATIC, gameTime, false); // Hide
                UpdateQuads(); // Update the texture quads here because we are skipping the rest of the logic
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
        if (distance < biteDistance && gameTime.TimeElapsed(nextBiteTime)) {
                // Player is within CQB range
                ChangeAnimation(AnimationType::ANIMATION_ATTACK, gameTime, true); // Bite
                //nextBiteTime = gameTime.NowAddMilliseconds(biteCooldown);
        } else if (distance < smackDistance && distance >= biteDistance && gameTime.TimeElapsed(nextSmackTime)) {
                // Player is within smack attack range
                ChangeAnimation(AnimationType::ANIMATION_ATTACK2, gameTime, true); // Smack off screen
                //nextSmackTime = gameTime.NowAddMilliseconds(smackCooldown);
        } else if (distance > biteDistance && distance < chaseDistance) {
                // Player is within chase range
                ChangeAnimation(AnimationType::ANIMATION_RUN, gameTime, false);
        } else if (distance < idleDistance) {
                // Player is within battle idle range
                ChangeAnimation(AnimationType::ANIMATION_BATTLE_IDLE, gameTime, false);
        } else {
                // Player is out of view range
                ChangeAnimation(AnimationType::ANIMATION_STATIC, gameTime, false);
        }

        // Movement Logic
        if (currentAnimation == AnimationType::ANIMATION_RUN && distance > biteDistance) {
                // Calculate the distance to move based on the speed and delta time
                const float moveDistance = walkSpeed * gameTime.delta_time;

                if (faceDirection == FaceDirection::FACE_DIRECTION_LEFT && canMoveLeft) {
                        // Move Left
                        position.x -= moveDistance;
                } else if (faceDirection == FaceDirection::FACE_DIRECTION_RIGHT_CHESTMONSTER && canMoveRight) {
                        // Move Right
                        position.x += moveDistance;
                } else {
                        // Return to idle if no movement is possible
                        ChangeAnimation(AnimationType::ANIMATION_IDLE);
                }
        }

        // Update collision box position
        collisionBox.left = position.x + 250 - (collisionBox.width / 2);
        collisionBox.top = position.y + 250 - (collisionBox.height / 2) + 40;

        UpdateQuads();
}

void ChestMonster::LateUpdate(GameTime gameTime) {
        // Update the animation
        TickAnimation(gameTime);
}
void ChestMonster::Draw(sf::RenderWindow &window, GameTime gameTime) {
        // Draw Shadow
        float shadowX = collisionBox.left + (collisionBox.width/2) - asset_manager.TextureLibraries["PrgUse"]->entries[9].texture.getSize().x / 2;
        float shadowY = position.y + 310;
        IDraw::Draw(window, "PrgUse", 9, sf::Vector2f(shadowX, shadowY));

        // Draw Monster
        window.draw(texQuads, &asset_manager.TextureLibraries["ChestMonster"]->entries[GetCurrentAnimationFrame()].texture);

        // // Collision box
        // sf::RectangleShape rect(sf::Vector2f(collisionBox.width, collisionBox.height));
        // rect.setPosition(collisionBox.left, collisionBox.top);
        // rect.setFillColor(sf::Color(0, 0, 255, 128));
        // rect.setOutlineColor(sf::Color::Black);
        // rect.setOutlineThickness(1.0f);
        // window.draw(rect);
        //
        // // DEBUG: Draw detector circles
        // sf::CircleShape leftDropDetector(5);
        // leftDropDetector.setFillColor(sf::Color::Green);
        // leftDropDetector.setPosition(leftDropDetectorPosition());
        // window.draw(leftDropDetector);
        // sf::CircleShape rightDropDetector(5);
        // rightDropDetector.setFillColor(sf::Color::Red);
        // rightDropDetector.setPosition(rightDropDetectorPosition());
        // window.draw(rightDropDetector);
        // // END DEBUG ^
}
void ChestMonster::CalculatePhysicsState(std::vector<Boundary> boundaries, GameTime gametime) {
        // Default the movement flags to false
        canMoveLeft = false;
        canMoveRight = false;

        // If not on ground, prevent movement and moves mob down until it rests on a boundary
        if (!onGround) {

                // Check each boundary to see if the monster is on the ground
                for (const auto &boundary: boundaries) {
                        sf::FloatRect boundaryRect(boundary.X, boundary.Y, boundary.Width, boundary.Height);
                        if (collisionBox.intersects(boundaryRect)) {
                                onGround = true;
                                return;
                        }

                        // Move the monster down until it rests on a boundary
                        const float fallSpeed = 100.0f; // (Pixels per second to move down)
                        position.y += fallSpeed * gametime.delta_time;
                }
        }

        for (const auto &boundary: boundaries) {
                sf::FloatRect boundaryRect(boundary.X, boundary.Y, boundary.Width, boundary.Height);
                if (boundaryRect.contains(GetLeftDropDetectorPosition())) {
                        // Left drop detector is inside the boundary - can move left
                        canMoveLeft = true;
                }
                if (boundaryRect.contains(GetRightDropDetectorPosition())) {
                        // Right drop detector is inside the boundary - can move right
                        canMoveRight = true;
                }
        }
}
void ChestMonster::TickAnimation(GameTime gameTime) {
        std::cout << "TickAnimation" << std::endl;
        if(gameTime.TimeElapsed(nextBiteTime) && currentAnimation == AnimationType::ANIMATION_ATTACK && currentAnimationFrame == 5) {
                BitePlayer();
                nextBiteTime = gameTime.NowAddMilliseconds(biteCooldown);
        }
        if(gameTime.TimeElapsed(nextSmackTime) && currentAnimation == AnimationType::ANIMATION_ATTACK2 && currentAnimationFrame == 5) {
                SmackPlayer();
                nextSmackTime = gameTime.NowAddMilliseconds(smackCooldown);
        }

        Mob::TickAnimation(gameTime);
}
void ChestMonster::BitePlayer() {
        std::cout << "BitePlayer" << std::endl;

        // Get a reference to the player to save on dereferencing calls
        Player& player = *this->player;

        // Check if the player is dead
        if (player.GetIsDead())
                return;

        // Euclidean distance calculation
        const float distanceX = player.position.x - position.x;
        const float distanceY = player.position.y - position.y;
        const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        // Check if the player is within bite range to deal damage
        if (distance <= (biteDistance*2.f)) {
                DamagePlayer(1);
        }
}
void ChestMonster::SmackPlayer() {
        // Get a reference to the player to save on dereferencing calls
        Player& player = *this->player;

        // Check if the player is dead
        if (player.GetIsDead())
                return;

        // Euclidean distance calculation
        const float distanceX = player.position.x - position.x;
        const float distanceY = player.position.y - position.y;
        const float distance = std::sqrt(distanceX * distanceX + distanceY * distanceY);

        // Check if the player is within smack range to deal damage
        if (distance <= (smackDistance*2.f)) {
                DamagePlayer(2);
        }
}
sf::Vector2f ChestMonster::GetLeftDropDetectorPosition() {
        // Calculate the left drop detector position based on the collision box
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos =  collisionBox.left - 5;

        return {xPos, yPos};
}
sf::Vector2f ChestMonster::GetRightDropDetectorPosition() {
        // Calculate the right drop detector position based on the collision box
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos = collisionBox.left + collisionBox.width + 5;

        return {xPos, yPos};
}
void ChestMonster::UpdateQuads() {
        // Get the current animation frame
        TextureEntry* entry = &asset_manager.TextureLibraries["ChestMonster"]->entries[GetCurrentAnimationFrame()];

        // Update the texture quads
        for (int i = 0; i < 4; ++i) {
                texQuads[i].texCoords = entry->texQuad[i].texCoords;
                texQuads[i].position = entry->texQuad[i].position + position;
        }
}
void ChestMonster::DamagePlayer(int amount) {
        // Apply damage to the player
        player->health.damage(amount);
}
