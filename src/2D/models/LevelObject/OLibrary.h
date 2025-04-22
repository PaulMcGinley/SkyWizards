//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef OLIBRARY_H
#define OLIBRARY_H

#include <3rdParty/pugixml/pugixml.hpp>
#include <string>

#include "BoundaryGroup.h"
#include "Graphic.h"

class OLibrary {
public:
        std::string FileName;
        std::vector<Graphic> Images;
        std::vector<BoundaryGroup> BoundaryGroups;
        bool deserialize(const pugi::xml_document &doc);
};

#endif //OLIBRARY_H
