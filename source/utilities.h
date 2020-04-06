
#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <switch.h>

struct RECT
{
    int x1;
    int y1;
    int x2;
    int y2;
};

bool checkOverlap(struct RECT RectA, struct RECT RectB);

#endif
