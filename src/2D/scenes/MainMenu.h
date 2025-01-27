//
// Created by Paul McGinley on 25/01/2025.
//

#ifndef MAINMENU_H
#define MAINMENU_H


#include "interfaces/IScene.h"


class MainMenu : public IScene {
public:
        // MainMenu();

        void update(GameTime gameTime) override;
        void lateUpdate(GameTime gameTime) override;
        void draw(sf::RenderWindow& window, GameTime gameTime) override;
        void initializeScene() override;
        void destroyScene() override;
        void onScene_Active() override;
        void onScene_Deactivate() override;

private:
        sf::VertexArray backgroundQuad{sf::Quads, 4};
};



#endif //MAINMENU_H
