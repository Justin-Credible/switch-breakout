
#ifndef _GAMELOGIC_H
#define _GAMELOGIC_H

#include "gameobjects.h"

struct WORLD_STATE initWorld();
struct WORLD_STATE updateWorld(struct WORLD_STATE worldState, u64 keysDown);

#endif
