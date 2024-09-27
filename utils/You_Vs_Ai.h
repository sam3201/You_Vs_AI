#include "raylib/raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef YOU_VS_AI_H
#define YOU_VS_AI_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600 

#define GRID_SIZE 10 
#define CELL_SIZE 40 
#define INNER_CELL_SIZE 40 
#define BORDER_THICKNESS 2

#define MAZE_SIZE (SCREEN_WIDTH / CELL_SIZE)

#define START_AREA_SIZE 1

#define CURRENCY_RATE 150 

#define COLOR_COUNT 8

#define TANK_PATH "assets/tanks/"

#define BASE_TANK_PATH "BASE_TANK" 
#define BASE_TANK_SIZE 20
#define BASE_TANK_SPEED 1.0f
#define BASE_TANK_ROTATION_SPEED 2.0f
#define BASE_TANK_FIRE_COOLDOWN 1.0f
#define BASE_TILE_CAPTURE_TIME 3.0f
#define BASE_TANK_HEALTH 10.0f
#define BASE_TANK_FIRE_SPEED 10.0f

typedef struct {
    Rectangle buttonRect;
    Color color;
} ColorButton;

int CheckColorSelection(ColorButton* buttons, int count) {
    for (int i = 0; i < count; i++) {
        if (CheckCollisionPointRec(GetMousePosition(), buttons[i].buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return i;
        }
    }
    return -1;
}

Texture2D BASE_TANKS_COLOR[8] = {
};

const char* colorNames[8] = { "blue.png", "red.png", "green.png", "yellow.png", "purple.png", "orange.png", "lightgray.png", "white.png" };
Color availableColors[COLOR_COUNT] = { BLUE, RED, GREEN, YELLOW, PURPLE, ORANGE, LIGHTGRAY, WHITE};

void LoadBaseTankTextures() {
    char filePath[100];
    for (int i = 0; i < 8; i++) {
        snprintf(filePath, sizeof(filePath), "%s%s", BASE_TANK_PATH, colorNames[i]);
        Image img = LoadImage(filePath);
        ImageResize(&img, CELL_SIZE, CELL_SIZE);
        BASE_TANKS_COLOR[i] = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}

void UnloadBaseTankTextures() {
    for (int i = 0; i < 8; i++) {
        UnloadTexture(BASE_TANKS_COLOR[i]);
    }
}

typedef struct {
    int x, y;
} Pos; 

typedef enum { BORDER, TILE, PLAYER_TILE } TileType; 

typedef struct {
    int health;
    TileType type; 
    Color color;
} Tile;

typedef struct {
    Tile grid[GRID_SIZE][GRID_SIZE];
} Maze;

typedef struct {
    int owner;
    Texture2D texture; 
    Pos pos;
    float speed;
    int direction; 
    bool is_alive;
    float rotation;
    float health;
    float fire_timer;
    float fire_speed;
    float fire_cooldown;
    float capture_timer;
} Tank;

typedef enum {
    PLAYER,
    AI
} EntityType;

typedef enum {
    NONE = -1,
    PLAYER_ONE = 0,
    PLAYER_TWO,
    PLAYER_THREE,
    PLAYER_FOUR,
    AI_ONE,
    AI_TWO,
    AI_THREE,
    AI_FOUR,
} CharacterType;

typedef struct {
    EntityType type; 
    CharacterType character_type;
    int frame_counter;
    int currency;
    Color color;
    Pos pos;
    Tank *tanks;
    unsigned int tanks_count;
} Character;

typedef enum {
    TITLE_SCREEN,
    COLOR_SELECT,
    PLAYER_SELECT,
    PLAYER_TYPE,
    GAME_SCREEN,
    GAME_PLAY,
    GAME_OVER,
    WIN_SCREEN
} GameScreen; 
 
typedef struct {
    Tile board[GRID_SIZE][GRID_SIZE];
    CharacterType characters[GRID_SIZE][GRID_SIZE];  
} Board;

void initialize_characters(Character* characters, int numPlayers, bool* isAI, CharacterType* selectedTypes, Color* chosenColors);
void Run(void);
void initialize_board(Board* board, Character* characters, int numPlayers);
void draw_board(Board* board, Character* characters, int character_count, int offsetX, int offsetY); 
bool IsButtonClicked(Rectangle button);
void DrawButton(Rectangle button, Color color, const char* text);
void update_tanks(Board* board, Character* characters, int character_count);
void spawn_tank(Character* character, int x, int y);
void capture_tile(Board* board, Tank* tank, int x, int y);
void draw_tanks(Character* characters, int character_count, int offsetX, int offsetY); 
void update_currency(Character* characters, int character_count);

#endif

