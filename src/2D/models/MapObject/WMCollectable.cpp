//
// Created by Paul McGinley on 18/05/2025.
//

#include "WMCollectable.h"

#include <iostream>
bool WMCollectable::Deserialize(const pugi::xml_node &node) {
        if (!node) {
                std::cerr << "Error: Invalid XML node provided for deserialization." << std::endl;
                return false; // Node is invalid
        }

        // Deserialize ObjectLibrary
        pugi::xml_node objectLibraryNode = node.child("ObjectLibrary");
        if (objectLibraryNode) {
                CollectableName = objectLibraryNode.text().as_string();
        } else {
                std::cerr << "Error: Missing ObjectLibrary." << std::endl;
                return false; // ObjectLibrary is required
        }

        // Deserialize Position
        pugi::xml_node positionNode = node.child("Position");
        if (positionNode) {
                auto floatNodes = positionNode.children("float");
                auto it = floatNodes.begin();
                if (it != floatNodes.end()) {
                        Position.x = it->text().as_float();
                        ++it;
                } else {
                        std::cerr << "Error: Missing X position." << std::endl;
                        return false; // X is required
                }
                if (it != floatNodes.end()) {
                        Position.y = it->text().as_float();
                } else {
                        std::cerr << "Error: Missing Y position." << std::endl;
                        return false; // Y is required
                }
        } else {
                std::cerr << "Error: Missing Position." << std::endl;
                return false; // Position is required
        }

        // Deserialize Action
        pugi::xml_node actionNode = node.child("Action");
        if (actionNode) {
                Action = actionNode.text().as_string();
        } else {
                std::cerr << "Error: Missing Action." << std::endl;
                //return false; // Action is required
        }

        // Deserialize CollectedSound
        pugi::xml_node collectedSoundNode = node.child("CollectedSound");
        if (collectedSoundNode) {
                CollectedSound = collectedSoundNode.text().as_string();
        } else {
                std::cerr << "Error: Missing CollectedSound." << std::endl;
                //return false; // CollectedSound is required
        }

        return true; // Successfully deserialized
}
