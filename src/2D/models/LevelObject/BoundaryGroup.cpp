//
// Created by Paul McGinley on 22/04/2025.
//

#include "BoundaryGroup.h"

bool BoundaryGroup::deserialize(const pugi::xml_node& node) {
        if (!node) {
                return false; // Node is invalid
        }
        // Deserialize Layer
        pugi::xml_node layerNode = node.child("Layer");
        if (layerNode) {
                Layer = layerNode.text().as_int();
        } else {
                return false; // Layer is required
        }

        // Deserialize Boundaries
        for (pugi::xml_node boundaryNode : node.children("Boundary")) {
                Boundary boundary;
                if (boundary.deserialize(boundaryNode)) {
                        Boundaries.push_back(boundary);
                } else {
                        return false; // Failed to deserialize a Boundary
                }
        }

        return true; // Successfully deserialized
}