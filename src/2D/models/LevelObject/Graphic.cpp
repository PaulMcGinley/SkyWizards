//
// Created by Paul McGinley on 22/04/2025.
//

#include "Graphic.h"

bool Graphic::deserialize(const pugi::xml_node& node) {
    if (!node) {
        return false; // Node is invalid
    }

    // Deserialize BackImageLibrary
    pugi::xml_node backImageLibraryNode = node.child("BackImageLibrary");
    if (backImageLibraryNode) {
        BackImageLibrary = backImageLibraryNode.text().as_string();
    } else {
        return false; // BackImageLibrary is required
    }

    // Deserialize BackIndex
    pugi::xml_node backIndexNode = node.child("BackIndex");
    if (backIndexNode) {
        BackIndex = backIndexNode.text().as_int();
    } else {
        return false; // BackIndex is required
    }

    // Deserialize BackEndIndex
    pugi::xml_node backEndIndexNode = node.child("BackEndIndex");
    if (backEndIndexNode) {
        BackEndIndex = backEndIndexNode.text().as_int();
    } else {
        return false; // BackEndIndex is required
    }

    // Deserialize BackAnimationSpeed
    pugi::xml_node backAnimationSpeedNode = node.child("BackAnimationSpeed");
    if (backAnimationSpeedNode) {
        BackAnimationSpeed = backAnimationSpeedNode.text().as_int();
    } else {
        return false; // BackAnimationSpeed is required
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

    // Deserialize DrawLayer
    pugi::xml_node drawLayerNode = node.child("DrawLayer");
    if (drawLayerNode) {
        DrawLayer = drawLayerNode.text().as_int();
    } else {
        return false; // DrawLayer is required
    }

    return true; // Successfully deserialized
}