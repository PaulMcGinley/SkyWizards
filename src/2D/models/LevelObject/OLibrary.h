//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef OLIBRARY_H
#define OLIBRARY_H

#include <string>
#include "BoundaryGroup.h"
#include "Graphic.h"

class OLibrary {
public:
        std::string FileName;
        std::vector<Graphic> Images;
        std::vector<BoundaryGroup> BoundaryGroups;
};

#endif //OLIBRARY_H
