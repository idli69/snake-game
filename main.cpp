#include <deque>
#include <raylib.h>
#include <raymath.h>

using namespace std;

// using deque.

// green color for the snake.
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 25;
int cellCount = 25;

double lastUpdateTime = 0;

bool eventTriggered(double interval) {
  double currentTime = GetTime();
  if (currentTime - lastUpdateTime >= interval) {
    lastUpdateTime = currentTime;
    return true;
  }
  return false;
};

class Snake {
public:
  deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};

  Vector2 direction = {1, 0};

  void Draw() {
    for (unsigned int i = 0; i < body.size(); i++) {
      float x = body[i].x;
      float y = body[i].y;

      Rectangle segment = {(cellSize * x), (cellSize * y), (float)cellSize,
                           (float)cellSize};

      DrawRectangleRounded(segment, 0.6, 6, darkGreen);
    }
  }

  void Update() {
    body.pop_back();
    body.push_front(Vector2Add(body[0], direction));
  }
};

class Food {

public:
  Vector2 position;
  Texture2D texture;

  Food() {
    // loading the appleimage to a Texture.
    Image image = LoadImage("./assets/apple.png");
    texture = LoadTextureFromImage(image);
    UnloadImage(image);

    position = GenerateRandomPos();
  };

  // unloading from memory, destructor.
  ~Food() { UnloadTexture(texture); }

  void Draw() {
    DrawTexture(texture, position.x * cellSize, position.y * cellSize, WHITE);
  };

  Vector2 GenerateRandomPos() {

    float x = GetRandomValue(0, cellCount - 1);
    float y = GetRandomValue(0, cellCount - 1);

    return Vector2{x, y};
  };
};

int main() {

  InitWindow(cellSize * cellCount, cellSize * cellCount, "Retro Snake Game");
  SetTargetFPS(60);

  Food food = Food();
  Snake snake = Snake();

  while (WindowShouldClose() == false) {
    BeginDrawing();

    if (eventTriggered(0.3)) {
      snake.Update();
    }

    ClearBackground(green);

    food.Draw();
    snake.Draw();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
