#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//--------------------------------------------------------------------------------------------------
// Shared definitions and global constants
//--------------------------------------------------------------------------------------------------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Game states for switching between mini games
typedef enum GameState {
    MENU,
    TIC_TAC_TOE,
    SNAKE,
    SPACE_INVADERS
} GameState;

//--------------------------------------------------------------------------------------------------
// TIC TAC TOE
//--------------------------------------------------------------------------------------------------
#define BOARD_SIZE 300  // size in pixels for the board

// 0 = empty, 1 = X, 2 = O
int tttBoard[3][3];
int tttCurrentPlayer = 1;
bool tttGameOver = false;
int tttWinner = 0; // 0 = none, 1 or 2 win; 3 = draw

// Reset the board
void InitTicTacToe() {
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            tttBoard[i][j] = 0;
        }
    }
    tttCurrentPlayer = 1;
    tttGameOver = false;
    tttWinner = 0;
}

// Check rows, columns and diagonals for a winner
int CheckTicTacToeWinner() {
    // Check rows
    for (int i = 0; i < 3; i++){
        if(tttBoard[i][0] != 0 && tttBoard[i][0] == tttBoard[i][1] && tttBoard[i][1] == tttBoard[i][2])
            return tttBoard[i][0];
    }
    // Check columns
    for (int j = 0; j < 3; j++){
        if(tttBoard[0][j] != 0 && tttBoard[0][j] == tttBoard[1][j] && tttBoard[1][j] == tttBoard[2][j])
            return tttBoard[0][j];
    }
    // Check diagonals
    if(tttBoard[0][0] != 0 && tttBoard[0][0] == tttBoard[1][1] && tttBoard[1][1] == tttBoard[2][2])
        return tttBoard[0][0];
    if(tttBoard[0][2] != 0 && tttBoard[0][2] == tttBoard[1][1] && tttBoard[1][1] == tttBoard[2][0])
        return tttBoard[0][2];

    // Check draw: if no cells are empty, then it is a draw (return a special value, e.g. 3)
    bool draw = true;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if (tttBoard[i][j] == 0) {
                draw = false;
                break;
            }
        }
    }
    if (draw) return 3;
    return 0;
}

void UpdateDrawTicTacToe() {
    // Calculate board positioning for centering
    int boardSize = BOARD_SIZE;
    int startX = (SCREEN_WIDTH - boardSize) / 2;
    int startY = (SCREEN_HEIGHT - boardSize) / 2;
    int cellSize = boardSize / 3;

    // Process input if game is not over
    if (!tttGameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        if (mouse.x >= startX && mouse.x < startX + boardSize &&
            mouse.y >= startY && mouse.y < startY + boardSize) {
            int col = (mouse.x - startX) / cellSize;
            int row = (mouse.y - startY) / cellSize;
            if (tttBoard[row][col] == 0) {
                tttBoard[row][col] = tttCurrentPlayer;
                int winner = CheckTicTacToeWinner();
                if (winner != 0) {
                    tttGameOver = true;
                    tttWinner = winner;
                } else {
                    tttCurrentPlayer = (tttCurrentPlayer == 1) ? 2 : 1;
                }
            }
        }
    }

    // Draw board border and grid
    DrawRectangleLines(startX, startY, boardSize, boardSize, BLACK);
    for (int i = 1; i < 3; i++) {
        DrawLine(startX + i * cellSize, startY, startX + i * cellSize, startY + boardSize, BLACK);
        DrawLine(startX, startY + i * cellSize, startX + boardSize, startY + i * cellSize, BLACK);
    }

    // Draw players' marks
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            int centerX = startX + j * cellSize + cellSize / 2;
            int centerY = startY + i * cellSize + cellSize / 2;
            if (tttBoard[i][j] == 1)
                DrawText("X", centerX - 10, centerY - 10, 20, RED);
            else if (tttBoard[i][j] == 2)
                DrawText("O", centerX - 10, centerY - 10, 20, BLUE);
        }
    }

    // Display result if game is over
    if (tttGameOver) {
        char msg[50];
        if (tttWinner == 3)
            sprintf(msg, "Draw! Press R to restart");
        else
            sprintf(msg, "Player %d wins! Press R to restart", tttWinner);
        DrawText(msg, startX, startY + boardSize + 20, 20, BLACK);
    }
}

