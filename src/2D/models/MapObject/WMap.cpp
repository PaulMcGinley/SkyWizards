//
// Created by Paul McGinley on 23/04/2025.
//

#include <iostream>
#include "WMap.h"

bool WMap::deserialize(const pugi::xml_node& node) {
        if (!node) {
                std::cerr << "Error: Invalid XML node provided for deserialization." << std::endl;
                return false;
        }

        pugi::xml_node root = node.child("WMap");
        if (!root) {
                return false; // Root node not found
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
                for (pugi::xml_node objectNode : levelObjectsNode.children("WMObject")) {
                        WMObject levelObject;
                        if (levelObject.deserialize(objectNode)) {
                                LevelObjects.push_back(levelObject);
                                std::cout << "Successfully deserialized a WMObject." << std::endl;
                        } else {
                                std::cerr << "Error: Failed to deserialize a WMObject." << std::endl;
                                return false;
                        }
                }
        } else {
                std::cerr << "Error: Missing 'LevelObjects' node." << std::endl;
                return false; // LevelObjects are required
        }

        std::cout << "Successfully deserialized WMap." << std::endl;
        return true; // Successfully deserialized
}