#include "utils/raylib/raylib.h"
#include "utils/You_Vs_AI.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void initialize_board(Board* board) {
  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      if (i == 0 || i == GRID_SIZE - 1 || j == 0 || j == GRID_SIZE - 1) {
        board->board[i][j].type = BORDER;
        board->board[i][j].health = INFINITY; 
      } else {
        board->board[i][j].type = TILE;
        board->board[i][j].health = 10; 
      }
      board->characters[i][j] = NONE;  
    }
  }
}

void draw_board(Board* board, Character* characters, int character_count) {
  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      Color tileColor;

      if (board->board[i][j].type == BORDER) {
        tileColor = GRAY;
      } else {
        if (board->characters[i][j] == NONE) {
          tileColor = LIGHTGRAY;  
        } else {
          tileColor = characters[board->characters[i][j] - 1].color;
        }
      }

      DrawRectangle(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, tileColor);

      if (board->board[i][j].type == TILE) {
        char healthText[8];
        sprintf(healthText, "%d", board->board[i][j].health);
        DrawText(healthText, j * CELL_SIZE + 10, i * CELL_SIZE + 10, 10, BLACK);
      }
    }
  }
}

void capture_tile(Board* board, Tank* tank, int x, int y) {
  if (board->board[x][y].type == TILE) {
    if (board->characters[x][y] == NONE || board->characters[x][y] != tank->owner) {
      tank->capture_timer += GetFrameTime();
      if (tank->capture_timer >= BASE_TILE_CAPTURE_TIME) {
        // Tank captures the tile
        board->characters[x][y] = tank->owner;
        tank->capture_timer = 0.0f;  
      }
    }
  }
}

void update_tanks(Board* board, Character* characters, int character_count) {
  for (int c = 0; c < character_count; c++) {
    Character* character = &characters[c];

    for (unsigned int i = 0; i < character->tanks_count; i++) {
      Tank* tank = &character->tanks[i];

      if (tank->is_alive) {
        int grid_x = (int)(tank->rect.x / CELL_SIZE);
        int grid_y = (int)(tank->rect.y / CELL_SIZE);

        if (grid_x >= 0 && grid_x < GRID_SIZE && grid_y >= 0 && grid_y < GRID_SIZE) {
          capture_tile(board, tank, grid_y, grid_x);
        }
      }
    }
  }
}

void draw_tanks(Character* characters, int character_count) {
  for (int c = 0; c < character_count; c++) {
    Character* character = &characters[c];

    for (unsigned int i = 0; i < character->tanks_count; i++) {
      Tank* tank = &character->tanks[i];

      if (tank->is_alive) {
        DrawRectanglePro(tank->rect, (Vector2){tank->rect.width / 2, tank->rect.height / 2}, tank->rotation, character->color);
      }
    }
  }
}

void spawn_tank(Character* character, int x, int y) {
  character->tanks = realloc(character->tanks, sizeof(Tank) * (character->tanks_count + 1));

  Tank new_tank = {
    .rect = (Rectangle){ x * CELL_SIZE, y * CELL_SIZE, BASE_TANK_SIZE, BASE_TANK_SIZE },
    .speed = BASE_TANK_SPEED,
    .direction = 0,
    .is_alive = true,
    .rotation = 0.0f,
    .health = 100.0f,
    .fire_timer = 0.0f,
    .fire_speed = 300.0f,
    .fire_cooldown = BASE_TANK_FIRE_COOLDOWN,
    .owner = character->character_type,
    .capture_timer = 0.0f
  };

  character->tanks[character->tanks_count] = new_tank;
  character->tanks_count++;
}

void update_currency(Character* character, int character_count, Board* board) {
  for (int c = 0; c < character_count; c++) {
    character[c].currency = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
      for (int j = 0; j < GRID_SIZE; j++) {
        if (board->characters[i][j] == character[c].character_type) {
          character[c].currency += 10;  
        }
      }
    }
  }
}

