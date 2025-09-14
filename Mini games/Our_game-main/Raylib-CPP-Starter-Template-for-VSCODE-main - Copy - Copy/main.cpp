#include <raylib.h>
#include <limits.h>

// Initialize Window Screen
const int screenWidth = 600;
const int screenHeight = 600;
// Initialize Cell Size
const int cellSize = 200;

// Create 3x3 Tic Tac Toe Board with 2D Array to take in 'X' and 'O' characters
char board[3][3] = { {' ', ' ', ' '}, 
                     {' ', ' ', ' '},  
                     {' ', ' ', ' '} };

// First Player is 'X' and Second Player is 'O'
bool playerXTurn = true;
bool gameOver = false;
char winner = ' ';

// At the start of the game, the game will be in two-player mode. Can change if wanted
bool singlePlayer = false; // Toggle between single-player and two-player mode


// Draw the lines of the board
void DrawBoard() 
{
    for (int i = 1; i < 3; i++) 
    {
        // DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)
        DrawLine(i * cellSize, 0, i * cellSize, screenHeight, WHITE);
        DrawLine(0, i * cellSize, screenWidth, i * cellSize, WHITE);
    }
}

// Draw the 'X' and 'O' marks on the board
void DrawMarks() 
{
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (board[i][j] == 'X') 
            {
                // DrawText(const char *text, int posX, int posY, int fontSize, Color color)
                DrawText("X", j * cellSize + 40, i * cellSize + 5, 200, RED);
            } 
            else if (board[i][j] == 'O') 
            {
                // DrawText(const char *text, int posX, int posY, int fontSize, Color color)
                DrawText("O", j * cellSize + 40, i * cellSize + 5, 200, BLUE);
            }
        }
    }
}

bool CheckWin() 
{
    for (int i = 0; i < 3; i++) 
    {
        // If the win is horizontal EX: 00, 01, 02; the character in those cells is the winner
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) 
        {
            winner = board[i][0];
            return true;
        }
         // If the win is vertical EX: 00, 10, 20; the character in those cells is the winner
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) 
        {
            winner = board[0][i];
            return true;
        }
    }
    // If the win is diagonal EX: 00, 11, 22; the character in those cells is the winner
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) 
    {
        winner = board[0][0];
        return true;
    }
    // If the win is diagonal EX: 02, 11, 20; the character in those cells is the winner
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) 
    {
        winner = board[0][2];
        return true;
    }
    return false;
}

// Check if the board is full or not
bool IsBoardFull() 
{
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            // If there is an empty cell, the board is not full so return false
            if (board[i][j] == ' ') return false;
        }
    }
    // Otherwise, the board is full
    return true;
}

// Trying to implement the Minimax Algorithm for the One Player Mode
// Minimax algorithm to determine the best move for AI
int Minimax(bool isMaximizing) 
{
    if (CheckWin()) return (winner == 'O') ? 1 : -1;
    if (IsBoardFull()) return 0;

    int bestScore = isMaximizing ? INT_MIN : INT_MAX;
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            if (board[i][j] == ' ') 
            {
                board[i][j] = isMaximizing ? 'O' : 'X';
                int score = Minimax(!isMaximizing);
                board[i][j] = ' ';
                bestScore = isMaximizing ? (score > bestScore ? score : bestScore)
                                         : (score < bestScore ? score : bestScore);
            }
        }
    }
    return bestScore;
}

// create AI_Move function to make the AI move
// AI chooses the best move based on Minimax function
void AI_Move() 
{
    if (gameOver) return;
    int bestScore = INT_MIN;
    int moveRow = -1, moveCol = -1;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) 
        {
            if (board[i][j] == ' ') 
            {
                board[i][j] = 'O';
                int score = Minimax(false);
                board[i][j] = ' ';
                if (score > bestScore) 
                {
                    bestScore = score;
                    moveRow = i;
                    moveCol = j;
                }
            }
        }
    }
    if (moveRow != -1 && moveCol != -1) {
        board[moveRow][moveCol] = 'O';
        if (CheckWin()) gameOver = true;
        else if (IsBoardFull()) gameOver = true;
        playerXTurn = true; // Ensure turn order remains correct
    }
}

// Reset the game when 'R' key is pressed
void ResetGame() 
{
    for (int i = 0; i < 3; i++) 
    {
        for (int j = 0; j < 3; j++) 
        {
            board[i][j] = ' ';
        }
    }
    playerXTurn = true;
    gameOver = false;
    winner = ' ';
}

int main() 
{
    // Initialize Window / InitWindow(int width, int height, const char *title)
    InitWindow(screenWidth, screenHeight, "Tic Tac Toe");
    // Set Target FPS / SetTargetFPS(int fps)
    SetTargetFPS(60);
    
    // Game Loop
    while (!WindowShouldClose()) 
    {
        // Detect Key Pressed if the player wants to play single-player or two-player mode or restart the game
        // IsKeyPressed(int key) returns true if the key is pressed
        if (IsKeyPressed(KEY_ONE)) singlePlayer = true;
        if (IsKeyPressed(KEY_TWO)) singlePlayer = false;
        if (IsKeyPressed(KEY_R)) ResetGame();

        // If the game is not over and the left mouse button is pressed, the player can make a move
        // IsMouseButtonPressed(int button)
        // GetMousePosition() returns the mouse position as a Vector2
        if (!gameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            // A Vector2 is a structure that contains two floating-point values (x and y), representing the horizontal and vertical position of the cursor
            // GetMousePosition() is a function from the Raylib library returns the current position of the mouse cursor on the screen as a Vector2 type
            // This declares a variable mousePos of type Vector2, which will store the mouse's current coordinates
            Vector2 mousePos = GetMousePosition();
            int row = mousePos.y / cellSize;
            int col = mousePos.x / cellSize;

            if (row >= 0 && row < 3 && col >= 0 && col < 3 && board[row][col] == ' ') 
            {
                board[row][col] = playerXTurn ? 'X' : 'O';
                if (CheckWin()) gameOver = true;
                else if (IsBoardFull()) gameOver = true;
                playerXTurn = !playerXTurn;
                if (!gameOver && singlePlayer && !playerXTurn) AI_Move();
            }
        }

        BeginDrawing(); // Start drawing of canvas
        ClearBackground(BLACK); // Black Color Background
        DrawBoard(); // Function Call, Draw the board
        DrawMarks(); // Function Call, Draw the 'X' and 'O' marks

        // If the game is over, display the winner or draw message
        if (gameOver) {
            if (winner != ' ') 
            {
                DrawText(TextFormat("%c Wins", winner), 150, 260, 100, GREEN);
            } 
            else 
            {
                DrawText("DRAW", 160, 260, 100, WHITE);
            }
        }

        // Display the game mode and restart instructions at the bottom of the screen
        // DrawText(const char *text, int posX, int posY, int fontSize, Color color)
        DrawText("Press 1: Single Player", 20, 550, 20, WHITE);
        DrawText("Press 2: Two Players", 300, 550, 20, WHITE);
        DrawText("Press R: Restart", 230, 580, 20, WHITE);

        EndDrawing(); // End drawing of canvas
    }
    CloseWindow(); // Close the window
    return 0;
}
