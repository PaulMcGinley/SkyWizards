//
// Created by Paul McGinley on 11/05/2025.
//

#include "WMMob.h"

#include <iostream>
bool WMMob::Deserialize(const pugi::xml_node &node) {
        if (!node) {
                std::cerr << "Error: Invalid XML node provided for deserialization." << std::endl;
                return false; // Node is invalid
        }

        // Deserialize ObjectLibrary
        pugi::xml_node objectLibraryNode = node.child("ObjectLibrary");
        if (objectLibraryNode) {
                MonsterName = objectLibraryNode.text().as_string();
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

        pugi::xml_node moveSpeedNode = node.child("MoveSpeed");
        if (moveSpeedNode) {
                MoveSpeed = moveSpeedNode.text().as_float();
        } else {
                std::cerr << "Error: Missing MoveSpeed." << std::endl;
                return false; // MoveSpeed is required
        }

        pugi::xml_node viewRangeNode = node.child("ViewRange");
        if (viewRangeNode) {
                ViewRange = viewRangeNode.text().as_float();
        } else {
                std::cerr << "Error: Missing ViewRange." << std::endl;
                return false; // ViewRange is required
        }

        pugi::xml_node healthNode = node.child("Health");
        if (healthNode) {
                Health = healthNode.text().as_int();
        } else {
                std::cerr << "Error: Missing Health." << std::endl;
                return false; // Health is required
        }

        return true;
}
