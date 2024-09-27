#include "utils/raylib/raylib.h"
#include "utils/raylib/raymath.h"
#include "utils/You_Vs_AI.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void generate_maze(Board* board) {
    srand(time(NULL));

    for (int i = 0; i < MAZE_SIZE; i++) {
      int tileX = rand() % (GRID_SIZE - 2) + 1;
      int tileY = rand() % (GRID_SIZE - 2) + 1;
      board->board[tileX][tileY].type = TILE;
      board->board[tileX][tileY].health = 100;
  }
}

void set_player_colors(Character* characters, int numPlayers, Color* chosenColors) {
    for (int i = 0; i < numPlayers; i++) {
        characters[i].color = chosenColors[i];
    }
}

void setup_player_start_area(Board* board, Character* characters, int player, int start_x, int start_y) {
    CharacterType playerType = characters[player].character_type;
    Color playerColor = characters[player].color;  // Use dynamic color assigned to the character

    for (int i = 0; i < START_AREA_SIZE; i++) {
        for (int j = 0; j < START_AREA_SIZE; j++) {
            int x = start_x + i;
            int y = start_y + j;
            if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                board->board[y][x].type = PLAYER_TILE;
                board->board[y][x].health = 100;
                board->characters[y][x] = playerType;

                // Assign the player's color to the board tile
                board->board[y][x].color = playerColor;
            }
        }
    }
}

void initialize_board(Board* board, Character* characters, int numPlayers) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            board->board[i][j].health = 100;
            board->board[i][j].type = BORDER;
            board->characters[i][j] = NONE;
        }
    }

    switch (numPlayers) {
        case 2:
            setup_player_start_area(board, characters, 0, GRID_SIZE / 2 - START_AREA_SIZE / 2, 0);
            setup_player_start_area(board, characters, 1, GRID_SIZE / 2 - START_AREA_SIZE / 2, GRID_SIZE - START_AREA_SIZE);
            break;
        case 4:
            setup_player_start_area(board, characters, 0, GRID_SIZE / 2 - START_AREA_SIZE / 2, 0);
            setup_player_start_area(board, characters, 1, GRID_SIZE / 2 - START_AREA_SIZE / 2, GRID_SIZE - START_AREA_SIZE);
            setup_player_start_area(board, characters, 2, 0, GRID_SIZE / 2 - START_AREA_SIZE / 2);
            setup_player_start_area(board, characters, 3, GRID_SIZE - START_AREA_SIZE, GRID_SIZE / 2 - START_AREA_SIZE / 2);
            break;
    }

    generate_maze(board);
}

