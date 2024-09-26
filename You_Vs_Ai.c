#include "utils/raylib/raylib.h"
#include "utils/You_Vs_AI.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void initialize_board(Board* board) {
  for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
      board->board[i][j].health = 100; 

      if (i == 0 || j == 0 || i == GRID_SIZE - 1 || j == GRID_SIZE - 1) {
        board->board[i][j].type = BORDER;
      } else {
        board->board[i][j].type = TILE; 
      }

      board->characters[i][j] = NONE;
    }
  }
}

void generate_maze(Board* board) {
  srand(time(NULL));

  int startX = rand() % (GRID_SIZE - 2) + 1;
  int startY = rand() % (GRID_SIZE - 2) + 1;

  board->board[startX][startY].type = TILE;

  int currentX = startX;
  int currentY = startY;

  while (true) {
    int directions[4] = {0, 0, 0, 0};
    int directionsCount = 0;

    if (currentX > 1 && board->board[currentX - 2][currentY].type == TILE) directions[0] = 1;
    if (currentX < GRID_SIZE - 2 && board->board[currentX + 2][currentY].type == TILE) directions[1] = 1;
    if (currentY > 1 && board->board[currentX][currentY - 2].type == TILE) directions[2] = 1;
    if (currentY < GRID_SIZE - 2 && board->board[currentX][currentY + 2].type == TILE) directions[3] = 1;

    if (directionsCount == 0) break;

    int directionIndex = rand() % directionsCount;
    int direction = directions[directionIndex];

    switch (direction) {
      case 0:
    currentX -= 2;
    board->board[currentX + 1][currentY].type = TILE;
    board->board[currentX][currentY + 1].type = TILE;
    break;
    case 1:
    currentX += 2;
    board->board[currentX][currentY + 1].type = TILE;
    board->board[currentX + 1][currentY].type = TILE;
    break;
    case 2:
    currentY -= 2;
    board->board[currentX + 1][currentY].type = TILE;
    board->board[currentX][currentY + 1].type = TILE;
    break;
    case 3:
    currentY += 2;
    board->board[currentX + 1][currentY].type = TILE;
    board->board[currentX][currentY + 1].type = TILE;
    break;
    default:
   break;
    }
  if (currentX == startX && currentY == startY) break;
  }
} 

void initialize_characters(Character* characters, int numPlayers, bool* isAI, CharacterType* selectedTypes) {
  for (int i = 0; i < numPlayers; i++) {
    characters[i].type = isAI[i] ? AI : PLAYER; 
    characters[i].character_type = selectedTypes[i]; 
    characters[i].currency = 100; 
    characters[i].tanks_count = 0; 

    switch (selectedTypes[i]) {
      case PLAYER_ONE: characters[i].color = RED; break;
      case PLAYER_TWO: characters[i].color = BLUE; break;
      case PLAYER_THREE: characters[i].color = GREEN; break;
      case PLAYER_FOUR: characters[i].color = YELLOW; break;
      case AI_ONE: characters[i].color = GRAY; break;
      case AI_TWO: characters[i].color = DARKGRAY; break;
      case AI_THREE: characters[i].color = PURPLE; break;
      case AI_FOUR: characters[i].color = ORANGE; break;
      default: characters[i].color = WHITE; break;
    }

    switch (i) {
      case 0: 
        characters[i].pos.x = GRID_SIZE / 2; 
        characters[i].pos.y = 0;
        break;
      case 1:
        characters[i].pos.x = GRID_SIZE / 2; 
        characters[i].pos.y = GRID_SIZE - 1; 
        break;
      case 2: 
        characters[i].pos.x = 0; 
        characters[i].pos.y = GRID_SIZE / 2; 
        break;
      case 3: 
        characters[i].pos.x = GRID_SIZE - 1;
        characters[i].pos.y = GRID_SIZE / 2; 
        break;
    }

    characters[i].tanks = NULL; 
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

void draw_board(Board* board, Character* characters, int character_count, int offsetX, int offsetY) {
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

      // Apply offset to position the board in the center
      DrawRectangle(j * CELL_SIZE + offsetX, i * CELL_SIZE + offsetY, CELL_SIZE, CELL_SIZE, tileColor);

      if (board->board[i][j].type == TILE) {
        char healthText[8];
        sprintf(healthText, "%d", board->board[i][j].health);
        DrawText(healthText, j * CELL_SIZE + 10 + offsetX, i * CELL_SIZE + 10 + offsetY, 10, BLACK);
      }
    }
  }
}

