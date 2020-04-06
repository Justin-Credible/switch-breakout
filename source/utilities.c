
#include "utilities.h"

// https://silentmatt.com/rectangle-intersection/
// https://stackoverflow.com/a/306332
bool checkOverlap(struct RECT RectA, struct RECT RectB)
{
    return RectA.x1 < RectB.x2 && RectA.x2 > RectB.x1 &&
            RectA.y1 < RectB.y2 && RectA.y2 > RectB.y1;
}
