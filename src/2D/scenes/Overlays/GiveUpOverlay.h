//
// Created by Paul McGinley on 01/06/2025.
//

#ifndef GIVEUPOVERLAY_H
#define GIVEUPOVERLAY_H
#include "interfaces/IScene.h"


class GiveUpOverlay : public IScene{
public:
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override;
        void InitializeScene() override;
        void DestroyScene() override;
        void OnScene_Activate() override;
        void OnScene_Deactivate() override;

        bool SetActive();

private:
        sf::RectangleShape shade;

        int index;
        bool active = false;
};



#endif //GIVEUPOVERLAY_H
