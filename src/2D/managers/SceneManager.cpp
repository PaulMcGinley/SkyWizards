//
// Created by Paul McGinley on 10/10/2024.
//

#include "SceneManager.h"
#include "interfaces/IScene.h"

#include <memory>

// Method to get the instance of the singleton
SceneManager& SceneManager::GetInstance() {
    static SceneManager instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
}

// Add scene to the scene manager map
void SceneManager::AddScene(const SceneType name, std::shared_ptr<IScene> scene) {
    // Move the scene into the map
    scenes[name] = std::move(scene);

    // Initialize the scene
    InitializeScene(name);
}

// Initialize a scene by name (enum SceneType)
void SceneManager::InitializeScene(const SceneType name) {
    // Check if the scene exists
    if (scenes.find(name) == scenes.end())
        return;

    // Check if the scene is already initialized
    if (scenes[name]->IsInitialized())
        return;

    // Initialize the scene
    scenes[name]->InitializeScene();
}

// Get a scene by name (enum SceneType) as a shared pointer
std::shared_ptr<IScene> SceneManager::GetScene(const SceneType name) {
     if (scenes.find(name) != scenes.end()) {
        return scenes[name];
    }
    return nullptr; // Or handle error appropriately
}

// Remove a scene by name (enum SceneType)
void SceneManager::RemoveScene(const SceneType name) {
    // Check if the scene exists
    if (scenes.find(name) == scenes.end())
        return;

    // If the scene to be removed is the current scene, deactivate and clear it
    if (currentScene == scenes[name]) {
        currentScene->OnScene_Deactivate();
        currentScene = nullptr;
    }

    // Destroy the scene
    scenes[name]->DestroyScene();

    // Remove the scene from the map
    scenes.erase(name);
}

// Change the current scene by name (enum SceneType)
void SceneManager::ChangeScene(const SceneType name) {
    // Check if the scene exists
    if (scenes.find(name) == scenes.end())
        return;

    // Check if there is an active scene, then deactivate it
    if (currentScene)
        currentScene->OnScene_Deactivate();

    // Set the current scene to the new scene
    currentScene = scenes[name];

    // Ensure the new scene is initialized
    if (!currentScene->IsInitialized()) {
        currentScene->InitializeScene();
    }

    // Activate the new scene
    currentScene->OnScene_Active();
}

// Get the current scene as a shared pointer
std::shared_ptr<IScene> SceneManager::GetCurrentScene() {
    return currentScene;
}

// Update the current scene
void SceneManager::Update(const GameTime game_time) {
    // Check if there is a current scene, then call the update
    if (currentScene)
        currentScene->Update(game_time);
}

// Late update the current scene
void SceneManager::LateUpdate(const GameTime game_time) {
    // Check if there is a current scene, then call the late update
    if (currentScene)
        currentScene->LateUpdate(game_time);
}

// Draw the current scene
void SceneManager::Draw(sf::RenderWindow& window, const GameTime game_time) {
    // Check if there is a current scene, then call the draw
    if (currentScene)
        currentScene->Draw(window, game_time);
}