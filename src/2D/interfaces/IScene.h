//
// Created by Paul McGinley on 08/10/2024.
//
#ifndef ISCENE_H
#define ISCENE_H

#include "IDraw.h"
#include "IUpdate.h"
#include "managers/GameManager.h"
#include "managers/SceneManager.h"

class IScene : public IUpdate, public IDraw {
public:
        IScene() = default;
        virtual ~IScene() = default;

        [[nodiscard]] bool IsInitialized() const { return initialized; }

        void Update(GameTime gameTime) override = 0;
        void LateUpdate(GameTime gameTime) override = 0;
        void Draw(sf::RenderWindow& window, GameTime gameTime) override = 0;

        virtual void InitializeScene() { initialized = true; };
        virtual void DestroyScene() = 0;
        virtual void OnScene_Activate() = 0;
        virtual void OnScene_Deactivate() = 0;

        GameManager& gameManager = GameManager::GetInstance();
        SceneManager& sceneManager = SceneManager::GetInstance();

private:
        bool initialized = false;
};

#endif //ISCENE_H
