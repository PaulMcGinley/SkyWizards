//
// Created by Paul McGinley on 03/10/2024.
//

#include "ChestMonster.h"

#include "models/TextureEntry.h"

ChestMonster::ChestMonster(sf::Vector2f spawnPosition) : position(spawnPosition) {

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
                     {AnimationType::ANIMATION_WALK, {187, 12, 100}}

        };
        // ChangeAni(AniType::Taunt, GameTime());
}

void ChestMonster::Update(GameTime gameTime) {
        // AI Code here

        // Walk AI
        if (currentAnimation == AnimationType::ANIMATION_WALK) {
                // Calculate how much to move
                float moveDistance =  WALK_SPEED * gameTime.delta_time;

                switch (face_direction) {
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
                float moveDistance =  RUN_SPEED * gameTime.delta_time;

                switch (face_direction) {
                        case FaceDirection::FACE_DIRECTION_LEFT:
                                position.x -= moveDistance;
                        break;
                        case FaceDirection::FACE_DIRECTION_RIGHT_CHESTMONSTER:
                                position.x += moveDistance;
                        break;
                        default: /* */;
                }
        }

        TextureEntry* entry = asset_manager.getChestMonsterFrame_ptr(frame());

        texture_quads[0].texCoords = entry->texQuad[0].texCoords;
        texture_quads[1].texCoords = entry->texQuad[1].texCoords;
        texture_quads[2].texCoords = entry->texQuad[2].texCoords;
        texture_quads[3].texCoords = entry->texQuad[3].texCoords;

        texture_quads[0].position = entry->texQuad[0].position + position;
        texture_quads[1].position = entry->texQuad[1].position + position;
        texture_quads[2].position = entry->texQuad[2].position + position;
        texture_quads[3].position = entry->texQuad[3].position + position;
}

void ChestMonster::LateUpdate(GameTime gameTime) {
        TickAnimation(gameTime);
}

void ChestMonster::Draw(sf::RenderWindow& window, GameTime gameTime) {
        window.draw(
            texture_quads,
            &asset_manager.getChestMonsterFrame_ptr(frame())->texture
        );
}