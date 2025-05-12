//
// Created by Paul McGinley on 03/10/2024.
//

#include "ChestMonster.h"

#include "models/TextureEntry.h"

ChestMonster::ChestMonster(sf::Vector2f spawnPosition, const float viewRange, const float moveSpeed, const int health)
        : Mob(spawnPosition, viewRange, moveSpeed, health) {
        sequences = {{AnimationType::ANIMATION_ATTACK, {0, 10, 100}},
                     {AnimationType::ANIMATION_ATTACK2, {10, 9, 100}},
                     {AnimationType::ANIMATION_BATTLE_IDLE, {19, 9, 100}},
                     {AnimationType::ANIMATION_DAMAGED, {28, 7, 100}},
                     {AnimationType::ANIMATION_DEATH, {35, 12, 100}},
                     {AnimationType::ANIMATION_DIZZY, {47, 16, 100}},
                     {AnimationType::ANIMATION_IDLE, {63, 16, 60}},
                     {AnimationType::ANIMATION_STATIC, {79, 1, 100}}, // should be length 1
                     {AnimationType::ANIMATION_RUN, {87, 7, 100}},
                     {AnimationType::ANIMATION_SENSE_SOMETHING, {95, 56, 100}},
                     {AnimationType::ANIMATION_TAUNT, {151, 24, 100}},
                     {AnimationType::ANIMATION_VICTORY, {185, 12, 100}},
                     {AnimationType::ANIMATION_WALK, {187, 12, 100}}};
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
                const float moveDistance =  runSpeed * gameTime.delta_time;

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

        TextureEntry* entry = &asset_manager.TextureLibraries["ChestMonster"]->entries[GetCurrentAnimationFrame()];

        // texQuads[0].texCoords = entry->texQuad[0].texCoords;
        // texQuads[1].texCoords = entry->texQuad[1].texCoords;
        // texQuads[2].texCoords = entry->texQuad[2].texCoords;
        // texQuads[3].texCoords = entry->texQuad[3].texCoords;
        //
        // texQuads[0].position = entry->texQuad[0].position + position;
        // texQuads[1].position = entry->texQuad[1].position + position;
        // texQuads[2].position = entry->texQuad[2].position + position;
        // texQuads[3].position = entry->texQuad[3].position + position;

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
}
void ChestMonster::UpdatePlayerPosition(sf::Vector2f playerPosition) {
        // Ceck if the player is within the view range
        // Calculate distance to player
        float distance = std::abs(playerPosition.x - position.x);

        if (distance < viewRange / 2) {
                // Player is within chase range
                ChangeAnimation(AnimationType::ANIMATION_RUN);
        } else if (distance < viewRange) {
                // Player is within view range
                ChangeAnimation(AnimationType::ANIMATION_SENSE_SOMETHING);
                // Check if the player is to the left or right
                if (playerPosition.x < position.x) {
                        faceDirection = FaceDirection::FACE_DIRECTION_LEFT;
                } else {
                        faceDirection = FaceDirection::FACE_DIRECTION_RIGHT_CHESTMONSTER;
                }
        } else {
                // Player is out of view range
                ChangeAnimation(AnimationType::ANIMATION_IDLE);
        }
}
