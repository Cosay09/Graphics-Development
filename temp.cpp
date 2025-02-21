#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Seamless Scrolling",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    SDL_Surface* image = SDL_LoadBMP("OIUGRK1 [Converted].bmp");
    if (!image) {
        printf("Unable to load image! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    int imgWidth = image->w;
    int imgHeight = image->h;
    int scrollX = 0;  // Tracks the current x position of the image
    int screenWidth = screenSurface->w;

    SDL_Event e;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Update the scrolling position
        scrollX -= 2;  // Move the image left
        if (scrollX <= -imgWidth) {
            scrollX = 0;  // Reset when it scrolls off-screen
        }

        // Clear screen with a black color
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

        // Draw the image
        SDL_Rect srcRect = {0, 0, imgWidth, imgHeight};
        SDL_Rect destRect1 = {scrollX, 0, imgWidth, imgHeight};
        SDL_BlitSurface(image, &srcRect, screenSurface, &destRect1);

        // Draw the repeating part of the image
        if (scrollX + imgWidth < screenWidth) {
            SDL_Rect destRect2 = {scrollX + imgWidth, 0, imgWidth, imgHeight};
            SDL_BlitSurface(image, &srcRect, screenSurface, &destRect2);
        }

        // Update the window surface
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16);  // Delay to cap frame rate at ~60 FPS
    }

    SDL_FreeSurface(image);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
