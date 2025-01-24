// Pong Game Refactored with Separated Functions

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
    Vec2() : x(0.0f), y(0.0f) {}

    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(Vec2 const &rhs)
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
            position.x = WIDTH / 2.0f;
            position.y = HEIGHT / 2.0f;

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

    void update(float dt)
    {
        position += velocity * dt;
        if (position.y < 0)
        {
            position.y = 0;
        }
        else if (position.y > (HEIGHT - Paddle_Height))
        {
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
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        surface = TTF_RenderText_Solid(font, to_string(score).c_str(), {0xFF, 0xFF, 0});
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

Contact checkPaddleCollision(Ball const &ball, Paddle const &paddle);
Contact CheckWallCollisions(Ball const &ball);

void ProcessInput(bool &running, bool buttons[]);
void UpdateGame(Ball &ball, Paddle &paddle1, Paddle &paddle2, PlayerScores &playerOneScore, PlayerScores &playerTwoScore, float dt);
void RenderGame(SDL_Renderer *renderer, Ball &ball, Paddle &paddle1, Paddle &paddle2, PlayerScores &playerOneScore, PlayerScores &playerTwoScore);

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    TTF_Font *font = TTF_OpenFont("Game_NumberFont.ttf", 40);
    
    // Ball, Paddle, and Score Initialization
    Ball ball(Vec2(WIDTH / 2.0f, HEIGHT / 2.0f), Vec2(Ball_Speed, 0.0f));
    Paddle paddle1(Vec2(50.0f, HEIGHT / 2.0f), Vec2(0.0f, 0.0f));
    Paddle paddle2(Vec2(WIDTH - 50.0f, HEIGHT / 2.0f), Vec2(0.0f, 0.0f));
    PlayerScores playerOne(Vec2(WIDTH / 4.0f, 20.0f), renderer, font);
    PlayerScores playerTwo(Vec2(WIDTH * 3 / 4, 20.0f), renderer, font);

    bool running = true;
    bool buttons[4] = {};
    float dt = 0.0f;

    while (running)
    {
        auto startTime = chrono::high_resolution_clock::now();

        ProcessInput(running, buttons);
        UpdateGame(ball, paddle1, paddle2, playerOne, playerTwo, dt);
        RenderGame(renderer, ball, paddle1, paddle2, playerOne, playerTwo);

        auto stopTime = chrono::high_resolution_clock::now();
        dt = chrono::duration<float, chrono::milliseconds::period>(stopTime - startTime).count();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

// Implementations of functions: ProcessInput, UpdateGame, RenderGame, and collision checks here

void ProcessInput(bool &running, bool buttons[])
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            bool isDown = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
                buttons[PaddleOneUP] = isDown;
                break;
            case SDLK_s:
                buttons[PaddleOneDown] = isDown;
                break;
            case SDLK_UP:
                buttons[PaddleTwoUp] = isDown;
                break;
            case SDLK_DOWN:
                buttons[PaddleTwoDown] = isDown;
                break;
            }
        }
    }
}

void UpdateGame(Ball &ball, Paddle &paddle1, Paddle &paddle2, PlayerScores &playerOneScore, PlayerScores &playerTwoScore, float dt)
{
    ball.update(dt);

    if (buttons[PaddleOneUP])
        paddle1.velocity.y = -Paddle_Speed;
    else if (buttons[PaddleOneDown])
        paddle1.velocity.y = Paddle_Speed;
    else
        paddle1.velocity.y = 0.0f;

    if (buttons[PaddleTwoUp])
        paddle2.velocity.y = -Paddle_Speed;
    else if (buttons[PaddleTwoDown])
        paddle2.velocity.y = Paddle_Speed;
    else
        paddle2.velocity.y = 0.0f;

    paddle1.update(dt);
    paddle2.update(dt);

    Contact contact = CheckWallCollisions(ball);
    if (contact.type != CollisionType::None)
    {
        ball.CollideWithWall(contact);
        if (contact.type == CollisionType::Left)
        {
            static int scoreTwo = 0;
            playerTwoScore.SetScore(++scoreTwo);
        }
        else if (contact.type == CollisionType::Right)
        {
            static int scoreOne = 0;
            playerOneScore.SetScore(++scoreOne);
        }
    }

    contact = checkPaddleCollision(ball, paddle1);
    if (contact.type != CollisionType::None)
    {
        ball.CollisionWithPaddle(contact);
    }

    contact = checkPaddleCollision(ball, paddle2);
    if (contact.type != CollisionType::None)
    {
        ball.CollisionWithPaddle(contact);
    }
}

void RenderGame(SDL_Renderer *renderer, Ball &ball, Paddle &paddle1, Paddle &paddle2, PlayerScores &playerOneScore, PlayerScores &playerTwoScore)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    ball.Draw(renderer);
    paddle1.Draw(renderer);
    paddle2.Draw(renderer);
    playerOneScore.Draw();
    playerTwoScore.Draw();

    SDL_RenderPresent(renderer);
}

Contact CheckWallCollisions(Ball const &ball)
{
    Contact contact{CollisionType::None, 0.0f};

    if (ball.position.x < 0.0f)
    {
        contact.type = CollisionType::Left;
    }
    else if (ball.position.x > (WIDTH - Ball_Width))
    {
        contact.type = CollisionType::Right;
    }

    if (ball.position.y < 0.0f)
    {
        contact.type = CollisionType::Top;
        contact.penetration = -ball.position.y;
    }
    else if (ball.position.y > (HEIGHT - Ball_Height))
    {
        contact.type = CollisionType::Bottom;
        contact.penetration = HEIGHT - Ball_Height - ball.position.y;
    }

    return contact;
}

Contact checkPaddleCollision(Ball const &ball, Paddle const &paddle)
{
    Contact contact{CollisionType::None, 0.0f};

    if ((ball.position.x < paddle.position.x + Paddle_Width) &&
        (ball.position.x + Ball_Width > paddle.position.x) &&
        (ball.position.y < paddle.position.y + Paddle_Height) &&
        (ball.position.y + Ball_Height > paddle.position.y))
    {
        float ballMid = ball.position.y + Ball_Height / 2.0f;
        float paddleMid = paddle.position.y + Paddle_Height / 2.0f;

        float region = (paddleMid - ballMid) / (Paddle_Height / 2.0f);

        if (region < -0.5f)
        {
            contact.type = CollisionType::Bottom;
        }
        else if (region > 0.5f)
        {
            contact.type = CollisionType::Top;
        }
        else
        {
            contact.type = CollisionType::Middle;
        }

        if (ball.velocity.x < 0.0f)
        {
            contact.penetration = paddle.position.x + Paddle_Width - ball.position.x;
        }
        else if (ball.velocity.x > 0.0f)
        {
            contact.penetration = paddle.position.x - (ball.position.x + Ball_Width);
        }
    }

    return contact;
}
