//
// Created by Paul McGinley on 22/04/2025.
//

#include "BoundaryGroup.h"

bool BoundaryGroup::Deserialize(const pugi::xml_node &node) {
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
        pugi::xml_node boundariesNode = node.child("Boundaries");
        if (!boundariesNode) {
                return false; // Boundaries container is required
        }

        // Iterate through the Boundary elements inside the Boundaries container
        for (pugi::xml_node boundaryNode: boundariesNode.children("Boundary")) {
                Boundary boundary;
                if (boundary.Deserialize(boundaryNode)) {
                        Boundaries.push_back(boundary);
                } else {
                        return false; // Failed to deserialize a Boundary
                }
        }

        return true; // Successfully deserialized
}
