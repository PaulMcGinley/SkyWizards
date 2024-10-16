//
// Created by Paul McGinley on 10/10/2024.
//

#pragma once

#include "enums/SceneType.h"
#include "interfaces/IScene.h"
#include <unordered_map>
#include <utility>

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

  // Method to add a scene
  void addScene(const SceneType name, std::shared_ptr<IScene> scene) {
    scenes[name] = std::move(scene); // Move the scene into the container

    scenes[name]->Scene_Init();
  }

  // Method to get a scene
  std::shared_ptr<IScene> getScene(const SceneType name) {
    return scenes[name];
  }

  // Method to remove a scene
  void removeScene(const SceneType name) {
    scenes[name]->Scene_Destroy();
    scenes.erase(name);
  }

  // Method to change the current scene
  void changeScene(const SceneType name) {
    if (!scenes.contains(name))
      return;

    if (currentScene)
      currentScene->OnScene_Deactive();

    currentScene = scenes[name];
    currentScene->OnScene_Active();
  }

  // Method to get the current scene
  std::shared_ptr<IScene> getCurrentScene() {
    return currentScene;
  }

  void Update(GameTime gameTime) override {
    if (currentScene)
      currentScene->Update(gameTime);
  }

  void LateUpdate(GameTime gameTime) override {
    if (currentScene)
      currentScene->LateUpdate(gameTime);
  }

  void Draw(sf::RenderWindow& window, GameTime gameTime) override {
    if (currentScene)
      currentScene->Draw(window, gameTime);
  }

private:
  // Private constructor to prevent instancing
  SceneManager() = default;

  // Container to hold scenes
  std::unordered_map<SceneType, std::shared_ptr<IScene>> scenes;

  // Pointer to the current scene
  std::shared_ptr<IScene> currentScene;
};