void capture_tile(Board* board, Tank* tank, int x, int y) {
  if (board->board[x][y].type == TILE) {
    if (board->characters[x][y] == NONE || board->characters[x][y] != tank->owner) {
      tank->capture_timer += GetFrameTime();
      if (tank->capture_timer >= BASE_TILE_CAPTURE_TIME) {
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

void draw_tanks(Character* characters, int character_count, int offsetX, int offsetY) {
  for (int c = 0; c < character_count; c++) {
    Character* character = &characters[c];

    for (unsigned int i = 0; i < character->tanks_count; i++) {
      Tank* tank = &character->tanks[i];

      if (tank->is_alive) {
        Rectangle tankRect = {
          .x = tank->rect.x + offsetX,
          .y = tank->rect.y + offsetY,
          .width = tank->rect.width,
          .height = tank->rect.height
        };
        DrawRectanglePro(tankRect, (Vector2){tank->rect.width / 2, tank->rect.height / 2}, tank->rotation, character->color);
      }
    }
  }
}

void draw_currency(Character* characters, int character_count) {
    switch (character_count) {
        case 2:
            DrawText(TextFormat("Player 1: $%d", characters[0].currency), 50, 10, 20, characters[0].color);
            DrawText(TextFormat("Player 2: $%d", characters[1].currency), SCREEN_WIDTH - 150, 10, 20, characters[1].color);
            break;

        case 4:
            for (int i = 0; i < 4; i++) {
                int textX = 20 + (i * 180);  
                int textY = 10;
                DrawText(TextFormat("Player %d: $%d", i + 1, characters[i].currency), textX, textY, 20, characters[i].color);
            }
            break;

        default:
            for (int i = 0; i < character_count; i++) {
                int textX = 20 + (i * 180);
                int textY = 10;
                DrawText(TextFormat("Player %d: $%d", i + 1, characters[i].currency), textX, textY, 20, characters[i].color);
            }
            break;
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

void RunSimulation() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "You Vs AI - Maze Game");
  int offsetX = (SCREEN_WIDTH - GRID_SIZE * CELL_SIZE) / 2;
  int offsetY = (SCREEN_HEIGHT - GRID_SIZE * CELL_SIZE) / 2;

  GameScreen screen = TITLE_SCREEN;
  int numPlayers = 0;
  bool isAI[4] = { false, false, false, false }; 
  int playerConfig[4] = { 0, 0, 0, 0 }; 
  CharacterType selectedTypes[4];  
  Character characters[4]; 

  Board gameBoard = { 0 };

  initialize_board(&gameBoard);
  generate_maze(&gameBoard);

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
        DrawText("Player Type Select", 100, 50, 20, DARKGRAY);

        for (int i = 0; i < numPlayers; i++) {
          Rectangle playerTypeButton = { 100, 100 + (i * 100), 200, 50 };

          if (playerConfig[i] == 0) {
            DrawButton(playerTypeButton, LIGHTGRAY, "Player");
          } else {
            DrawButton(playerTypeButton, LIGHTGRAY, "CPU");
          }

          if (IsButtonClicked(playerTypeButton)) {
            playerConfig[i] = !playerConfig[i];
            isAI[i] = (playerConfig[i] == 1);
          }

          Rectangle blueTypeButton = { 300, 100 + (i * 100), 100, 50 };
          Rectangle redTypeButton = { 400, 100 + (i * 100), 100, 50 };
          Rectangle greenTypeButton = { 500, 100 + (i * 100), 100, 50 };
          Rectangle yellowTypeButton = { 600, 100 + (i * 100), 100, 50 };

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

        Rectangle startGameButton = { 700, 500, 100, 50};
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

        draw_board(&gameBoard, characters, numPlayers, offsetX, offsetY); 
        draw_tanks(characters, numPlayers, offsetX, offsetY);
        draw_currency(characters, numPlayers);

        update_tanks(&gameBoard, characters, numPlayers);

        switch (numPlayers) {
          case 2:
            spawn_tank(&characters[0], GRID_SIZE / 2, 0);  
            spawn_tank(&characters[1], GRID_SIZE / 2, GRID_SIZE - 1);  
            break;
          case 4: 
            spawn_tank(&characters[0], GRID_SIZE / 2, 0);  
            spawn_tank(&characters[1], GRID_SIZE / 2, GRID_SIZE - 1);  
            spawn_tank(&characters[2], 0, GRID_SIZE / 2);  
            spawn_tank(&characters[3], GRID_SIZE - 1, GRID_SIZE / 2);  
          break;
        }

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

