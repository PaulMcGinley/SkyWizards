//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef BOUNDARYGROUP_H
#define BOUNDARYGROUP_H

#include <3rdParty/pugixml/pugixml.hpp>
#include <vector>

#include "Boundary.h"

class BoundaryGroup {
public:
        int Layer;
        std::vector<Boundary> Boundaries;
        bool Deserialize(const pugi::xml_node &node);
};

#endif //BOUNDARYGROUP_H
