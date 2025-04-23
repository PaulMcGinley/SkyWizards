//
// Created by Paul McGinley on 10/10/2024.
//

#pragma once

#include <unordered_map>
#include <utility>
#include "Enumerators/SceneType.h"
#include "interfaces/IScene.h"

class SceneManager : public IUpdate, public IDraw {
public:
        // Method to get the instance of the singleton
        static SceneManager& getInstance() {
                static SceneManager instance; // Guaranteed to be destroyed.
                // Instantiated on first use.
                return instance;
        }

        // Delete copy constructor and assignment operator to prevent copies
        SceneManager(const SceneManager&) = delete;
        void operator=(const SceneManager&) = delete;

        // Add scene to the scene manager map
        void addScene(const SceneType name, std::shared_ptr<IScene> scene) {

                // Move the scene into the map
                scenes[name] = std::move(scene);

                // Initialize the scene
                initializeScene(name);
        }

        // Initialize a scene by name (enum SceneType)
        void initializeScene(const SceneType name) {

                // Check if the scene exists
                if (!scenes.contains(name))
                        return;

                // Check if the scene is already initialized
                if (scenes[name]->IsInitialized())
                        return;

                // Initialize the scene
                scenes[name]->initializeScene();
        }

        // Get a scene by name (enum SceneType) as a shared pointer
        std::shared_ptr<IScene> getScene(const SceneType name) {
                return scenes[name];
        }

        // Remove a scene by name (enum SceneType)
        void removeScene(const SceneType name) {

                // Check if the scene exists
                if (!scenes.contains(name))
                        return;

                // Destroy the scene
                scenes[name]->destroyScene();

                // Remove the scene from the map
                scenes.erase(name);
        }

        // Change the current scene by name (enum SceneType)
        void changeScene(const SceneType name) {

                // Check if the scene exists
                if (!scenes.contains(name))
                        return;

                // Check if there is an active scene, then deactivate it
                if (currentScene)
                        currentScene->onScene_Deactivate();

                // Set the current scene to the new scene
                currentScene = scenes[name];

                // Activate the new scene
                currentScene->onScene_Active();
        }

        // Get the current scene as a shared pointer
        std::shared_ptr<IScene> getCurrentScene() {
                return currentScene;
        }

        // Update the current scene
        void update(const GameTime game_time) override {

                // Check if there is a current scene, then call the update
                if (currentScene)
                        currentScene->update(game_time);
        }

        // Late update the current scene
        void lateUpdate(const GameTime game_time) override {

                // Check if there is a current scene, then call the late update
                if (currentScene)
                        currentScene->lateUpdate(game_time);
        }

        // Draw the current scene
        void Draw(sf::RenderWindow& window, const GameTime game_time) override {

                // Check if there is a current scene, then call the draw
                if (currentScene)
                        currentScene->Draw(window, game_time);
        }

private:
        // Private constructor to prevent instancing
        SceneManager() = default;

        // Map to hold scenes <SceneType, IScene>
        std::unordered_map<SceneType, std::shared_ptr<IScene>> scenes;

        // Pointer to the current scene
        std::shared_ptr<IScene> currentScene;
};