//--------------------------------------------------------------------------------------------------
// SNAKE
//--------------------------------------------------------------------------------------------------
#define SNAKE_MAX_LENGTH 100
#define SNAKE_CELL_SIZE 20

typedef struct {
    int x;
    int y;
} Vector2i;

Vector2i snake[SNAKE_MAX_LENGTH];
int snakeLength = 0;
int snakeDirX = 1;
int snakeDirY = 0;
Vector2i snakeFood;
bool snakeGameOver = false;
float snakeMoveTimer = 0.0f;
float snakeMoveDelay = 0.15f; // seconds per move

// Derive grid dimensions (based on cell size)
int snakeGridWidth = SCREEN_WIDTH / SNAKE_CELL_SIZE;
int snakeGridHeight = SCREEN_HEIGHT / SNAKE_CELL_SIZE;

void InitSnake() {
    snakeLength = 3;
    int startX = snakeGridWidth / 2;
    int startY = snakeGridHeight / 2;
    for (int i = 0; i < snakeLength; i++){
        snake[i].x = startX - i;
        snake[i].y = startY;
    }
    snakeDirX = 1;
    snakeDirY = 0;
    snakeGameOver = false;
    snakeMoveTimer = 0.0f;
    snakeFood.x = GetRandomValue(0, snakeGridWidth - 1);
    snakeFood.y = GetRandomValue(0, snakeGridHeight - 1);
}

void UpdateDrawSnake() {
    float delta = GetFrameTime();
    snakeMoveTimer += delta;

    // Change direction (disallow reverse)
    if (IsKeyPressed(KEY_UP) && snakeDirY == 0) { snakeDirX = 0; snakeDirY = -1; }
    else if (IsKeyPressed(KEY_DOWN) && snakeDirY == 0) { snakeDirX = 0; snakeDirY = 1; }
    else if (IsKeyPressed(KEY_LEFT) && snakeDirX == 0) { snakeDirX = -1; snakeDirY = 0; }
    else if (IsKeyPressed(KEY_RIGHT) && snakeDirX == 0) { snakeDirX = 1; snakeDirY = 0; }

    if (snakeMoveTimer >= snakeMoveDelay && !snakeGameOver) {
        snakeMoveTimer = 0.0f;
        // Compute new head position
        Vector2i newHead = { snake[0].x + snakeDirX, snake[0].y + snakeDirY };

        // Check wall collision
        if (newHead.x < 0 || newHead.x >= snakeGridWidth || newHead.y < 0 || newHead.y >= snakeGridHeight)
            snakeGameOver = true;
        // Check self-collision
        for (int i = 0; i < snakeLength; i++){
            if (snake[i].x == newHead.x && snake[i].y == newHead.y)
                snakeGameOver = true;
        }

        if (!snakeGameOver) {
            // Shift body segments
            for (int i = snakeLength; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0] = newHead;
            // Handle food collision
            if (newHead.x == snakeFood.x && newHead.y == snakeFood.y) {
                snakeLength++;
                if(snakeLength > SNAKE_MAX_LENGTH) snakeLength = SNAKE_MAX_LENGTH;
                snakeFood.x = GetRandomValue(0, snakeGridWidth - 1);
                snakeFood.y = GetRandomValue(0, snakeGridHeight - 1);
            }
        }
    }

    // Draw snake (each body part as a green square)
    for (int i = 0; i < snakeLength; i++){
        DrawRectangle(snake[i].x * SNAKE_CELL_SIZE, snake[i].y * SNAKE_CELL_SIZE, SNAKE_CELL_SIZE, SNAKE_CELL_SIZE, GREEN);
    }
    // Draw food as a red square
    DrawRectangle(snakeFood.x * SNAKE_CELL_SIZE, snakeFood.y * SNAKE_CELL_SIZE, SNAKE_CELL_SIZE, SNAKE_CELL_SIZE, RED);
    
    if (snakeGameOver) {
        DrawText("Game Over! Press R to restart", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2, 20, BLACK);
    }
}

