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

#include <chrono>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;
const int Ball_Width = 15;
const int Ball_Height = 15;
const int Paddle_Width = 10;
const int Paddle_Height = 80;
const float Paddle_Speed = 1.0f;
const float Ball_Speed = 1.0f;

enum Buttons
{
    PaddleOneUP = 0,
    PaddleOneDown,
    PaddleTwoUp,
    PaddleTwoDown,
};

enum class CollisionType
{
    None,
    Top,
    Middle,
    Bottom,
    Left,
    Right
};

struct Contact
{
    CollisionType type;
    float penetration;
};

class Vec2
{
public:
    Vec2() : x(0.0f), y(0.0f) {} // Use constructor to initialize x and y

    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(Vec2 const &rhs) // rhs = Right Hand Side
    {
        return Vec2(x + rhs.x, y + rhs.y);
    }
    Vec2 &operator+=(Vec2 const &rhs)
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
    Ball(Vec2 position, Vec2 velocity)
        : position(position), velocity(velocity)
    {
        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.y);
        rect.w = Ball_Width;
        rect.h = Ball_Height;
    }

    void Draw(SDL_Renderer *renderer)
    {
        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.y);

        SDL_RenderFillRect(renderer, &rect);
    }

    void update(float dt)
    {
        position += velocity * dt;
    }

    void CollisionWithPaddle(Contact const &contact)
    {
        position.x += contact.penetration;
        velocity.x = -velocity.x;

        if (contact.type == CollisionType::Top)
        {
            velocity.y = -0.75f * Ball_Speed;
        }
        else if (contact.type == CollisionType::Bottom)
        {
            velocity.y = 0.75f * Ball_Speed;
        }
    }

    void CollideWithWall(Contact const &contact)
    {
        if ((contact.type == CollisionType::Top) || (contact.type == CollisionType::Bottom))
        {
            position.y += contact.penetration;
            velocity.y = -velocity.y;
        }
        else if (contact.type == CollisionType::Left || contact.type == CollisionType::Right)
        {
            // Reset ball position to the center
            position.x = WIDTH / 2.0f;
            position.y = HEIGHT / 2.0f;

            // Randomize Y-axis velocity after reset
            velocity.x = (contact.type == CollisionType::Left) ? Ball_Speed : -Ball_Speed;
            velocity.y = ((rand() % 2) == 0 ? 1 : -1) * (0.5f + static_cast<float>(rand()) / RAND_MAX * 0.5f) * Ball_Speed;
        }
    }

    Vec2 position;
    Vec2 velocity;
    SDL_Rect rect{};
};

class Paddle
{
public:
    Paddle(Vec2 position, Vec2 velocity) : position(position), velocity(velocity)
    {
        rect.x = static_cast<int>(position.x);
        rect.y = static_cast<int>(position.y);
        rect.w = Paddle_Width;
        rect.h = Paddle_Height;
    }

    void Draw(SDL_Renderer *renderer)
    {
        rect.y = static_cast<int>(position.y);

        SDL_RenderFillRect(renderer, &rect);
    }

    // Update paddle position
    void update(float dt)
    {
        position += velocity * dt;

        if (position.y < 0)
        {
            // Keeps the paddle at the top of the screen
            position.y = 0;
        }
        else if (position.y > (HEIGHT - Paddle_Height))
        {
            // Keeps the paddle at the bottom of the screen
            position.y = HEIGHT - Paddle_Height;
        }
    }

