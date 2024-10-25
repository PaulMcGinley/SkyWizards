using System.Collections.Generic;

namespace LevelObjectEditor;

public struct Element
{
    int backImageIndex;
    int frontImageIndex;
    private List<(int, int)> bounds;
}