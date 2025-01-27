//
// Created by Paul McGinley on 08/10/2024.
//
#ifndef ISCENE_H
#define ISCENE_H

#include "IDraw.h"
#include "IUpdate.h"

class IScene : public IUpdate, public IDraw {
public:
        IScene() = default;
        virtual ~IScene() = default;

        [[nodiscard]] bool IsInitialized() const { return initialized; }

        void update(GameTime gameTime) override = 0;
        void lateUpdate(GameTime gameTime) override = 0;
        void draw(sf::RenderWindow& window, GameTime gameTime) override = 0;

        virtual void initializeScene() { initialized = true; };
        virtual void destroyScene() = 0;
        virtual void onScene_Active() = 0;
        virtual void onScene_Deactivate() = 0;

private:
        bool initialized = false;
};

#endif //ISCENE_H
