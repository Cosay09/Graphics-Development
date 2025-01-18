#include<iostream>
#include <SDL2/SDL.h>

using namespace std;

const int WINDOW_WIDTH = 720;
const int WINDOW_HEIGHT = 480;

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Replica", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        cout << "Window was not created." << '\n' << SDL_GetError << '\n';
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
            }
        }

        // SETS THE SCREEN TO BLACK AND RE_DRAWS EVERYTIME
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x0, 0x0, 0xFF);
        SDL_RenderClear(renderer);

        // Set the color to yellow;
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0, 0xFF);

        // Draw border
        for (int i = 0; i < WINDOW_HEIGHT; i++)
        {
            if (i % 5 != 0)
            {
                SDL_RenderDrawPoint(renderer, WINDOW_WIDTH / 2, i);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}