//
// Created by Paul McGinley on 03/10/2024.
//

#include "ChestMonster.h"

#include <SFML/Graphics/CircleShape.hpp>

#include "models/TextureEntry.h"
#include "scenes/GameScene.h"

ChestMonster::ChestMonster(Player *player, sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(player, spawnPosition, viewRange, moveSpeed, health)
        , nextAttackTime(0) {
        sequences = {{AnimationType::ANIMATION_ATTACK, {0, 10, 100}},
                     {AnimationType::ANIMATION_ATTACK2, {10, 9, 100, nullptr, [this]() { DamagePlayer(1); }, nullptr}},
                     {AnimationType::ANIMATION_BATTLE_IDLE,
                      {19, 9, 100, nullptr, [this]() { ChangeAnimation(AnimationType::ANIMATION_TAUNT); }, nullptr}},
                     {AnimationType::ANIMATION_DAMAGED, {28, 7, 100}},
                     {AnimationType::ANIMATION_DEATH, {35, 12, 100}},
                     {AnimationType::ANIMATION_DIZZY, {47, 16, 100}},
                     {AnimationType::ANIMATION_IDLE,
                      {63, 16, 60, nullptr, [this]() { ChangeAnimation(AnimationType::ANIMATION_BATTLE_IDLE); },
                       nullptr}},
                     {AnimationType::ANIMATION_STATIC, {79, 1, 100}}, // should be length 1
                     {AnimationType::ANIMATION_RUN, {87, 7, 100}},
                     {AnimationType::ANIMATION_SENSE_SOMETHING,
                      {95, 56, 100, nullptr, [this]() { ChangeAnimation(AnimationType::ANIMATION_IDLE); }, nullptr}},
                     {AnimationType::ANIMATION_TAUNT,
                      {151, 24, 100, nullptr, [this]() { ChangeAnimation(AnimationType::ANIMATION_SENSE_SOMETHING); },
                       nullptr}},
                     {AnimationType::ANIMATION_VICTORY, {185, 12, 100}},
                     {AnimationType::ANIMATION_WALK, {187, 12, 100}}};

        collisionBox.width = 100;
        collisionBox.height = 100;
}

void ChestMonster::Update(GameTime gameTime) {
        // AI Code here

        // Walk AI
        if (currentAnimation == AnimationType::ANIMATION_WALK) {
                // Calculate how much to move
                const float moveDistance =  walkSpeed * gameTime.delta_time;

                switch (faceDirection) {
                        case FaceDirection::FACE_DIRECTION_LEFT:
                                position.x -= moveDistance;
                        break;
                        case FaceDirection::FACE_DIRECTION_RIGHT_CHESTMONSTER:
                                position.x += moveDistance;
                        break;
                        default: /* */;
                }
        }

        // Run AI
        if (currentAnimation == AnimationType::ANIMATION_RUN) {
                // Calculate how much to move
                const float moveDistance =  walkSpeed * gameTime.delta_time;

                switch (faceDirection) {
                        case FaceDirection::FACE_DIRECTION_LEFT:
                                position.x -= moveDistance;
                        break;
                        case FaceDirection::FACE_DIRECTION_RIGHT_CHESTMONSTER:
                                position.x += moveDistance;
                        break;
                        default: /* */;
                }
        }

        collisionBox.left = position.x + 250 - (collisionBox.width / 2);
        collisionBox.top = position.y + 250 - (collisionBox.height / 2) + 40;

        TextureEntry* entry = &asset_manager.TextureLibraries["ChestMonster"]->entries[GetCurrentAnimationFrame()];

        for (int i = 0; i < 4; ++i) {
                texQuads[i].texCoords = entry->texQuad[i].texCoords;
                texQuads[i].position = entry->texQuad[i].position + position;
        }
}

void ChestMonster::LateUpdate(GameTime gameTime) {
        TickAnimation(gameTime);
}

void ChestMonster::Draw(sf::RenderWindow &window, GameTime gameTime) {

        window.draw(texQuads,
                    &asset_manager.TextureLibraries["ChestMonster"]->entries[GetCurrentAnimationFrame()].texture);

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
        leftDropDetector.setPosition(leftDropDetectorPosition());
        window.draw(leftDropDetector);
        sf::CircleShape rightDropDetector(5);
        rightDropDetector.setFillColor(sf::Color::Red);
        rightDropDetector.setPosition(rightDropDetectorPosition());
        window.draw(rightDropDetector);
        // END DEBUG ^


}
void ChestMonster::UpdatePlayerPosition(const sf::Vector2f playerPosition, GameTime gameTime) {
        if (player->GetIsDead()) {
                ChangeAnimation(AnimationType::ANIMATION_STATIC);
                return;
        }

        // Check if the player is to the left or right
        if (playerPosition.x < position.x) {
                faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
        } else {
                faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_CHESTMONSTER;
        }

        // Ceck if the player is within the view range
        // Calculate distance to player
        float distance = std::abs((playerPosition.x + 250) - (position.x + 250)); // +250 for the offset

        if (distance <= 100) {
                // Player is within attack range
                ChangeAnimation(AnimationType::ANIMATION_ATTACK); // Bite
        } else if (distance < 200) {
                // Player is within attack range
                ChangeAnimation(AnimationType::ANIMATION_ATTACK2); // Smack off screen
        } else if (distance > viewRange / 2 && distance < viewRange) {
                // Player is within battle idle range
                ChangeAnimation(AnimationType::ANIMATION_BATTLE_IDLE);
        } else if (distance < viewRange / 2) {
                // Player is within chase range
                ChangeAnimation(AnimationType::ANIMATION_RUN);
        } else if (distance < viewRange) {
                // Player is within view range
                if (currentAnimation == AnimationType::ANIMATION_STATIC) {
                        ChangeAnimation(AnimationType::ANIMATION_SENSE_SOMETHING);
                }
        } else {
                // Player is out of view range
                ChangeAnimation(AnimationType::ANIMATION_STATIC);
        }
}
sf::Vector2f ChestMonster::leftDropDetectorPosition() {
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos =  collisionBox.left - 5;

        return {xPos, yPos};
}
sf::Vector2f ChestMonster::rightDropDetectorPosition() {
        float yPos = collisionBox.top + collisionBox.height + 5;
        float xPos = collisionBox.left + collisionBox.width + 5;

        return {xPos, yPos};
}
void ChestMonster::DamagePlayer(int amount) {
        player->health.damage(amount);
}
