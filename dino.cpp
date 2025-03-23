#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

const int W_WIDTH = 1080;
const int W_HEIGHT = 480;

SDL_Window *window = NULL;
SDL_Surface *screensurface = NULL;
SDL_Surface *drawSurface = NULL;
SDL_Rect rect;
//SDL_Renderer *renderer = NULL;

bool init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Run DINO Run!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        screensurface = SDL_GetWindowSurface(window);
    }

    /*renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
    {
        printf("Couldn't initialize renderer! %s\n", SDL_GetError());
        return false;
    }*/

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("IMG_Init failed: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

bool loadmedia()
{
    bool success = true;

    drawSurface = IMG_Load("background-1.png");
    if (drawSurface == NULL)
    {
        printf("Couldn't load image! %s\n", SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
    SDL_DestroyWindow(window);
    SDL_FreeSurface(drawSurface);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        if (!loadmedia())
        {
            printf("Couldn't load media! %s", SDL_GetError());
        }
        else
        {
            rect.x = 0;
            rect.y = 0;
            rect.w = drawSurface->w;
            rect.h = drawSurface->h;

            bool running = true;
            SDL_Event event;

            while (running)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT ||
                        (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                    {
                        running = false;
                    }
                }
                SDL_FillRect(screensurface, NULL, SDL_MapRGB(screensurface->format, 0, 0, 0));

                SDL_BlitSurface(drawSurface, NULL, screensurface, &rect);

                SDL_UpdateWindowSurface(window);
            }
        }
    }

    close();
    return 0;
}
