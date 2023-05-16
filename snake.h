// #define SDL_MAIN_HANDLED uses so that SDL2 doesn't redefine main as SDL_main
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define SNAKE_SIZE 20
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *scoreTextTex = NULL, *scoreNumTex = NULL;
Mix_Music *gameMusic = NULL;
Mix_Chunk *dirChange = NULL;
Mix_Chunk *foodEaten = NULL;
Mix_Chunk *gameOver = NULL;
enum
{
    GAME,
    GAME_OVER
};
enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};
int gameState = GAME;
const int FPS = 10;
int currSnakeDir = RIGHT;
int myScore = 0;
typedef struct Snake
{
    SDL_Rect node;
    struct Snake *next;
    struct Snake *prev;
} Snake;
Snake *snakeHead = NULL, *snakeTail = NULL;
typedef struct Food
{
    int x;
    int y;
    SDL_Rect food_rec;
} Food;
Food newFood;
