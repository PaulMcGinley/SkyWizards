//
// Created by Paul McGinley on 23/04/2025.
//

#include "WMap.h"

bool WMObject::deserialize(const pugi::xml_node& node) {
        if (!node) {
                return false; // Node is invalid
        }

        // Deserialize ObjectLibrary
        pugi::xml_node objectLibraryNode = node.child("ObjectLibrary");
        if (objectLibraryNode) {
                ObjectLibrary = objectLibraryNode.text().as_string();
        } else {
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
                        return false; // X is required
                }
                if (it != floatNodes.end()) {
                        Position.y = it->text().as_float();
                } else {
                        return false; // Y is required
                }
        } else {
                return false; // Position is required
        }

        return true; // Successfully deserialized
}