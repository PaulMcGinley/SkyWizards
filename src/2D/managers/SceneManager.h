//
// Created by Paul McGinley on 23/04/2025.
//

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <unordered_map>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Enumerators/SceneType.h"
#include "interfaces/IUpdate.h"
#include "interfaces/IDraw.h"
#include "models/GameTime.h"

class IScene;

class SceneManager final : public IUpdate, public IDraw {
public:
        // Method to get the instance of the singleton
        static SceneManager& GetInstance();

        // Delete copy constructor and assignment operator to prevent copies
        SceneManager(const SceneManager&) = delete;
        void operator=(const SceneManager&) = delete;

        // Add scene to the scene manager map
        void AddScene(SceneType name, std::shared_ptr<IScene> scene);

        // Initialize a scene by name (enum SceneType)
        void InitializeScene(SceneType name);

        // Get a scene by name (enum SceneType) as a shared pointer
        std::shared_ptr<IScene> GetScene(SceneType name);

        // Remove a scene by name (enum SceneType)
        void RemoveScene(SceneType name);

        // Change the current scene by name (enum SceneType)
        void ChangeScene(SceneType name);

        // Get the current scene as a shared pointer
        std::shared_ptr<IScene> GetCurrentScene();

        // Update the current scene
        void Update(GameTime game_time) override;

        // Late update the current scene
        void LateUpdate(GameTime game_time) override;

        // Draw the current scene
        void Draw(sf::RenderWindow& window, GameTime game_time) override;

        // Get the type of the current scene
        SceneType GetCurrentSceneType() const {
                for (const auto &pair: scenes) {
                        if (pair.second == currentScene) {
                                return pair.first;
                        }
                }
        }

private:
        // Private constructor to prevent instancing
        SceneManager() = default;

        // Map to hold scenes <SceneType, IScene>
        std::unordered_map<SceneType, std::shared_ptr<IScene>> scenes;

        // Pointer to the current scene
        std::shared_ptr<IScene> currentScene;
};

#endif //SCENEMANAGER_H