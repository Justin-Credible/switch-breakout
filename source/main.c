#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <switch.h>

#include "gameobjects.h"
#include "constants.h"
#include "gamelogic.h"
#include "renderer.h"

// The framebuffer object for rendering to the screen.
Framebuffer fb;

// Flag to control if the game loop should continue executing.
bool doUpdate = true;

// "Inputs" to the game loop.
struct WORLD_STATE worldState;
u64 keysDown;

// Function for the game logic loop; runs on its own thread.
void *gameThread(void *vargp)
{
    // Naive game loop with no concept of time.
    while(doUpdate)
    {
        worldState = updateWorld(worldState, keysDown);
        svcSleepThread(50000000ULL); // 500 ms
    }

    pthread_exit(NULL);
    return NULL;
}

// Handles rendering the scene on the main thread.
void doRender()
{
    // Retrieve the framebuffer
    u32 stride;
    u32* framebuffer = (u32*) framebufferBegin(&fb, &stride);

    render(worldState, framebuffer, stride);

    // We're done rendering, so we end the frame here.
    framebufferEnd(&fb);
}

// Main program entrypoint.
int main(int argc, char* argv[])
{
    // Initialise sockets.
    socketInitializeDefault();

    // Redirect stdout & stderr over network to nxlink.
    nxlinkStdio();

    // Retrieve the default window.
    NWindow* win = nwindowGetDefault();

    // Create a linear double-buffered framebuffer.
    framebufferCreate(&fb, win, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_8888, 2);
    framebufferMakeLinear(&fb);

    // Initialize the game state.
    worldState = initWorld();

    // Start on the game over screen.
    worldState.gameover = true;

    // Fire up the game loop in its own thread so it can run independently.
    pthread_t thread_id;
    pthread_create(&thread_id,NULL, gameThread, NULL); 

    // Main application loop.
    while (appletMainLoop())
    {
        hidScanInput();

        // Save the pressed keys so the game loop can read them.
        keysDown = hidKeysHeld(CONTROLLER_P1_AUTO);

        // See if we need to quit.
        if (keysDown & KEY_PLUS)
            break;

        // Render the scene.
        doRender();
    }

    // Release the framebuffer resources.
    framebufferClose(&fb);

    // Flag to halt the game loop.
    doUpdate = false;

    // Sleep longer than the game loop sleeps so we can be sure its done.
    sleep(1);

    // Wait for the game loop thread to exit.
    pthread_join(thread_id, NULL);

    // Ensure the nxlink debugging socket is closed.
    socketExit();

    return 0;
}