void initialize_characters(Character* characters, int numPlayers, bool* isAI, CharacterType* selectedTypes, Color* chosenColors) {
    set_player_colors(characters, numPlayers, chosenColors);

    for (int i = 0; i < numPlayers; i++) {
        characters[i].type = isAI[i] ? AI : PLAYER;
        characters[i].character_type = selectedTypes[i];
        characters[i].currency = 0;  
        characters[i].tanks_count = 0;
        characters[i].frame_counter = 0;
        characters[i].tanks_count = 0;

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

void DrawColorButtons(ColorButton* buttons, int count) {
    for (int i = 0; i < count; i++) {
        DrawRectangleRec(buttons[i].buttonRect, buttons[i].color);
        DrawRectangleLinesEx(buttons[i].buttonRect, 2, DARKGRAY);
    }
}

void draw_board(Board* board, Character* characters, int character_count, int offsetX, int offsetY) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            Color tileColor;

            if (board->board[i][j].type == BORDER) {
                tileColor = GRAY;
            } else {
                CharacterType owner = board->characters[i][j];
                switch (owner) {
                    case PLAYER_ONE: tileColor = RED; break;
                    case PLAYER_TWO: tileColor = BLUE; break;
                    case PLAYER_THREE: tileColor = GREEN; break;
                    case PLAYER_FOUR: tileColor = YELLOW; break;
                    default: tileColor = LIGHTGRAY; break;
                }
            }

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
                int grid_x = (int)(tank->pos.x / CELL_SIZE);
                int grid_y = (int)(tank->pos.y / CELL_SIZE);

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
                Vector2 position = {
                    tank->pos.x + offsetX,
                    tank->pos.y + offsetY
                };

                DrawTextureEx(tank->texture, position, tank->rotation, 1.0f, WHITE);
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

void spawn_player_tile(Board* board, Character* character, int start_x, int start_y) {
    int x = start_x + START_AREA_SIZE / 2;
    int y = start_y + START_AREA_SIZE / 2;
    
    if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
        board->board[y][x].type = TILE;
        board->board[y][x].health = 100;
        board->characters[y][x] = character->character_type;
        character->pos.x = x;
        character->pos.y = y;
    }
}

void spawn_tank(Character* character, int x, int y) {
    character->tanks = realloc(character->tanks, sizeof(Tank) * (character->tanks_count + 1));

    Tank new_tank = {
        .texture = BASE_TANKS_COLOR[character->character_type],
        .pos = {x * CELL_SIZE, y * CELL_SIZE},
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

void update_currency(Character* characters, int character_count) {
    for (int c = 0; c < character_count; c++) {
        characters[c].frame_counter++;

        if (characters[c].frame_counter >= CURRENCY_RATE) {
            characters[c].currency++;  
            characters[c].frame_counter = 0;
        }

        // Optional: Additional logic for gaining currency based on owned tiles can remain here
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
    Color chosenColors[4] = { RED, BLUE, GREEN, YELLOW };  

    Board gameBoard = { 0 };
    ColorButton colorButtons[COLOR_COUNT];

    for (int i = 0; i < COLOR_COUNT; i++) {
        colorButtons[i].color = availableColors[i];
        colorButtons[i].buttonRect = (Rectangle){ 50 + (i * 60), 300, 50, 50 };
    }

    int selectedColorIndex = -1;
    int currentPlayer = 0;

    LoadBaseTankTextures();
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
                DrawText("Select Number of Players", 200, 200, 30, DARKGRAY);

                Rectangle player2Button = { 300, 300, 100, 50 };
                Rectangle player4Button = { 450, 300, 100, 50 };
                DrawButton(player2Button, LIGHTGRAY, "2 Players");
                DrawButton(player4Button, LIGHTGRAY, "4 Players");

                if (IsButtonClicked(player2Button)) {
                    numPlayers = 2;
                    screen = COLOR_SELECT;
                }
                if (IsButtonClicked(player4Button)) {
                    numPlayers = 4;
                    screen = COLOR_SELECT;
                }

                EndDrawing();
                break;

            case COLOR_SELECT:
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Select Color for Player", 200, 200, 30, DARKGRAY);

                DrawText(TextFormat("Player %d", currentPlayer + 1), 200, 250, 20, DARKGRAY);

                DrawColorButtons(colorButtons, COLOR_COUNT);

                selectedColorIndex = CheckColorSelection(colorButtons, COLOR_COUNT);
                if (selectedColorIndex != -1) {
                    chosenColors[currentPlayer] = availableColors[selectedColorIndex];
                    currentPlayer++;

                    if (currentPlayer >= numPlayers) {
                        screen = GAME_SCREEN;
                        currentPlayer = 0;
                        initialize_board(&gameBoard, characters, numPlayers);
                        initialize_characters(characters, numPlayers, isAI, selectedTypes, chosenColors);
                    }
                }

                EndDrawing();
                break;

            case GAME_SCREEN:
                BeginDrawing();
                ClearBackground(BLACK);
                
                draw_board(&gameBoard, characters, numPlayers, offsetX, offsetY);
                draw_tanks(characters, numPlayers, offsetX, offsetY);
                draw_currency(characters, numPlayers);

                EndDrawing();
                break;

            case GAME_OVER:
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Game Over", 400, 300, 30, RED);
                EndDrawing();
                break;
        }
    }

    CloseWindow();
}
void Run() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "You Vs AI - Maze Game");
    int offsetX = (SCREEN_WIDTH - GRID_SIZE * CELL_SIZE) / 2;
    int offsetY = (SCREEN_HEIGHT - GRID_SIZE * CELL_SIZE) / 2;

    GameScreen screen = TITLE_SCREEN;
    int numPlayers = 0;
    bool isAI[4] = { false, false, false, false };
    int playerConfig[4] = { 0, 0, 0, 0 };
    CharacterType selectedTypes[4];
    Character characters[4];
    Color chosenColors[4] = { RED, BLUE, GREEN, YELLOW };  

    Board gameBoard = { 0 };

    LoadBaseTankTextures();

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
                DrawText("Select Number of Players", 200, 150, 30, DARKGRAY);

                Rectangle twoPlayerButton = { 250, 300, 100, 50 };
                Rectangle fourPlayerButton = { 450, 300, 100, 50 };
                DrawButton(twoPlayerButton, LIGHTGRAY, "2 Players");
                DrawButton(fourPlayerButton, LIGHTGRAY, "4 Players");

                if (IsButtonClicked(twoPlayerButton)) {
                    numPlayers = 2;
                    for (int i = 0; i < 2; i++) {
                        selectedTypes[i] = PLAYER_ONE + i;  
                    }
                    screen = GAME_SCREEN;
                } else if (IsButtonClicked(fourPlayerButton)) {
                    numPlayers = 4;
                    for (int i = 0; i < 4; i++) {
                        selectedTypes[i] = PLAYER_ONE + i;  
                    }
                    screen = GAME_SCREEN;
                }

                EndDrawing();
                break;

            case GAME_SCREEN:
                initialize_characters(characters, numPlayers, isAI, selectedTypes, chosenColors);
                initialize_board(&gameBoard, characters, numPlayers);
                screen = GAME_PLAY;
                break;

            case GAME_PLAY:
                BeginDrawing();
                ClearBackground(RAYWHITE);

                draw_board(&gameBoard, characters, numPlayers, offsetX, offsetY);
                draw_tanks(characters, numPlayers, offsetX, offsetY);
                draw_currency(characters, numPlayers);

                update_tanks(&gameBoard, characters, numPlayers);
                update_currency(characters, numPlayers);
                EndDrawing();
                break;

            case GAME_OVER:
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Game Over", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 40, DARKGRAY);
                EndDrawing();
                break;
        }
    }

    UnloadBaseTankTextures();
    CloseWindow();
}

int main(void) {
    Run();
    return 0;
}

