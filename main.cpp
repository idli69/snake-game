#include <deque>
#include <raylib.h>
#include <raymath.h>

using namespace std;

// Deque is used for smooth snake head and tail mechanism.
// This Data Structure is like an array but i can add and remove from front and
// back easily.

// green color for the snake.
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

// The game ground is divided into cells. 25 * 25
int cellSize = 25;
int cellCount = 25;
int offSet = 35;

// to store the last time Update() function ran.
double lastUpdateTime = 0;

// function to add time intervals between events.
bool eventTriggered(double interval)
{

    // get the time elapsed since InitWindow()
    double currentTime = GetTime();

    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
};

// Checks if the collision element has position on the snake.
bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

// Class that handles all the functionalities of the Snake enitiy.
class Snake
{
public:
    // Vectors for the body of the snake
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            // Defines the snake body parts as rectangle and makes it rounded.
            Rectangle segment = {
                (offSet + cellSize * x), (offSet + cellSize * y), (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.6, 6, darkGreen);
        }
    }

    // Updates/Moves the snake body.
    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
            addSegment = false;
        else
            body.pop_back();
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

// Class that handles everything of the Food entity.
class Food
{
public:
    Vector2 position;
    Texture2D texture;

    Food(deque<Vector2> snakeBody)
    {
        // loading the appleimage to a Texture.
        Image image = LoadImage("./assets/small_apple.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);

        // Get random position for the apple.
        position = GenerateRandomPos(snakeBody);
    };

    // unloading from memory, destructor.
    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, offSet + position.x * cellSize, offSet + position.y * cellSize, WHITE);
    };

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    };

    // Function to generate a random (x,y) vector.
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    };
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);

    bool running = true;
    int score = 0;

    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("./assets/sounds/eat.mp3");
        wallSound = LoadSound("./assets/sounds/wall.mp3");
    }
    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }
    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollsionWithScreedEdges();
            CheckCollisionWithTail();
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollsionWithScreedEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
            GameOver();
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
            GameOver();
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> HeadlessBody = snake.body;
        HeadlessBody.pop_front();

        if (ElementInDeque(snake.body[0], HeadlessBody))
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }
};

int main()
{

    InitWindow(
        2 * offSet + cellSize * cellCount, 2 * offSet + cellSize * cellCount, "Retro Snake Game");
    SetTargetFPS(60);

    Game game = Game();

    while (WindowShouldClose() == false)
    {

        BeginDrawing();
        if (eventTriggered(0.3))
        {
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        };
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        };
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        };
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        };

        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offSet - 5,
                                       (float)offSet - 5,
                                       (float)cellSize * cellCount + 10,
                                       (float)cellSize * cellCount + 10},
                             5,
                             darkGreen);

        DrawText("Retro Snake", offSet - 5, 15, 30, darkGreen);
        DrawText(TextFormat("Score : %i", game.score), offSet - 25, offSet, 30, darkGreen);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
