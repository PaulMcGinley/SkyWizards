//
// Created by Paul McGinley on 23/04/2025.
//

#include "WMap.h"
#include <iostream>

#include <unordered_set>

#include "WMCollectable.h"

WMap::~WMap() = default;

bool WMap::Deserialize(const pugi::xml_node &node) {
        if (!node) {
                std::cerr << "Error: Invalid XML node provided for deserialization." << std::endl;
                return false;
        }

        pugi::xml_node root = node.child("WMap");
        if (!root) {
                return false; // Root node not found
        }

        pugi::xml_node startXNode = root.child("startXPos");
        if (startXNode) {
                startPosition.x = startXNode.text().as_float();
                std::cout << "Start X Position: " << startPosition.x << std::endl;
        } else {
                std::cerr << "Error: Missing 'startXPos' node." << std::endl;
                return false; // startXPos is required
        }

        pugi::xml_node startYNode = root.child("startYPos");
        if (startYNode) {
                startPosition.y = startYNode.text().as_float();
                std::cout << "Start Y Position: " << startPosition.y << std::endl;
        } else {
                std::cerr << "Error: Missing 'startYPos' node." << std::endl;
                return false; // startYPos is required
        }

        pugi::xml_node endXNode = root.child("endXPos");
        if (endXNode) {
                endPosition.setPosition(endXNode.text().as_float(), endPosition.getPosition().y);
                std::cout << "End X Position: " << endPosition.getPosition().x << std::endl;
        } else {
                std::cerr << "Error: Missing 'endXPos' node." << std::endl;
                return false; // endXPos is required
        }

        pugi::xml_node endYNode = root.child("endYPos");
        if (endYNode) {
                endPosition.setPosition(endPosition.getPosition().x, endYNode.text().as_float());
                std::cout << "End Y Position: " << endPosition.getPosition().y << std::endl;
        } else {
                std::cerr << "Error: Missing 'endYPos' node." << std::endl;
                return false; // endYPos is required
        }

        pugi::xml_node endWidthNode = root.child("endWidth");
        if (endWidthNode) {
                endPosition.setSize(sf::Vector2f(endWidthNode.text().as_float(), endPosition.getSize().y));
                std::cout << "End Width: " << endPosition.getSize().x << std::endl;
        } else {
                std::cerr << "Error: Missing 'endWidth' node." << std::endl;
                return false; // endWidth is required
        }

        pugi::xml_node endHeightNode = root.child("endHeight");
        if (endHeightNode) {
                endPosition.setSize(sf::Vector2f(endPosition.getSize().x, endHeightNode.text().as_float()));
                std::cout << "End Height: " << endPosition.getSize().y << std::endl;
        } else {
                std::cerr << "Error: Missing 'endHeight' node." << std::endl;
                return false; // endHeight is required
        }

        // Deserialize ParallaxBackgroundIndex
        pugi::xml_node parallaxNode = root.child("ParallaxBackgroundIndex");
        if (parallaxNode) {
                ParallaxBackgroundIndex = parallaxNode.text().as_int();
                std::cout << "ParallaxBackgroundIndex: " << ParallaxBackgroundIndex << std::endl;
        } else {
                std::cerr << "Error: Missing 'ParallaxBackgroundIndex' node." << std::endl;
                return false; // ParallaxBackgroundIndex is required
        }

        // Deserialize MountainsBackgroundIndex
        pugi::xml_node mountainsNode = root.child("MountainsBackgroundIndex");
        if (mountainsNode) {
                MountainsBackgroundIndex = mountainsNode.text().as_int();
                std::cout << "MountainsBackgroundIndex: " << MountainsBackgroundIndex << std::endl;
        } else {
                std::cerr << "Error: Missing 'MountainsBackgroundIndex' node." << std::endl;
                return false; // MountainsBackgroundIndex is required
        }

        // Deserialize LevelObjects
        pugi::xml_node levelObjectsNode = root.child("LevelObjects");
        if (levelObjectsNode) {
                for (pugi::xml_node objectNode: levelObjectsNode.children("WMObject")) {
                        WMObject levelObject;
                        if (levelObject.Deserialize(objectNode)) {
                                LevelObjects.push_back(levelObject);
                                std::cout << "Successfully deserialized WMObject: " << levelObject.ObjectLibraryFile << std::endl;
                        } else {
                                std::cerr << "Error: Failed to deserialize a WMObject." << std::endl;
                                return false;
                        }
                }
        } else {
                std::cerr << "Error: Missing 'LevelObjects' node." << std::endl;
                return false; // LevelObjects are required
        }

        // Deserialize Mobs
        pugi::xml_node mobsNode = root.child("Mobs");
        if (mobsNode) {
                for (pugi::xml_node mobNode: mobsNode.children("WMMob")) {
                        WMMob mob;
                        if (mob.Deserialize(mobNode)) {
                                Mobs.push_back(mob);
                                std::cout << "Successfully deserialized a WMMob: " << mob.MonsterName << std::endl;
                        } else {
                                std::cerr << "Error: Failed to deserialize a WMMob." << std::endl;
                                return false;
                        }
                }
        }

        // Deserialize Collectables
        pugi::xml_node collectablesNode = root.child("Collectables");
        if (collectablesNode) {
                for (pugi::xml_node collectableNode: collectablesNode.children("WMCollectable")) {
                        WMCollectable collectable;
                        if (collectable.Deserialize(collectableNode)) {
                                Collectables.push_back(collectable);
                                std::cout << "Successfully deserialized a WMCollectable: " << collectable.CollectableName << std::endl;
                        } else {
                                std::cerr << "Error: Failed to deserialize a WMCollectable." << std::endl;
                                return false;
                        }
                }
        }

        // Deserialize song
        pugi::xml_node songNode = root.child("song");
        if (songNode) {
                song = songNode.text().as_string();
                std::cout << "Song: " << song << std::endl;
        }

        std::cout << "Successfully deserialized WMap." << std::endl;
        return true; // Successfully deserialized
}
void WMap::Update(GameTime gameTime) { /* Nothing to do here */  }
void WMap::LateUpdate(GameTime gameTime) { /* Nothing to do here */  }
void WMap::Draw(sf::RenderWindow &window, GameTime gameTime) { /* Nothing to do here */  }
