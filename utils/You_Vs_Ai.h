#include "raylib/raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef YOU_VS_AI_H
#define YOU_VS_AI_H

#define GRID_SIZE 10 
#define CELL_SIZE 40 
#define INNER_CELL_SIZE 40 
#define BORDER_THICKNESS 2

#define BASE_TANK_SIZE 20
#define BASE_TANK_SPEED 1.0f
#define BASE_TANK_ROTATION_SPEED 2.0f
#define BASE_TANK_FIRE_COOLDOWN 1.0f
#define BASE_TILE_CAPTURE_TIME 3.0f

typedef struct {
    int x, y;
} Pos; 

typedef enum { BORDER, TILE } TileType; 

typedef struct {
    int health;
    TileType type; 
} Tile;

typedef struct {
    Tile grid[GRID_SIZE][GRID_SIZE];
} Maze;

typedef struct {
    Rectangle rect;
    float speed;
    int direction; 
    bool is_alive;
    float rotation;
    float health;
    float fire_timer;
    float fire_speed;
    float fire_cooldown;
    int owner;
    float capture_timer;
} Tank;

typedef enum {
    PLAYER,
    AI
} EntityType;

typedef enum {
    NONE,
    PLAYER_ONE,
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
    int currency;
    Color color;
    Pos pos;
    Tank *tanks;
    unsigned int tanks_count;
} Character;

typedef enum {
    TITLE_SCREEN,
    PLAYER_SELECT,
    PLAYER_TYPE,
    GAME_SCREEN,
    GAME_OVER,
    WIN_SCREEN
} GameScreen; 
 
typedef struct {
    Tile board[GRID_SIZE][GRID_SIZE];
    CharacterType characters[GRID_SIZE][GRID_SIZE];  
} Board;

void initialize_characters(Character* characters, int numPlayers, bool* isAI, CharacterType* selectedTypes); 
void RunSimulation(void);
void initialize_board(Board* board);
void draw_board(Board* board, Character* characters, int character_count);
bool IsButtonClicked(Rectangle button);
void DrawButton(Rectangle button, Color color, const char* text);
void update_tanks(Board* board, Character* characters, int character_count);
void spawn_tank(Character* character, int x, int y);
void capture_tile(Board* board, Tank* tank, int x, int y);
void draw_tanks(Character* characters, int character_count);
void update_currency(Character* character, int character_count, Board* board);

#endif
