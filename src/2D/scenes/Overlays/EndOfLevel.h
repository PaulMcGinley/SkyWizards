//
// Created by Paul McGinley on 25/05/2025.
//

#ifndef ENDOFLEVEL_H
#define ENDOFLEVEL_H

#include <SFML/Graphics/Sprite.hpp>


#include "interfaces/IDraw.h"
#include "interfaces/IScene.h"
#include "interfaces/IUpdate.h"
#include "models/Score.h"

class EndOfLevel : public IScene {
public:
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;


        void ResetBoard();
        void SetSlimeData(int available, int killed);
        void SetChestMonsterData(int available, int killed);
        void SetAngryMushroomData(int available, int killed);
        void SetCactusMonsterData(int available, int killed);
        void SetCoinData(int available, int collected);
        void SetTimeTaken(int timeInMilliseconds);
        void CalculatePercentComplete();
        void SetMapName(const std::string & string);

private:
        std::string mapName;
        sf::RectangleShape shade;
        sf::Sprite rating;

        int slimesAvailable = 0;
        int slimesKilled = 0;
        int chestMonstersAvailable = 0;
        int chestMonstersKilled = 0;
        int angryMushroomsAvailable = 0;
        int angryMushroomsKilled = 0;
        int cactusMonstersAvailable = 0;
        int cactusMonstersKilled = 0;
        int coinsAvailable = 0;
        int coinsCollected = 0;
        int timeTaken = 0; // in milliseconds
        float percentComplete = 0.f; // Percentage of the level completed
};

#endif //ENDOFLEVEL_H
