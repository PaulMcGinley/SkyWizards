//
// Created by Paul McGinley on 03/10/2024.
//

#include "ChestMonster.h"

#include "models/TextureEntry.h"

ChestMonster::ChestMonster() {

    position = {1000, 520};
    sequences = {
        {AniType::Attack, {0, 10, 100}},
        {AniType::Attack2, {10, 9, 100}},
        {AniType::BattleIdle, {19, 9, 100}},
        {AniType::Damaged, {28, 7, 100}},
        {AniType::Death, {35, 12, 100}},
        {AniType::Dizzy, {47, 16, 100}},
        {AniType::Idle, {63, 16, 60}},
        {AniType::Static, {79 , 1, 100}}, // should be length 1
        {AniType::Run, {87, 7, 100}},
        {AniType::SenseSomething, {95, 56, 100}},
        {AniType::Taunt, {151, 24, 100}},
        {AniType::Victory, {185, 12, 100}},
        {AniType::Walk, {187, 12, 100}}

    };
       // ChangeAni(AniType::Taunt, GameTime());
}

void ChestMonster::Update(GameTime gameTime) {
    // AI Code here

    // Walk AI
    if (currentAni == AniType::Walk) {
        // Calculate how much to move
        float moveDistance =  walkSpeed * gameTime.deltaTime;

        switch (faceDirection) {
            case FaceDirection::Left:
                position.x -= moveDistance;
            break;
            case FaceDirection::ChestMonsterRight:
                position.x += moveDistance;
            break;
        }
    }

    // Run AI
    if (currentAni == AniType::Run) {
        // Calculate how much to move
        float moveDistance =  runSpeed * gameTime.deltaTime;

        switch (faceDirection) {
            case FaceDirection::Left:
                position.x -= moveDistance;
            break;
            case FaceDirection::ChestMonsterRight:
                position.x += moveDistance;
            break;
        }
    }



    //AssetManager& assetManager = AssetManager::getInstance();
    TextureEntry* entry = assetManager.GetChestMonsterFrame_ptr(frame());

    texQuad[0].texCoords = entry->texQuad[0].texCoords;
    texQuad[1].texCoords = entry->texQuad[1].texCoords;
    texQuad[2].texCoords = entry->texQuad[2].texCoords;
    texQuad[3].texCoords = entry->texQuad[3].texCoords;

    texQuad[0].position = entry->texQuad[0].position + position;
    texQuad[1].position = entry->texQuad[1].position + position;
    texQuad[2].position = entry->texQuad[2].position + position;
    texQuad[3].position = entry->texQuad[3].position + position;
}

void ChestMonster::LateUpdate(GameTime gameTime) {
    TickAnimation(gameTime);
}

void ChestMonster::Draw(sf::RenderWindow& window, GameTime gameTime) {
    window.draw(
        texQuad,
        &assetManager.GetChestMonsterFrame_ptr(frame())->texture
    );
}