//--------------------------------------------------------------------------------------------------
// SPACE INVADERS
//--------------------------------------------------------------------------------------------------
#define INVADER_ROWS 3
#define INVADER_COLS 8

typedef struct {
    Vector2 position;
    bool alive;
} Invader;

Invader invaders[INVADER_ROWS][INVADER_COLS];
float invaderSpeed = 20.0f;
int invaderDirection = 1; // 1 moves right, -1 moves left

// Player variables
Vector2 playerPos;
float playerSpeed = 200.0f;

// Bullet structure (only one active bullet is allowed for simplicity)
typedef struct {
    Vector2 position;
    bool active;
    float speed;
} Bullet;

Bullet playerBullet;
bool spaceGameOver = false;

void InitSpaceInvaders() {
    // Arrange invaders in a grid
    int spacing = 50;
    int startX = 100;
    int startY = 50;
    for (int i = 0; i < INVADER_ROWS; i++){
        for (int j = 0; j < INVADER_COLS; j++){
            invaders[i][j].alive = true;
            invaders[i][j].position.x = startX + j * spacing;
            invaders[i][j].position.y = startY + i * spacing;
        }
    }
    invaderDirection = 1;
    
    // Place player at bottom center
    playerPos.x = SCREEN_WIDTH / 2;
    playerPos.y = SCREEN_HEIGHT - 50;
    
    // Initialize bullet
    playerBullet.active = false;
    playerBullet.speed = 300.0f;
    
    spaceGameOver = false;
}

void UpdateInvaders(float delta) {
    bool changeDirection = false;
    // Move each invader horizontally
    for (int i = 0; i < INVADER_ROWS; i++){
        for (int j = 0; j < INVADER_COLS; j++){
            if (invaders[i][j].alive) {
                invaders[i][j].position.x += invaderSpeed * invaderDirection * delta;
                if (invaders[i][j].position.x < 0 || invaders[i][j].position.x > SCREEN_WIDTH - 30) {
                    changeDirection = true;
                }
            }
        }
    }
    if (changeDirection) {
        invaderDirection *= -1;
        // Move invaders down when they hit an edge
        for (int i = 0; i < INVADER_ROWS; i++){
            for (int j = 0; j < INVADER_COLS; j++){
                invaders[i][j].position.y += 20;
                // If any invader reaches near the player, the game ends
                if (invaders[i][j].position.y > playerPos.y - 30)
                    spaceGameOver = true;
            }
        }
    }
}

void UpdatePlayer(float delta) {
    if (IsKeyDown(KEY_LEFT)) {
        playerPos.x -= playerSpeed * delta;
        if (playerPos.x < 0) playerPos.x = 0;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        playerPos.x += playerSpeed * delta;
        if (playerPos.x > SCREEN_WIDTH) playerPos.x = SCREEN_WIDTH;
    }
}

void UpdateBullet(float delta) {
    if (playerBullet.active) {
        playerBullet.position.y -= playerBullet.speed * delta;
        if (playerBullet.position.y < 0) {
            playerBullet.active = false;
        }
    } else {
        if (IsKeyPressed(KEY_SPACE)) {
            playerBullet.active = true;
            playerBullet.position.x = playerPos.x;
            playerBullet.position.y = playerPos.y;
        }
    }
}

void CheckBulletCollisions() {
    if (playerBullet.active) {
        for (int i = 0; i < INVADER_ROWS; i++){
            for (int j = 0; j < INVADER_COLS; j++){
                if (invaders[i][j].alive) {
                    Rectangle invaderRect = { invaders[i][j].position.x, invaders[i][j].position.y, 30, 30 };
                    Rectangle bulletRect   = { playerBullet.position.x, playerBullet.position.y, 5, 10 };
                    if (CheckCollisionRecs(invaderRect, bulletRect)) {
                        invaders[i][j].alive = false;
                        playerBullet.active = false;
                    }
                }
            }
        }
    }
}

