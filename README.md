# Retro Snake

A classic retro Snake game made with **C++** and **[raylib](https://www.raylib.com/)**.

## How to play

- Duh!

## Features

- The window scales to fit your screen size.
- Score, high score and a Game Over screen.
- Sound effects when you eat or crash.

## Run the game

1. Download the zip for your system.
2. Unzip it.
3. Run the game file inside.

Keep the `assets` folder in the same folder as the game file, or the apple and sounds will not load.

## Build from source

You need a C++ compiler and raylib installed.

**Linux**

```bash
g++ main.cpp -o snake -O2 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

**Windows (MinGW)**

```bash
g++ main.cpp -o snake.exe -O2 -lraylib -lopengl32 -lgdi32 -lwinmm
```

Then run it from the project folder so the `assets` folder is found.


## Credits

- Made by: Idli
- Built with [raylib](https://www.raylib.com/) (zlib license)
- Art and sounds: [Pixabay](https://pixabay.com/sound-effects/)

## License

`[choose one, for example MIT, or write "All rights reserved"]`
