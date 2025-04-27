//
// Created by Paul McGinley on 24/04/2025.
//

#ifndef GAMESCENE_H
#define GAMESCENE_H
#include "interfaces/IScene.h"
#include "objects/Player.h"


class GameScene : public IScene {
public:
        GameScene();

        void LoadMap(std::string name) {
                mapName = std::move(name);
                LoadAssets();
        }
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;

private:
        std::string mapName;
        WMap* map;
        int mapWidth; // Used to track the map width (End point + 50)

        sf::Texture* skyBoxTexture;
        sf::Sprite skyBoxSprite;

        sf::Texture* mountainsTexture;
        sf::Sprite mountainsSprite;

        void ValidateMap();
        void LoadSky();
        void LoadMountains();
        void LoadAssets();
        // void CalculateRectangle();

        Player player = Player();
        sf::View viewport;

};



#endif //GAMESCENE_H
