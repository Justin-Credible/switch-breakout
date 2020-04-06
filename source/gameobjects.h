
#ifndef _GAMEOBJECTS_H
#define _GAMEOBJECTS_H

#include "constants.h"

struct BLOCK
{
    int x;
    int y;
    bool alive;
};

struct BALL
{
    int x;
    int y;
    int travelX;
    int travelY;
};

struct PADDLE
{
    int x;
    int y;
};

struct WORLD_STATE
{
    struct BLOCK blocks[BLOCKS_ROWS * BLOCKS_COLUMNS];
    struct BALL ball;
    struct PADDLE paddle;
    int balls;
    int score;
    bool gameover;
};

#endif
