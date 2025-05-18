//
// Created by Paul McGinley on 22/04/2025.
//

#include "OLibrary.h"

#include <iostream>

bool OLibrary::deserialize(const pugi::xml_document &doc) {
        // Clear existing data
        Images.clear();
        BoundaryGroups.clear();

        // Get the root node
        pugi::xml_node root = doc.child("OLibrary");
        if (!root) {
                return false; // Root node not found
        }

        // Deserialize Images
        for (pugi::xml_node imageNode: root.child("Images").children("Graphic")) {
                Graphic graphic;
                if (graphic.deserialize(imageNode)) {
                        Images.push_back(graphic);
                } else {
                        return false; // Failed to deserialize a Graphic
                }
        }

        // Deserialize BoundaryGroups
        for (pugi::xml_node boundaryGroupNode: root.child("BoundaryGroups").children("BoundaryGroup")) {
                BoundaryGroup boundaryGroup;
                if (boundaryGroup.deserialize(boundaryGroupNode)) {
                        int layer = boundaryGroup.Layer;

                        // Find the corresponding Image by layer index
                        bool foundMatchingLayer = false;
                        for (size_t i = 0; i < Images.size(); ++i) {
                                if (i == static_cast<size_t>(layer)) {
                                        // Add the boundary group to our collection first
                                        BoundaryGroups.push_back(boundaryGroup);
                                        // Then establish the link (after it's safely in the vector)
                                        Images[i].Boundaries = &BoundaryGroups.back().Boundaries;
                                        foundMatchingLayer = true;
                                        std::cout << "Boundary Found matching layer: " << layer << std::endl;
                                        break;
                                }
                        }

                        if (!foundMatchingLayer) {
                                std::cerr << "Warning: BoundaryGroup references non-existent layer " << layer << std::endl;
                                BoundaryGroups.push_back(boundaryGroup);
                        }
                } else {
                        std::cerr << "Failed to deserialize a BoundaryGroup" << std::endl;
                        return false; // Failed to deserialize a BoundaryGroup
                }
        }

        return true; // Successfully deserialized
}
void OLibrary::Update(GameTime gameTime) {
        // TODO: Change to for i loop to match to boundary groups
        for (auto& image : Images) {
                // Not animated
                if (image.BackEndIndex == -1)
                        continue;

                // Not ready for next frame
                if(!gameTime.TimeElapsed(image.nextFrameTime))
                        continue;
                // if (gameTime.total_game_time < image.nextFrameTime)
                //         continue;

                // Update the next frame time
                image.currentFrame++;
                if (image.currentFrame >= image.BackEndIndex) {
                        image.currentFrame = image.BackIndex; // Loop back to the start
                }

                // Update next frame time
                image.nextFrameTime = gameTime.total_game_time + (static_cast<float>(image.BackAnimationSpeed) / 1000.0f);
        }

        // TODO: Update the boundary groups
}
void OLibrary::LateUpdate(GameTime gameTime) {
        // Not implemented
}
void OLibrary::Draw(sf::RenderWindow &window, GameTime gameTime) {

}
