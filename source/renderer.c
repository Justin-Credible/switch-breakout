
#include <stdlib.h>

#include <switch.h>

#include "renderer.h"
#include "constants.h"
#include "gameobjects.h"
#include "font.h"

// Returns the color value for the row of blocks at the given
// row index (starting with index zero at the top of the screen).
u32 getBlockColorForRow(int row)
{
    switch (row)
    {
        case 0:
        case 1:
            return RGBA8_MAXALPHA(0xee, 0x40, 0x35);
        case 2:
        case 3:
            return RGBA8_MAXALPHA(0xf3, 0x77, 0x36);
        case 4:
        case 5:
            return RGBA8_MAXALPHA(0xfd, 0xf4, 0x98);
        case 6:
        case 7:
            return RGBA8_MAXALPHA(0x7b, 0xc0, 0x43);
        case 8:
        case 9:
            return RGBA8_MAXALPHA(0x03, 0x92, 0xcf);
        default:
            return RGBA8_MAXALPHA(0x00, 0x00, 0x00);
    }
}

void renderBlocks(struct WORLD_STATE worldState, u32 *framebuffer, u32 stride)
{
    for (int i = 0; i < BLOCKS_ROWS * BLOCKS_COLUMNS; i++)
    {
        struct BLOCK block = worldState.blocks[i];

        if (!block.alive)
            continue;

        for (u32 y = 0; y < BLOCK_HEIGHT * PIXEL_SCALE; y ++)
        {
            for (u32 x = 0; x < BLOCK_WIDTH * PIXEL_SCALE; x ++)
            {
                u32 color = getBlockColorForRow(i / BLOCKS_COLUMNS);

                u32 position = (block.y * PIXEL_SCALE + y) * stride / sizeof(u32) + (block.x * PIXEL_SCALE + x);
                framebuffer[position] = color;
            }
        }
    }

    return;
}

void renderBall(struct WORLD_STATE worldState, u32 *framebuffer, u32 stride)
{
    for (u32 y = 0; y < BALL_HEIGHT * PIXEL_SCALE; y ++)
    {
        for (u32 x = 0; x < BALL_WIDTH * PIXEL_SCALE; x ++)
        {
            u32 position = (worldState.ball.y * PIXEL_SCALE + y) * stride / sizeof(u32) + (worldState.ball.x * PIXEL_SCALE + x);
            framebuffer[position] = RGBA8_MAXALPHA(0xFA, 0xF0, 0xE6);
        }
    }

    return;
}

void renderPaddle(struct WORLD_STATE worldState, u32 *framebuffer, u32 stride)
{
    for (u32 y = 0; y < PADDLE_HEIGHT * PIXEL_SCALE; y ++)
    {
        for (u32 x = 0; x < PADDLE_WIDTH * PIXEL_SCALE; x ++)
        {
            u32 position = (worldState.paddle.y * PIXEL_SCALE + y) * stride / sizeof(u32) + (worldState.paddle.x * PIXEL_SCALE + x);
            framebuffer[position] = RGBA8_MAXALPHA(0xFD, 0xF5, 0xE6);
        }
    }

    return;
}

void renderFontChar(char chr, int originX, int originY, int scale, u32 *framebuffer, u32 stride)
{
    char *bitmapChar = font8x8_basic[chr];

    for (int y = 0; y < 8 * scale; y++)
    {
        int bitmapCharRow = bitmapChar[y / scale];

        for (int x = 0; x < 8 * scale; x++)
        {
            bool pixel = ((bitmapCharRow >> (x / scale)) & 0x01) > 0;

            u32 position = (originY * scale + y) * stride / sizeof(u32) + (originX * scale + x);

            if (pixel)
                framebuffer[position] = RGBA8_MAXALPHA(0xFF, 0xFF, 0xFF);
            else
                framebuffer[position] = RGBA8_MAXALPHA(0x00, 0x00, 0x00);
        }
    }
}

void renderScore(struct WORLD_STATE worldState, u32 *framebuffer, u32 stride)
{
    // Convert the score from an integer to an array of characters.
    // The bitmap font is conveniently indexed by character.
    int score = worldState.score;
    char scoreChars[5];
    itoa(score, scoreChars, 10);

    // Score will be at the top right.
    int y = 0;
    int x = FB_WIDTH - FONT_WIDTH;

    // Depending on what the score is, determines how many digits we'll render.
    int charCount = 1;

    // Determine how many digits to show and adjust the x offset to account for it.

    if (score >= 10)
    {
        charCount++;
        x -= FONT_WIDTH;
    }

    if (score >= 100)
    {
        charCount++;
        x -= FONT_WIDTH;
    }

    if (score >= 1000)
    {
        charCount++;
        x -= FONT_WIDTH;
    }

    if (charCount >= 10000)
    {
        x -= FONT_WIDTH;
        charCount++;
    }

    // Render each character.
    for (int i = 0; i < charCount; i++)
    {
        renderFontChar(scoreChars[i], x, y, FONT_SCALE, framebuffer, stride);
        x += FONT_WIDTH;
    }
}

void renderBallCount(struct WORLD_STATE worldState, u32 *framebuffer, u32 stride)
{
    // The bitmap font is conveniently indexed by character.
    int ballCount = worldState.balls;
    char ballCountChar[1];
    itoa(ballCount, ballCountChar, 10);

    int x = FB_WIDTH / FONT_SCALE - FONT_WIDTH;
    int y = FB_HEIGHT / FONT_SCALE - FONT_HEIGHT - 2;
    renderFontChar(ballCountChar[0], x, y, FONT_SCALE, framebuffer, stride);
}

void renderAttractText(struct WORLD_STATE worldState, u32 *framebuffer, u32 stride)
{
    if (!worldState.gameover)
        return;

    char gameOverChars[9] = { 'G', 'A', 'M', 'E', ' ', 'O', 'V', 'E', 'R' };
    char actionChars[19] = {'P', 'R', 'E', 'S', 'S', ' ', '[', '-', ']', ' ', 'T', 'O', ' ', 'S', 'T', 'A', 'R', 'T', };

    int y = 150;
    int x = 170;

    for (int i = 0; i < 9; i++)
    {
        renderFontChar(gameOverChars[i], x, y, 3, framebuffer, stride);
        x += FONT_WIDTH;
    }

    y = 170;
    x = 130;

    for (int i = 0; i < 18; i++)
    {
        renderFontChar(actionChars[i], x, y, 3, framebuffer, stride);
        x += FONT_WIDTH;
    }
}

void render(struct WORLD_STATE worldState, u32 *framebuffer, u32 stride)
{
    // Clear the screen.
    for (u32 y = 0; y < FB_HEIGHT; y ++)
    {
        for (u32 x = 0; x < FB_WIDTH; x ++)
        {
            u32 position = y * stride / sizeof(u32) + x;
            framebuffer[position] = RGBA8_MAXALPHA(0x00, 0x00, 0x00);
        }
    }

    renderBlocks(worldState, framebuffer, stride);
    renderBall(worldState, framebuffer, stride);
    renderPaddle(worldState, framebuffer, stride);
    renderScore(worldState, framebuffer, stride);
    renderBallCount(worldState, framebuffer, stride);
    renderAttractText(worldState, framebuffer, stride);
}
