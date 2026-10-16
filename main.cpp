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
Color headColor = {200, 90, 40, 255};

// The game ground is divided into cells. 25 * 25
const int cellCount = 25;
const float marginCells = 1.5f;
const float screenUse = 0.8f;

int cellSize;
int offSet;

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
bool ElementInDeque(Vector2 element, const deque<Vector2>& body)
{
    for (unsigned int i = 0; i < body.size(); i++)
    {
        if (Vector2Equals(body[i], element))
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
    Vector2 nextDirection = {1, 0};
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            Color color = darkGreen;
            if (i == 0) // segment 0 is the head
            {
                color = headColor;
            }

            // Defines the snake body parts as rectangle and makes it rounded.
            Rectangle segment = {
                (offSet + cellSize * x), (offSet + cellSize * y), (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.6, 6, color);
        }
    }

    // Updates/Moves the snake body.
    void Update()
    {
        direction = nextDirection;
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
        nextDirection = {1, 0};
        addSegment = false;
    }
};

// Class that handles everything of the Food entity.
class Food
{
public:
    Vector2 position;
    Texture2D texture;

    Food(const deque<Vector2>& snakeBody)
    {
        // loading the appleimage to a Texture.
        Image image = LoadImage("./assets/apple.png");
        ImageResize(&image, cellSize, cellSize);
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
    Vector2 GenerateRandomPos(const deque<Vector2>& snakeBody)
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
    int highScore = 0;

    Sound eatSound;
    Sound wallSound;
    Music music;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("./assets/sounds/eat.mp3");
        wallSound = LoadSound("./assets/sounds/wall.mp3");

        music = LoadMusicStream("./assets/sounds/snake.mp3");
        music.looping = true;
        SetMusicVolume(music, 0.4f);
        PlayMusicStream(music);
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
    void Start()
    {
        if (running == false)
        {
            running = true;
            score = 0;
        }
    }
    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithScreedEdges();
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
            if (score > highScore)
            {
                highScore = score;
            }

            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithScreedEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1)
            GameOver();
        if (snake.body[0].y == cellCount || snake.body[0].y == -1)
            GameOver();
    }

    void CheckCollisionWithTail()
    {
        // Start at 1 to skip the head (index 0).
        for (unsigned int i = 1; i < snake.body.size(); i++)
        {
            if (Vector2Equals(snake.body[0], snake.body[i]))
            {
                GameOver();
                return;
            }
        }
    }
    void GameOver()
    {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        PlaySound(wallSound);
    }
};

void SetupWindow()
{
    InitWindow(400, 400, "Retro Snake Game");

    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    int maxWindowSize = (int)(screenHeight * screenUse);
    cellSize = (int)(maxWindowSize / (cellCount + 2 * marginCells));
    offSet = (int)(cellSize * marginCells);

    int windowSize = 2 * offSet + cellSize * cellCount;
    SetWindowSize(windowSize, windowSize);
    SetWindowPosition((screenWidth - windowSize) / 2, (screenHeight - windowSize) / 2);
}

int main()
{
    ChangeDirectory(GetApplicationDirectory());

    SetupWindow();
    SetTargetFPS(60);

    {
        Game game = Game();

        while (WindowShouldClose() == false)
        {

            // INPUT
            if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
            {
                game.snake.nextDirection = {0, -1};
                game.Start();
            };
            if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
            {
                game.snake.nextDirection = {0, 1};
                game.Start();
            };
            if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
            {
                game.snake.nextDirection = {-1, 0};
                game.Start();
            };
            if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
            {
                game.snake.nextDirection = {1, 0};
                game.Start();
            };

            // UPDATE
            if (eventTriggered(0.3))
            {
                game.Update();
            }

            // DRAW
            BeginDrawing();

            float border = cellSize / 5.0f;
            float boardSize = (float)cellSize * cellCount;
            int fontSize = cellSize;

            ClearBackground(green);
            DrawRectangleLinesEx(Rectangle{offSet - border,
                                           offSet - border,
                                           boardSize + 2 * border,
                                           boardSize + 2 * border},
                                 border,
                                 darkGreen);

            DrawText("Retro Snake",
                     (int)(offSet - border),
                     (offSet - fontSize) / 2,
                     fontSize,
                     darkGreen);

            const char* scoreText = TextFormat("Score : %i", game.score);
            int scoreWidth = MeasureText(scoreText, fontSize);
            DrawText(scoreText,
                     (int)(offSet + boardSize + border - scoreWidth),
                     (offSet - fontSize) / 2,
                     fontSize,
                     darkGreen);
            DrawText(TextFormat("High Score : %i", game.highScore),
                     (int)(offSet - border),
                     (int)(offSet + boardSize + border * 2),
                     fontSize,
                     darkGreen);

            game.Draw();

            if (game.running == false)
            {
                DrawRectangle(
                    offSet, offSet, (int)boardSize, (int)boardSize, Fade(darkGreen, 0.6f));

                int bigSize = fontSize * 2;
                int w1 = MeasureText("Game Over", bigSize);
                int w2 = MeasureText("Press an arrow key", fontSize);
                int centerX = offSet + (int)(boardSize / 2);
                int centerY = offSet + (int)(boardSize / 2);

                DrawText("Game Over", centerX - w1 / 2, centerY - bigSize, bigSize, green);
                DrawText("Press an arrow key",
                         centerX - w2 / 2,
                         centerY + fontSize / 2,
                         fontSize,
                         green);
            }

            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
