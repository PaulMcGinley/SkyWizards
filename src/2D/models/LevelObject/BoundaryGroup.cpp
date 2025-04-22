//
// Created by Paul McGinley on 22/04/2025.
//

#include "BoundaryGroup.h"

bool BoundaryGroup::deserialize(const pugi::xml_node& node) {
        if (!node) {
                return false; // Node is invalid
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