#include "I_InputHandling.h"

playerinput_t playerinput;

// -------------------------------
// Handles SDL Events and input
// -------------------------------
void I_HandleInputGame(void)
{
    const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

    SDL_Event e;
    
    while(SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT:
                application.quit = true;
            break;

            default:
                break;
        }

        U_MixRand(e.common.timestamp);
        G_InGameInputHandlingEvent(&e);
    }

    // Send Input event to subsystems
    if(application.gamestate == GSTATE_GAME)
    {
        G_InGameInputHandling(keyboard_state);
    }
}

void I_HandleInputMenu(void)
{
    SDL_Event e;
    
    while(SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_QUIT:
                application.quit = true;
            break;

            default:
                break;
        }

        G_InMenuInputHandling(&e);
    }
}
