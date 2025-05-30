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
        void SetMobData(int available, int killed);
        void SetCoinData(int available, int collected);
        void SetTimeTaken(int timeInMilliseconds);
        void CalculatePercentComplete();
        void SetMapName(const std::string & string);

private:
        std::string mapName;
        sf::RectangleShape shade;
        sf::Sprite rating;

        int mobsAvailable = 0;
        int mobsKilled = 0;
        int coinsAvailable = 0;
        int coinsCollected = 0;
        int timeTaken = 0; // in milliseconds
        int percentComplete = 0; // Percentage of the level completed
};

#endif //ENDOFLEVEL_H
