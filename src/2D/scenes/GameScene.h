//
// Created by Paul McGinley on 24/04/2025.
//

#ifndef GAMESCENE_H
#define GAMESCENE_H
#include <SFML/Graphics/Text.hpp>


#include "Overlays/EndOfLevel.h"
#include "interfaces/IScene.h"
#include "models/LevelObject/Boundary.h"
#include "models/LevelObject/Collectable.h"
#include "models/Projectile.h"
#include "objects/BigCoin.h"
#include "objects/Player.h"
#include "objects/UICoin.h"
#include "objects/mobs/ChestMonster.h"


class GameScene : public IScene {
public:
        GameScene();

        void LoadMap(std::string name);
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;

        void DamagePlayer(int amount);

        void AddProjectile(std::unique_ptr<Projectile> projectile) { projectiles.push_back(std::move(projectile)); }

        void MewGame();

private:
        void (GameScene::*UpdateLoop)(GameTime);
        void Update_Loading(GameTime gameTime);
        void Update_Game(GameTime gameTime);
        void Update_EndOfLevel(GameTime gameTime);
        float startTime = 0.f;

        std::string mapName;
        WMap* map;
        BigCoin bigCoin;
        UICoin uiCoin;

        sf::Texture* skyBoxTexture;
        sf::Sprite skyBoxSprite;

        sf::Texture* mountainsTexture;
        sf::Sprite mountainsSprite;

        sf::RectangleShape backgroundShade = sf::RectangleShape(sf::Vector2f(1920, 1080));

        void ValidateMap();
        void LoadSky();
        void LoadMountains();
        void LoadAssets();
        void LoadMobs();
        void LoadCollectables();
        void CheckProjectileCollisions(GameTime gameTime);

        void CalculateParallaxBackground();
        void DrawBehindEntities(sf::RenderWindow& window, GameTime gameTime);
        void DrawEntities(sf::RenderWindow& window, GameTime gameTime);
        void DrawInFrontOfEntities(sf::RenderWindow& window, GameTime gameTime);
        void DEBUG_DrawMapBoundaries(sf::RenderWindow& window, GameTime gameTime);
        std::vector<Boundary> getLocalBoundaries() const;

        Player player = Player(this);
        void SpawnPlayer();
        sf::View viewport;

        std::vector<std::unique_ptr<Mob>> monsters;
        std::vector<std::unique_ptr<Projectile>> projectiles;
        std::vector<std::unique_ptr<Collectable>> collectables;

        float levelStartTime = 0.f;
        float levelEndTime = 0.f;
        std::string levelTime(GameTime gameTime, bool withMilliseconds);

        sf::Text timerText;
        sf::Font timerFont;

        float LevelScorePercent();

        std::shared_ptr<EndOfLevel> summaryOverlay;
        sf::Texture deadLine;
        sf::Sprite deadLineSprite;
        sf::RectangleShape blackness;
};



#endif //GAMESCENE_H
