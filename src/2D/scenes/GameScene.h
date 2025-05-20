//
// Created by Paul McGinley on 24/04/2025.
//

#ifndef GAMESCENE_H
#define GAMESCENE_H
#include "interfaces/IScene.h"
#include "models/LevelObject/Boundary.h"
#include "models/LevelObject/Collectable.h"
#include "models/Projectile.h"
#include "objects/Player.h"
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
        void OnScene_Active() override;
        void OnScene_Deactivate() override;

        void DamagePlayer(int amount);

        void AddProjectile(std::unique_ptr<Projectile> projectile) {
                projectiles.push_back(std::move(projectile));
        }

private:
        void (GameScene::*UpdateLoop)(GameTime);
        void Update_Loading(GameTime gameTime);
        void Update_Game(GameTime gameTime);
        float startTime =0.f;

        std::string mapName;
        WMap* map;

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
        void CheckProjectileCollisions();

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

};



#endif //GAMESCENE_H
