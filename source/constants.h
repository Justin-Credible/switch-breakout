
#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// Size of the bitmapped font from font.h.
#define FONT_WIDTH 8
#define FONT_HEIGHT 8

// How much we want to scale the bitmapped font up.
#define FONT_SCALE 3

// Define the desired framebuffer resolution (here we set it to 720p).
#define FB_WIDTH  1280
#define FB_HEIGHT 720

// We'll scale that down by 8.
#define PIXEL_SCALE 8

// Which leaves us with 160x90 "virtual pixels".
#define WORLD_WIDTH 160
#define WORLD_HEIGHT 90

// Each block will be 8x4 "virtual pixels".
#define BLOCK_WIDTH 8
#define BLOCK_HEIGHT 4

// We'll have 17 blocks per row and 10 rows.
#define BLOCKS_COLUMNS 17
#define BLOCKS_ROWS 10

// The size of the paddle.
#define PADDLE_WIDTH 12
#define PADDLE_HEIGHT 2

// The size of the ball.
#define BALL_WIDTH 2
#define BALL_HEIGHT 2

#endif
