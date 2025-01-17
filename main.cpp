/*
MIT License

Copyright (c) 2020 Austin Morlan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;
const int Ball_Width = 10;
const int Ball_Height = 10;
const int Paddle_Width = 7;
const int Paddle_Height = 70;

class Vec2
{
public:
    Vec2(): x(0.0f), y(0.0f) {} //Use constructor to initialize x and y
    
    Vec2(float x, float y): x(x), y(y) {}
    
    Vec2 operator+(Vec2 const& rhs) //rhs = Right Hand Side
    {
        return Vec2(x + rhs.x, y + rhs.y);
    }
    Vec2& operator+=(Vec2 const& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator*(float rhs)
    {
        return Vec2(x * rhs, y * rhs);
    }
    float x, y;
};

class Ball
{
public:
	Ball(Vec2 position)
		: position(position)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = Ball_Width;
		rect.h = Ball_Height;
	}

	void Draw(SDL_Renderer* renderer)
	{
		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);

		SDL_RenderFillRect(renderer, &rect);
	}

	Vec2 position;
	SDL_Rect rect{};
};

class Paddle
{
public:
    Paddle(Vec2 position) : position(position)
    {
        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.y);
        rect.w = Paddle_Width;
        rect.h = Paddle_Height;
    }

    void Draw (SDL_Renderer *renderer)
    {
        rect.y = static_cast<int>(position.y);

        SDL_RenderFillRect(renderer, &rect);
    }

    Vec2 position;
    SDL_Rect rect;
};

class PlayerScores
{
public:
    PlayerScores(Vec2 position, SDL_Renderer *renderer, TTF_Font *font)
        : renderer(renderer), font(font)
    {
        surface = TTF_RenderText_Solid(font, "0", {0xFF, 0xFF, 0});
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.x);
        rect.w = width;
        rect.h = height;
    }

    ~PlayerScores()
    {
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void Draw()
    {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }

    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Rect rect{};
    SDL_Texture *texture{};
    SDL_Surface *surface{};
};

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow(
        "Pong",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL)
    {
        cout << "SDL Window Creation Failed." << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Initialize the Text
    TTF_Font *scoreFont = TTF_OpenFont("Game_Number.ttf", 40);

    // Player score text
    PlayerScores playerone(Vec2(WIDTH / 4, 20), renderer, scoreFont);

    PlayerScores playertwo (Vec2(WIDTH - WIDTH / 4, 20), renderer, scoreFont);

    // Create Ball
    Ball ball(
        Vec2(WIDTH / 2.0f - Ball_Width / 2.0f, HEIGHT / 2.0f - Ball_Height / 2.0f));

    // Create two Paddle
    Paddle paddle1(
        Vec2(WIDTH / 60.0f - Paddle_Width / 2.0f, HEIGHT / 2.0f - Paddle_Height / 2.0f));

    Paddle paddle2(
        Vec2(WIDTH - (WIDTH / 60.0f), HEIGHT / 2.0f - Paddle_Height / 2.0f));
        
    // GAME LOGIC
    bool running = true;
    while (running)
    {
        // AN EVENT TO KEEP THE LOOP RUNNING
        SDL_Event event;

        // LINE 32-47 CHECKS IF WINDOW IS CLOSED OR PRESSED THE ESCAPE KEY
        if (SDL_PollEvent(&event))
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
        //SETS THE SCREEN TO BLACK AND RE_DRAWS EVERYTIME
        SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
        SDL_RenderClear(renderer);

        //Set the color to yellow;
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0, 0xFF);

        //Draw border
        for (int i = 0; i < HEIGHT; i++)
        {
            if (i % 5 != 0)
            {
                SDL_RenderDrawPoint(renderer, WIDTH / 2, i);
            }
        }

        // Draw Ball
        ball.Draw(renderer);

        // Draw Paddles
        paddle1.Draw(renderer);
        paddle2.Draw(renderer);

        //Draw Scores
        playerone.Draw();
        playertwo.Draw();

        //Present the backbuffer
        SDL_RenderPresent(renderer);
    }

    // CLEANUPS ALWAYS!!!!!!!!!!
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(scoreFont);
    TTF_Quit();
    SDL_Quit();

    return 0;
}