bool IsButtonClicked(Rectangle button) {
  return (CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

void DrawButton(Rectangle button, Color color, const char* text) {
  DrawRectangleRec(button, color);
  DrawRectangleLinesEx(button, 2, DARKGRAY);
  int textWidth = MeasureText(text, 20);
  DrawText(text, button.x + button.width / 2 - textWidth / 2, button.y + button.height / 2 - 10, 20, BLACK);
}

void initialize_characters(Character* characters, int numPlayers, bool* isAI, CharacterType* selectedTypes) {
  for (int i = 0; i < numPlayers; i++) {
    characters[i].character_type = selectedTypes[i];  
    characters[i].tanks_count = 0;
    characters[i].tanks = NULL;
    characters[i].currency = 0;
    characters[i].type = isAI[i] ? AI : PLAYER;  

    switch (characters[i].character_type) {
      case PLAYER_ONE:
      case AI_ONE:
        characters[i].color = BLUE;
        break;
      case PLAYER_TWO:
      case AI_TWO:
        characters[i].color = RED;
        break;
      case PLAYER_THREE:
      case AI_THREE:
        characters[i].color = GREEN;
        break;
      case PLAYER_FOUR:
      case AI_FOUR:
        characters[i].color = YELLOW;
        break;
      default:
        break;
    }
  }
}

void RunSimulation() {
  InitWindow(800, 600, "You Vs AI - Maze Game");

  GameScreen screen = TITLE_SCREEN;
  int numPlayers = 0;
  bool isAI[4] = { false, false, false, false }; 
  int playerConfig[4] = { 0, 0, 0, 0 }; 
  CharacterType selectedTypes[4];  
  Character characters[4]; 

  Board gameBoard;
  initialize_board(&gameBoard); 

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    switch (screen) {
      case TITLE_SCREEN:
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("You Vs AI", 300, 200, 40, DARKGRAY);

        Rectangle startButton = { 350, 300, 100, 50 };
        DrawButton(startButton, LIGHTGRAY, "Start");

        if (IsButtonClicked(startButton)) {
          screen = PLAYER_SELECT;
        }

        EndDrawing();
        break;

      case PLAYER_SELECT:
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Player Select Screen", 300, 150, 20, DARKGRAY);

        Rectangle oneVOneButton = { 300, 200, 200, 50 };
        DrawButton(oneVOneButton, LIGHTGRAY, "1v1");

        Rectangle twoVTwoButton = { 300, 300, 200, 50 };
        DrawButton(twoVTwoButton, LIGHTGRAY, "2v2");

        if (IsButtonClicked(oneVOneButton)) {
          numPlayers = 2;
          screen = PLAYER_TYPE;
        }
        if (IsButtonClicked(twoVTwoButton)) {
          numPlayers = 4;
          screen = PLAYER_TYPE;
        }

        EndDrawing();
        break;

      case PLAYER_TYPE:
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Player Type Select", 100, 150, 20, DARKGRAY);

        for (int i = 0; i < numPlayers; i++) {
          Rectangle playerTypeButton = { 100, 200 + (i * 100), 200, 50 };

          if (playerConfig[i] == 0) {
            DrawButton(playerTypeButton, LIGHTGRAY, "Player");
          } else {
            DrawButton(playerTypeButton, LIGHTGRAY, "CPU");
          }

          if (IsButtonClicked(playerTypeButton)) {
            playerConfig[i] = !playerConfig[i];
            isAI[i] = (playerConfig[i] == 1);
          }

          Rectangle blueTypeButton = { 300, 200 + (i * 100), 100, 50 };
          Rectangle redTypeButton = { 400, 200 + (i * 100), 100, 50 };
          Rectangle greenTypeButton = { 500, 200 + (i * 100), 100, 50 };
          Rectangle yellowTypeButton = { 600, 200 + (i * 100), 100, 50 };

          if (IsButtonClicked(blueTypeButton)) {
            selectedTypes[i] = PLAYER_ONE;
            DrawButton(blueTypeButton, BLUE, "Blue");
          } else {
            DrawButton(blueTypeButton, LIGHTGRAY, "Blue");
          }
          if (IsButtonClicked(redTypeButton)) {
            selectedTypes[i] = PLAYER_TWO;
            DrawButton(redTypeButton, RED, "Red");
          } else {
            DrawButton(redTypeButton, LIGHTGRAY, "Red");
          }
          if (IsButtonClicked(greenTypeButton)) {
            selectedTypes[i] = PLAYER_THREE;
            DrawButton(greenTypeButton, GREEN, "Green");
          } else {
            DrawButton(greenTypeButton, LIGHTGRAY, "Green");
          }
          if (IsButtonClicked(yellowTypeButton)) {
            selectedTypes[i] = PLAYER_FOUR;
            DrawButton(yellowTypeButton, YELLOW, "Yellow");
          } else {
            DrawButton(yellowTypeButton, LIGHTGRAY, "Yellow");
          }
        }

        Rectangle startGameButton = { 700, 500, 100, 50 };
        DrawButton(startGameButton, LIGHTGRAY, "Start Game");

        Rectangle backButton = { 0, 500, 100, 50 };
        DrawButton(backButton, LIGHTGRAY, "Back");

        if (IsButtonClicked(startGameButton)) {
          initialize_characters(characters, numPlayers, isAI, selectedTypes);  
          screen = GAME_SCREEN;
        }

        if (IsButtonClicked(backButton)) {
          screen = PLAYER_SELECT;
        }

        EndDrawing();
        break;
      case GAME_SCREEN:
        BeginDrawing();
        ClearBackground(RAYWHITE);

        draw_board(&gameBoard, characters, numPlayers);

        switch (numPlayers) {
          case 2:
            spawn_tank(&characters[0], 1, 1);
            spawn_tank(&characters[1], GRID_SIZE - 2, GRID_SIZE - 2);
            break;
          case 4:
            spawn_tank(&characters[0], 1, 1); 
            spawn_tank(&characters[1], 1, GRID_SIZE - 2); 
            spawn_tank(&characters[2], GRID_SIZE - 2, 1);
            spawn_tank(&characters[3], GRID_SIZE - 2, GRID_SIZE - 2); ;
            break;
        }

        draw_tanks(characters, numPlayers);
        update_tanks(&gameBoard, characters, numPlayers);

        EndDrawing();
        break;
    }
  }

  CloseWindow();
}

int main(void) {
  RunSimulation();
  return 0;
}


