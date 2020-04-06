
#include <switch.h>

#include "gamelogic.h"
#include "constants.h"
#include "gamelogic.h"
#include "utilities.h"

#define BALL_START_COUNT 3;
#define BALL_START_X 10;
#define BALL_START_Y 50;
#define BALL_START_TRAVEL_X 1;
#define BALL_START_TRAVEL_Y 1;

// Initializes the world state to starting values ready for a new game.
struct WORLD_STATE initWorld()
{
    struct WORLD_STATE worldState;

    // Setup score, lives, etc.
    worldState.gameover = false;
    worldState.balls = BALL_START_COUNT;
    worldState.score = 0;

    // Setup the paddle.
    worldState.paddle.y = WORLD_HEIGHT - PADDLE_HEIGHT - 2;
    worldState.paddle.x = WORLD_WIDTH / 2 - (PADDLE_WIDTH / 2);

    // Setup the ball.
    worldState.ball.x = BALL_START_X;
    worldState.ball.y = BALL_START_Y;
    worldState.ball.travelX = BALL_START_TRAVEL_X;
    worldState.ball.travelY = BALL_START_TRAVEL_Y;

    // Setup the initial bricks.

    int i = 0;
    int x = 0;
    int y = 0;

    for (int rowIndex = 0; rowIndex < BLOCKS_ROWS; rowIndex++)
    {
        if (rowIndex == 0)
            y = 1;
        else
            y = y + BLOCK_HEIGHT + 1;

        for (int columnIndex = 0; columnIndex < BLOCKS_COLUMNS; columnIndex++)
        {
            struct BLOCK block;
            block.alive = true;

            if (columnIndex == 0)
                block.x = 1;
            else
                block.x = x + BLOCK_WIDTH + 1;

            block.y = y;

            x = block.x;

            worldState.blocks[i] = block;
            i++;
        }

        x = 0;
    }

    return worldState;
}

// Determines the number of points to be awarded when breaking a block
// at the given row (indexed zero from the top).
int getScoreForRow(int row)
{
    switch (row)
    {
        case 0:
        case 1:
            return 100;
        case 2:
        case 3:
            return 50;
        case 4:
        case 5:
            return 25;
        case 6:
        case 7:
            return 10;
        case 8:
        case 9:
        default:
            return 5;
    }
}

struct WORLD_STATE updateWorld(struct WORLD_STATE worldState, u64 keysDown)
{
    // If the game is over there is no work to do.
    if (worldState.gameover)
    {
        // See if the user presses the [-] button to restart the game.
        if (keysDown & KEY_MINUS)
             return initWorld();

        return worldState;
    }

    // Update paddle.

    if (keysDown & KEY_LEFT && worldState.paddle.x - 5 >= 0)
        worldState.paddle.x -= 5;

    if (keysDown & KEY_RIGHT && worldState.paddle.x + 5 + PADDLE_WIDTH <= WORLD_WIDTH)
        worldState.paddle.x += 5;

    // Get next location for ball.

    int ballNextX = worldState.ball.x + worldState.ball.travelX;
    int ballNextY = worldState.ball.y + worldState.ball.travelY;

    struct RECT ballNextRect;
    ballNextRect.x1 = ballNextX;
    ballNextRect.y1 = ballNextY;
    ballNextRect.x2 = ballNextX + BALL_WIDTH;
    ballNextRect.y2 = ballNextY + BALL_HEIGHT;

    // Check for collision with world boundaries.

    if (ballNextX <= 0 || ballNextX + BALL_WIDTH >= WORLD_WIDTH - 1)
        worldState.ball.travelX = worldState.ball.travelX * -1;

    if (ballNextY <= 0)
        worldState.ball.travelY = worldState.ball.travelY * -1;

    if (ballNextY + BALL_HEIGHT >= WORLD_HEIGHT - 1)
    {
        worldState.balls--;

        if (worldState.balls == 0)
            worldState.gameover = true;
        else
        {
            worldState.ball.x = BALL_START_X;
            worldState.ball.y = BALL_START_Y;
            worldState.ball.travelX = BALL_START_TRAVEL_X;
            worldState.ball.travelY = BALL_START_TRAVEL_Y;
        }

        return worldState;
    }

    // Check for collision with paddle.

    struct RECT paddleRect;
    paddleRect.x1 = worldState.paddle.x;
    paddleRect.y1 = worldState.paddle.y;
    paddleRect.x2 = worldState.paddle.x + PADDLE_WIDTH;
    paddleRect.y2 = worldState.paddle.y + PADDLE_HEIGHT;

    if (checkOverlap(paddleRect, ballNextRect))
        worldState.ball.travelY = worldState.ball.travelY * -1;

    // Check for collision with bricks.
    for (int i = 0; i < BLOCKS_COLUMNS * BLOCKS_ROWS; i++)
    {
        struct BLOCK block = worldState.blocks[i];

        if (!block.alive)
            continue;

        struct RECT blockRect;
        blockRect.x1 = block.x;
        blockRect.y1 = block.y;
        blockRect.x2 = block.x + BLOCK_WIDTH;
        blockRect.y2 = block.y + BLOCK_HEIGHT;

        bool overlap = checkOverlap(blockRect, ballNextRect);

        if (overlap)
        {
            worldState.ball.travelY = worldState.ball.travelY * -1;
            worldState.score += getScoreForRow(i / BLOCKS_COLUMNS);
            block.alive = false;
        }

        worldState.blocks[i] = block;
    }

    // Update the ball location based on the final value of the travel field.
    worldState.ball.x = worldState.ball.x + worldState.ball.travelX;
    worldState.ball.y = worldState.ball.y + worldState.ball.travelY;

    return worldState;
}