    Vec2 position;
    Vec2 velocity;
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
        rect.y = static_cast<int>(position.y);
        rect.w = width;
        rect.h = height;
    }

    void SetScore(int score)
    {
        SDL_FreeSurface (surface);
        SDL_DestroyTexture (texture);

        surface = TTF_RenderText_Solid(font, to_string(score).c_str(), {0xFF, 0xFF, 0, 0xFF});
        
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        int height, width;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
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

// Ball and Paddle Collision
Contact chekcPaddleCollision(Ball const &ball, Paddle const &paddle)
{
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + Ball_Width;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + Ball_Height;

    float paddleLeft = paddle.position.x;
    float paddleRight = paddle.position.x + Paddle_Width;
    float paddleTop = paddle.position.y;
    float paddleBottom = paddle.position.y + Paddle_Height;

    Contact contact{};

    if (ballLeft >= paddleRight)
    {
        return contact;
    }
    if (ballRight <= paddleLeft)
    {
        return contact;
    }

    if (ballTop >= paddleBottom)
    {
        return contact;
    }
    if (ballBottom <= paddleTop)
    {
        return contact;
    }

    float paddleRangeUpper = paddleBottom - (2.0f * Paddle_Height / 3.0f);
    float paddleRangerMiddle = paddleBottom - (Paddle_Height / 3.0f);

    if (ball.velocity.x < 0)
    {
        // Left paddle
        contact.penetration = paddleRight - ballLeft;
    }
    else if (ball.velocity.x > 0)
    {
        // Right paddle
        contact.penetration = paddleLeft - ballRight;
    }

    if ((ballBottom > paddleTop) && (ballBottom < paddleRangeUpper))
    {
        contact.type = CollisionType::Top;
    }
    else if ((ballBottom > paddleRangeUpper) && (ballBottom < paddleRangerMiddle))
    {
        contact.type = CollisionType::Middle;
    }
    else
    {
        contact.type = CollisionType::Bottom;
    }

    return contact;
}

Contact CheckWallCollisions(Ball const &ball)
{
    float ballLeft = ball.position.x;
    float ballRight = ball.position.x + Ball_Width;
    float ballTop = ball.position.y;
    float ballBottom = ball.position.y + Ball_Height;

    Contact contact{};

    if (ballLeft < 0.0f)
    {
        contact.type = CollisionType::Left;
    }
    else if (ballRight > WIDTH)
    {
        contact.type = CollisionType::Right;
    }
    else if (ballTop < 0.0f)
    {
        contact.type = CollisionType::Top;
        contact.penetration = -ballTop;
    }
    else if (ballBottom > HEIGHT)
    {
        contact.type = CollisionType::Bottom;
        contact.penetration = HEIGHT - ballBottom;
    }

    return contact;
}

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

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Initialize the Text
    TTF_Font *scoreFont = TTF_OpenFont("D:/Graphics Development/Game_NumberFont.ttf", 40);

    // Create Ball
    Ball ball(
        Vec2(WIDTH / 2.0f - Ball_Width / 2.0f, HEIGHT / 2.0f - Ball_Height / 2.0f),
        Vec2(Ball_Speed, 0.0f));

    // Create two Paddle
    Paddle paddle1(
        Vec2(50.0f, HEIGHT / 2.0f),
        Vec2(0.0f, 0.0f));

    Paddle paddle2(
        Vec2(WIDTH - 50.0f, HEIGHT / 2.0f),
        Vec2(0.0f, 0.0f));

    // Player score text
    PlayerScores playerone(Vec2(WIDTH / 4.0f, 20.0f), renderer, scoreFont);

    PlayerScores playertwo(Vec2(WIDTH * 3 / 4, 20.0f), renderer, scoreFont);

    // GAME LOGIC
    bool running = true;
    bool buttons[4] = {};

    int playerOneScore = 0;
    int playerTwoScore = 0;

    float dt = 0.0f;

    while (running)
    {
        auto startTime = chrono::high_resolution_clock::now();

        // AN EVENT TO KEEP THE LOOP RUNNING
        SDL_Event event;
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
                else if (event.key.keysym.sym == SDLK_w)
                {
                    buttons[Buttons::PaddleOneUP] = true;
                }
                else if (event.key.keysym.sym == SDLK_s)
                {
                    buttons[Buttons::PaddleOneDown] = true;
                }
                else if (event.key.keysym.sym == SDLK_UP)
                {
                    buttons[Buttons::PaddleTwoUp] = true;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    buttons[Buttons::PaddleTwoDown] = true;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_w)
                {
                    buttons[Buttons::PaddleOneUP] = false;
                }
                else if (event.key.keysym.sym == SDLK_s)
                {
                    buttons[Buttons::PaddleOneDown] = false;
                }
                else if (event.key.keysym.sym == SDLK_UP)
                {
                    buttons[Buttons::PaddleTwoUp] = false;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    buttons[Buttons::PaddleTwoDown] = false;
                }
            }
        }

        if (buttons[Buttons::PaddleOneUP])
        {
            paddle1.velocity.y = -Paddle_Speed;
        }
        else if (buttons[Buttons::PaddleOneDown])
        {
            paddle1.velocity.y = Paddle_Speed;
        }
        else
        {
            paddle1.velocity.y = 0.0f;
        }

        if (buttons[Buttons::PaddleTwoUp])
        {
            paddle2.velocity.y = -Paddle_Speed;
        }
        else if (buttons[Buttons::PaddleTwoDown])
        {
            paddle2.velocity.y = Paddle_Speed;
        }
        else
        {
            paddle2.velocity.y = 0.0f;
        }

        // Update paddle position
        paddle1.update(dt);
        paddle2.update(dt);

        // Update Ball position
        ball.update(dt);

        // Check collisions
        if (Contact contact = chekcPaddleCollision(ball, paddle1);
            contact.type != CollisionType::None)
        {
            ball.CollisionWithPaddle(contact);
        }
        else if (contact = chekcPaddleCollision(ball, paddle2);
                 contact.type != CollisionType::None)
        {
            ball.CollisionWithPaddle(contact);
        }
        else if (contact = CheckWallCollisions(ball);
                 contact.type != CollisionType::None)
        {
            ball.CollideWithWall(contact);

            if (contact.type == CollisionType::Left)
            {
                ++playerTwoScore;

                playertwo.SetScore(playerTwoScore);
            }
            else if (contact.type == CollisionType::Right)
            {
                ++playerOneScore;
                playerone.SetScore(playerOneScore);
            }
        }

        // SETS THE SCREEN TO BLACK AND RE_DRAWS EVERYTIME
        SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
        SDL_RenderClear(renderer);

        // Set the color to yellow;
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0, 0xFF);

        // Draw border
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

        // Draw Scores
        playerone.Draw();
        playertwo.Draw();

        // Present the backbuffer
        SDL_RenderPresent(renderer);

        // Calculate frame time
        auto stopTime = chrono::high_resolution_clock::now();
        dt = chrono::duration<float, chrono::milliseconds::period>(stopTime - startTime).count();
    }

    // CLEANUPS ALWAYS!!!!!!!!!!
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(scoreFont);
    TTF_Quit();
    SDL_Quit();

    return 0;
}