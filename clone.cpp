#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

const int W_WIDTH = 1080;
const int W_HEIGHT = 720;

SDL_Window *window = NULL;
SDL_Surface *screensurface = NULL;
SDL_Surface *drawSurface = NULL;

int xPos = 0; // Starting position for the image
int delay = 50;
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

    drawSurface = IMG_Load("background.png");
    if (drawSurface == NULL)
    {
        printf("Couldn't load image! %s\n", SDL_GetError());
        success = false;
    }

    return success;
}

void tileImage(int xOffset)
{
    int imgWidth = drawSurface->w;
    int imgHeight = drawSurface->h;

    // Loop over the window dimensions and blit the image repeatedly
    for (int y = 0; y < W_HEIGHT; y += imgHeight)
    {
        // First blit the main image
        for (int x = xOffset; x < W_WIDTH + imgWidth; x += imgWidth)
        {
            SDL_Rect destRect = {x, y, imgWidth, imgHeight};
            SDL_BlitSurface(drawSurface, NULL, screensurface, &destRect);
        }

        // Blit an additional copy of the image to create a seamless loop
        for (int x = xOffset - imgWidth; x < W_WIDTH; x += imgWidth)
        {
            SDL_Rect destRect = {x, y, imgWidth, imgHeight};
            SDL_BlitSurface(drawSurface, NULL, screensurface, &destRect);
        }
    }

    // Update the window surface to display the changes
    SDL_UpdateWindowSurface(window);
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

                // Clear the screen by filling it with black (or any color you want)
                SDL_FillRect(screensurface, NULL, SDL_MapRGB(screensurface->format, 0, 0, 0));

                // Move the image
                tileImage(xPos);

                // Update the x position of the image to move it to the right
                xPos -= 5; // Adjust speed by changing the increment

                // If the image has moved off the screen, reset its position
                if (xPos >= W_WIDTH)
                {
                    xPos = 0; // Reset to the left side
                }

                if (delay > 0)
                {
                    delay--;
                }
                SDL_Delay(delay);
            }
        }
    }

    close();
    return 0;
}
