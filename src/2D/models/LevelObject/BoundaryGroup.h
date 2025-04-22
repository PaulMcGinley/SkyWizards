//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef BOUNDARYGROUP_H
#define BOUNDARYGROUP_H

#include <vector>
#include "Boundary.h"

class BoundaryGroup {
public:
        int Layer;
        std::vector<Boundary> Boundaries;
};

#endif //BOUNDARYGROUP_H
