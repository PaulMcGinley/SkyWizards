//
// Created by Paul McGinley on 22/04/2025.
//

#ifndef WMAP_H
#define WMAP_H

#include <string>
#include <vector>
#include "WMObject.h"

class WMap {
public:
        std::string FileName;

        int ParallaxBackgroundIndex;
        int MountainsBackgroundIndex;
        std::vector<WMObject> LevelObjects;
        std::vector<std::string> Scripts;
};

#endif //WMAP_H
