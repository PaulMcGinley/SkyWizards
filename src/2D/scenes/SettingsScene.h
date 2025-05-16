//
// Created by Paul McGinley on 16/05/2025.
//

#ifndef SETTINGSSCENE_H
#define SETTINGSSCENE_H
#include "interfaces/IScene.h"


class SettingsScene : public IScene{
public:
        SettingsScene();
        ~SettingsScene() override = default;
        void Update(GameTime gameTime) override;
        void LateUpdate(GameTime gameTime) override;
        void Draw(sf::RenderWindow &window, GameTime gameTime) override;
        void OnScene_Active() override;
        void OnScene_Deactivate() override;
        void DestroyScene() override;
        void InitializeScene() override;
};



#endif //SETTINGSSCENE_H
