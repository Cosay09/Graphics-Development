#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int WINDOW_WIDTH = 720;
const int WINDOW_HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Surface *surface = NULL;
SDL_Surface *img1 = NULL;

bool init();
bool loadmedia();
void close();

int main(int argc, char *argv[])
{
    if (!init())
    {
        printf("Could not initialize! SDL Error: %s\n", SDL_GetError());
    }
    else
    {
        if (!loadmedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            SDL_BlitSurface(img1, NULL, surface, NULL);

            SDL_UpdateWindowSurface(window);

            SDL_Event event;
            bool running = true;

            while (running)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        running = false;
                    }
                    if (event.type == SDL_KEYDOWN)
                    {
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            running = false;
                        }
                    }
                }
            }
        }
    }
    close();

    return 0;
}

bool init()
{
    bool success = true;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL could not be initialized! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window couldn't be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            surface = SDL_GetWindowSurface(window);
        }
    }

    return success;
}

bool loadmedia()
{
    bool success = true;

    img1 = SDL_LoadBMP("OIUGRK1 [Converted].bmp");
    if (img1 == NULL)
    {
        printf("Couldn't load image %s! SDL Error: %s", "OIUGRK1 [Converted].bmp", SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
    SDL_DestroyWindow(window);
    SDL_FreeSurface(img1);
    SDL_Quit();
}