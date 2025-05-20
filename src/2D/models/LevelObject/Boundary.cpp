//
// Created by Paul McGinley on 22/04/2025.
//

#include "Boundary.h"

bool Boundary::Deserialize(const pugi::xml_node &node) {
        if (!node) {
                return false; // Node is invalid
        }

        // Deserialize X
        pugi::xml_node xNode = node.child("X");
        if (xNode) {
                X = xNode.text().as_int();
        } else {
                return false; // X is required
        }

        // Deserialize Y
        pugi::xml_node yNode = node.child("Y");
        if (yNode) {
                Y = yNode.text().as_int();
        } else {
                return false; // Y is required
        }

        // Deserialize Width
        pugi::xml_node widthNode = node.child("Width");
        if (widthNode) {
                Width = widthNode.text().as_int();
        } else {
                return false; // Width is required
        }

        // Deserialize Height
        pugi::xml_node heightNode = node.child("Height");
        if (heightNode) {
                Height = heightNode.text().as_int();
        } else {
                return false; // Height is required
        }

        // Deserialize Active
        pugi::xml_node activeNode = node.child("Active");
        if (activeNode) {
                Active = activeNode.text().as_bool();
        } else {
                return false; // Active is required
        }

        return true; // Successfully deserialized
}