void UpdateDrawSpaceInvaders() {
    float delta = GetFrameTime();
    if (!spaceGameOver) {
        UpdateInvaders(delta);
        UpdatePlayer(delta);
        UpdateBullet(delta);
        CheckBulletCollisions();
    }

    // Draw the player ship
    DrawRectangleV(playerPos, (Vector2){40, 20}, BLUE);
    // Draw bullet if active
    if (playerBullet.active)
        DrawRectangle(playerBullet.position.x, playerBullet.position.y, 5, 10, RED);
    // Draw invaders
    for (int i = 0; i < INVADER_ROWS; i++){
        for (int j = 0; j < INVADER_COLS; j++){
            if (invaders[i][j].alive)
                DrawRectangleV(invaders[i][j].position, (Vector2){30, 30}, GREEN);
        }
    }

    // Check win condition: if all invaders are eliminated
    bool anyAlive = false;
    for (int i = 0; i < INVADER_ROWS; i++){
        for (int j = 0; j < INVADER_COLS; j++){
            if (invaders[i][j].alive)
                anyAlive = true;
        }
    }
    if (!anyAlive) {
        DrawText("You Win! Press R to restart", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2, 20, BLACK);
        spaceGameOver = true;
    }
    
    if (spaceGameOver)
        DrawText("Game Over! Press R to restart", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 30, 20, BLACK);
}

//--------------------------------------------------------------------------------------------------
// MAIN FUNCTION: MENU & GAME STATE HANDLING
//--------------------------------------------------------------------------------------------------
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Multi Mini Games - Raylib");
    SetTargetFPS(60);
    srand(time(NULL));   // Initialize random seed

    GameState currentState = MENU;

    // Preinitialize each mini game (they will also reinitialize upon restarting)
    InitTicTacToe();
    InitSnake();
    InitSpaceInvaders();

    while (!WindowShouldClose())    // Main game loop
    {
        // (Global) If not in menu and player presses R, restart current mini game
        if(currentState != MENU && IsKeyPressed(KEY_R)){
            switch(currentState) {
                case TIC_TAC_TOE: InitTicTacToe(); break;
                case SNAKE:      InitSnake();      break;
                case SPACE_INVADERS: InitSpaceInvaders(); break;
                default: break;
            }
        }
        // Allow returning to the main menu with ESC
        if(currentState != MENU && IsKeyPressed(KEY_Q)){
            currentState = MENU;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Handle game state
        switch(currentState) {
            case MENU: {
                DrawText("Multi Mini Games", SCREEN_WIDTH/2 - 120, 50, 30, DARKBLUE);
                DrawText("Press 1: Tic Tac Toe", SCREEN_WIDTH/2 - 100, 150, 20, BLACK);
                DrawText("Press 2: Snake", SCREEN_WIDTH/2 - 100, 200, 20, BLACK);
                DrawText("Press 3: Space Invaders", SCREEN_WIDTH/2 - 100, 250, 20, BLACK);
                DrawText("Press ESC in any mini game to return here", SCREEN_WIDTH/2 - 170, 500, 15, GRAY);
                if (IsKeyPressed(KEY_ONE)) { currentState = TIC_TAC_TOE; InitTicTacToe(); }
                else if (IsKeyPressed(KEY_TWO)) { currentState = SNAKE; InitSnake(); }
                else if (IsKeyPressed(KEY_THREE)) { currentState = SPACE_INVADERS; InitSpaceInvaders(); }
            } break;
            case TIC_TAC_TOE:
                UpdateDrawTicTacToe();
                break;
            case SNAKE:
                UpdateDrawSnake();
                break;
            case SPACE_INVADERS:
                UpdateDrawSpaceInvaders();
                break;
            default:
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
