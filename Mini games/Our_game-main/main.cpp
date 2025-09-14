#include <raylib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits.h>
using namespace std;

//Game state
enum GameState {
    MENU,
    SPACEINVADERS,
    TIC_TAC_TOE,
    MARIO,
    SNAKE
};


namespace SpaceInvadersGame {

    // Modified Alien structure now includes a health field.
    struct Alien {
        Vector2 position;
        bool alive;
        Texture2D image;  
    };

    struct Laser {
        Vector2 position;
        int speed;
    };

    // Constants for drawing
    const int SPACESHIP_WIDTH = 40;
    const int SPACESHIP_HEIGHT = 20;
    const int ALIEN_WIDTH = 30;
    const int ALIEN_HEIGHT = 20;

    // Space Invaders
    void Run() {
        int gameLevel = 1;  
        
        //Player Variables
        Vector2 spaceshipPos = { 1280/2 - SPACESHIP_WIDTH/2, 680 };  
        int spaceshipSpeed = 5;  
        int spaceshipLives = 3; 
        double lastFireTime = 0.0; 
        double fireCooldown = 0.3; 
        vector<Laser> shipLasers; 
    
        // Load spaceship texture
        Texture2D spaceshipTexture = LoadTexture("spaceship.png");
        
        // Alien variables
        vector<Alien> aliens; 
        const int alienRows = 5, alienCols = 8;  
        float alienSpacingX = 70, alienSpacingY = 50; 
        float startX = 150, startY = 50;
    
        // Alien speed
        float alienSpeed = 1.5; 
        float speedIncreaseFactor = 1.4f;  // Alien speed increases by 40% per level
        float alienMoveDown = 20;  
    
        // Generate aliens based on level
        auto generateAliens = [&]() {
            aliens.clear();
            for (int i = 0; i < alienRows; i++) {
                for (int j = 0; j < alienCols; j++) {
                    Alien a;
                    a.position = { startX + j * alienSpacingX, startY + i * alienSpacingY };
                    a.alive = true;
    
                    // Alien type based on row
                    if (i == 3 || i == 4) {
                        a.image = LoadTexture("alien_1.png");
                    } else if (i == 2) {
                        a.image = LoadTexture("alien_2.png");
                    } else if (i == 1) {
                        a.image = LoadTexture("alien_2.png");
                    } else {
                        a.image = LoadTexture("alien_3.png"); 
                    }
    
                    aliens.push_back(a);
                }
            }
        };
        generateAliens(); 
    
        // Alien direction and movement
        int alienDirection = 1; 
        float alienMaxX = 1280 - ALIEN_WIDTH; 
        float alienMinX = 0; 
    
        // Alien Laser Variables
        vector<Laser> alienLasers; 
        double lastAlienFireTime = 0.0;
        double alienFireInterval = 2.0; 
    
        int score = 0;  // Player score
        bool gameOver = false, win = false; 
    
        SetTargetFPS(60); 
    
        // Main Game Loop
        while (!WindowShouldClose()) {
            // Player Input  Updates
            if (IsKeyDown(KEY_A)) {
                spaceshipPos.x -= spaceshipSpeed;  // Move left
                if (spaceshipPos.x < 0) spaceshipPos.x = 0; 
            }
            if (IsKeyDown(KEY_D)) {
                spaceshipPos.x += spaceshipSpeed;  // Move right
                if (spaceshipPos.x + SPACESHIP_WIDTH > 1280)
                    spaceshipPos.x = 1280 - SPACESHIP_WIDTH; 
            }
            if (IsKeyDown(KEY_SPACE) && (GetTime() - lastFireTime >= fireCooldown)) {
                // Shoot laser
                Laser l;
                l.position = { spaceshipPos.x + SPACESHIP_WIDTH/2 - 2, spaceshipPos.y };
                l.speed = -7; 
                shipLasers.push_back(l);
                lastFireTime = GetTime();
            }
    
            // Update Player Lasers
            for (size_t i = 0; i < shipLasers.size(); i++) {
                shipLasers[i].position.y += shipLasers[i].speed; 
                if (shipLasers[i].position.y < 0) {
                    shipLasers.erase(shipLasers.begin() + i);  // Remove off screen lasers
                    i--;
                }
            }
    
            // Alien Movement
            bool moveDown = false;
            for (auto &a : aliens) {
                if (!a.alive) continue;
    
                a.position.x += alienSpeed * alienDirection;  // Move left or right
    
                // left and right bounds check
                if (a.position.x > alienMaxX || a.position.x < alienMinX) {
                    moveDown = true;
                }
            }
    
            // Move aliens down if wall is hit
            if (moveDown) {
                alienDirection *= -1;
                for (auto &a : aliens) {
                    if (a.alive) a.position.y += alienMoveDown;
                }
            }
    
            // Check collisions player laser and alien
            for (size_t i = 0; i < shipLasers.size(); i++) {
                for (size_t j = 0; j < aliens.size(); j++) {
                    if (!aliens[j].alive) continue;
                    Rectangle laserRect = { shipLasers[i].position.x, shipLasers[i].position.y, 5, 10 };
                    Rectangle alienRect = { aliens[j].position.x, aliens[j].position.y, ALIEN_WIDTH, ALIEN_HEIGHT };
                    if (CheckCollisionRecs(laserRect, alienRect)) {
                        aliens[j].alive = false;
                        shipLasers.erase(shipLasers.begin() + i);
                        i--;
                        score += 100; 
                        break;
                    }
                }
            }
    
            // Update Alien Lasers
            for (size_t i = 0; i < alienLasers.size(); i++) {
                alienLasers[i].position.y += alienLasers[i].speed;  
                if (alienLasers[i].position.y > 720) {
                    alienLasers.erase(alienLasers.begin() + i);  // Remove off-screen lasers
                    i--;
                }
            }
    
            // Alien Firing
            if (GetTime() - lastAlienFireTime >= alienFireInterval) {
                vector<int> aliveIndices;
                for (size_t i = 0; i < aliens.size(); i++) {
                    if (aliens[i].alive) aliveIndices.push_back(i);
                }
                if (!aliveIndices.empty()) {
                    int index = aliveIndices[GetRandomValue(0, aliveIndices.size() - 1)];
                    Laser l;
                    l.position = { aliens[index].position.x + ALIEN_WIDTH/2 - 2, aliens[index].position.y + ALIEN_HEIGHT };
                    l.speed = 5;
                    alienLasers.push_back(l);
                    lastAlienFireTime = GetTime();
                }
            }
    
            // Check collisions alien laser and player
            Rectangle spaceshipRect = { spaceshipPos.x, spaceshipPos.y, SPACESHIP_WIDTH, SPACESHIP_HEIGHT };
            for (size_t i = 0; i < alienLasers.size(); i++) {
                Rectangle laserRect = { alienLasers[i].position.x, alienLasers[i].position.y, 5, 10 };
                if (CheckCollisionRecs(laserRect, spaceshipRect)) {
                    spaceshipLives--; 
                    alienLasers.erase(alienLasers.begin() + i);
                    i--;
                    if (spaceshipLives <= 0) gameOver = true;
                }
            }
    
            // Level Progression
            bool allDead = true;
            for (auto &a : aliens) {
                if (a.alive) { 
                    allDead = false; 
                    break; 
                }
            }
            if (allDead) {
                if (gameLevel < 3) {
                    gameLevel++;
                    alienSpeed *= speedIncreaseFactor;
                    generateAliens();  // Regenerate aliens
                } else {
                    win = true;  // Win if all levels are complete
                }
            }
    
            // Drawing
            BeginDrawing();
            if (gameOver) {
                ClearBackground(BLACK);
                DrawText("GAME OVER", 600, 300, 40, RED);
                DrawText(TextFormat("Score: %i", score), 600, 350, 30, WHITE);
                DrawText("Press ENTER to return to Menu", 550, 400, 20, YELLOW);
            }
            else if (win) {
                ClearBackground(BLACK);
                DrawText("YOU WIN!", 600, 300, 40, GREEN);
                DrawText(TextFormat("Score: %i", score), 600, 350, 30, WHITE);
                DrawText("Press ENTER to return to Menu", 550, 400, 20, YELLOW);
            }
            else {
                ClearBackground(BLACK);
                // Draw player spaceship
                DrawTexture(spaceshipTexture, spaceshipPos.x, spaceshipPos.y, WHITE);
                DrawText(TextFormat("Lives: %i", spaceshipLives), 10, 10, 20, WHITE);
                DrawText(TextFormat("Score: %i", score), 10, 40, 20, WHITE);
                DrawText(TextFormat("Level: %i", gameLevel), 10, 70, 20, WHITE);
    
                // Draw player lasers
                for (auto &l : shipLasers) {
                    DrawRectangle(l.position.x, l.position.y, 5, 10, WHITE);
                }
    
                // Draw aliens
                for (auto &a : aliens) {
                    if (a.alive) {
                        DrawTexture(a.image, a.position.x, a.position.y, WHITE);  // Draw alien image
                    }
                }
    
                // Draw alien lasers
                for (auto &l : alienLasers) {
                    DrawRectangle(l.position.x, l.position.y, 5, 10, RED);
                }
            }
            EndDrawing();
    
            // Return to Menu if game ends
            if ((gameOver || win) && IsKeyPressed(KEY_ENTER))
                break;
        }
    
        // Unload textures
        UnloadTexture(spaceshipTexture);
    }
}    
    // End space invaders

// Tic Tac Toe Game Namespace
namespace TicTacToeGame {
// Initialize Window Screen
    const int screenWidth = 600;
    const int screenHeight = 600;
// Initialize Cell Size
    const int cellSize = 200;

// Create 3x3 Tic Tac Toe Board with 2D Array to take in 'X' and 'O' characters
    char board[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} };

// First Player is 'X' and Second Player is 'O'
    bool playerXTurn = true;
    bool gameOver = false;
    char winner = ' ';
// At the start of the game, the game will be in two-player mode. Can change if wanted
    bool singlePlayer = false;  // Toggle between single-player and two-player mode

// Draw the lines of the board
    void DrawBoard() {
        for (int i = 1; i < 3; i++) {
            // DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color)
            DrawLine(i * cellSize, 0, i * cellSize, screenHeight, WHITE);
            DrawLine(0, i * cellSize, screenWidth, i * cellSize, WHITE);
        }
    }

// Draw the 'X' and 'O' marks on the board
    void DrawMarks() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == 'X')
                    // DrawText(const char *text, int posX, int posY, int fontSize, Color color)
                    DrawText("X", j * cellSize + 40, i * cellSize + 5, 200, RED);
                else if (board[i][j] == 'O')
                    DrawText("O", j * cellSize + 40, i * cellSize + 5, 200, BLUE);
            }
    }
    
    bool CheckWin() {
        for (int i = 0; i < 3; i++) {
            // If the win is horizontal EX: 00, 01, 02; the character in those cells is the winner
            if (board[i][0] != ' ' && board[i][0]==board[i][1] && board[i][1]==board[i][2]) { winner = board[i][0]; return true; }
            // If the win is vertical EX: 00, 10, 20; the character in those cells is the winner
            if (board[0][i] != ' ' && board[0][i]==board[1][i] && board[1][i]==board[2][i]) { winner = board[0][i]; return true; }
        }
        // If the win is diagonal EX: 00, 11, 22; the character in those cells is the winner
        if (board[0][0] != ' ' && board[0][0]==board[1][1] && board[1][1]==board[2][2]) { winner = board[0][0]; return true; }
        // If the win is diagonal EX: 02, 11, 20; the character in those cells is the winner
        if (board[0][2] != ' ' && board[0][2]==board[1][1] && board[1][1]==board[2][0]) { winner = board[0][2]; return true; }
        return false;
    }

// Check if the board is full or not
    bool IsBoardFull() {
                                                    // If there is an empty cell, the board is not full so return false
        for (int i=0;i<3;i++) for (int j=0;j<3;j++) if (board[i][j]==' ') return false;
        // Otherwise, the board is full
        return true;
    }

// Trying to implement the Minimax Algorithm for the One Player Mode
// Minimax algorithm to determine the best move for AI
    int Minimax(bool isMaximizing) {
        if (CheckWin()) return (winner=='O')?1:-1;
        if (IsBoardFull()) return 0;
        int bestScore = isMaximizing ? INT_MIN : INT_MAX;
        for (int i=0;i<3;i++){
            for (int j=0;j<3;j++){
                if (board[i][j]==' '){
                    board[i][j] = isMaximizing ? 'O' : 'X';
                    int score = Minimax(!isMaximizing);
                    board[i][j] = ' ';
                    bestScore = isMaximizing ? max(score, bestScore) : min(score, bestScore);
                }
            }
        }
        return bestScore;
    }

// create AI_Move function to make the AI move
// AI chooses the best move based on Minimax function
    void AI_Move() {
        if (gameOver) return;
        int bestScore = INT_MIN, moveRow=-1, moveCol=-1;
        for (int i=0;i<3;i++){
            for (int j=0;j<3;j++){
                if (board[i][j]==' '){
                    board[i][j] = 'O';
                    int score = Minimax(false);
                    board[i][j] = ' ';
                    if (score > bestScore) { bestScore = score; moveRow = i; moveCol = j; }
                }
            }
        }
        if (moveRow!=-1 && moveCol!=-1){
            board[moveRow][moveCol] = 'O';
            if (CheckWin()) gameOver = true;
            else if (IsBoardFull()) gameOver = true;
            playerXTurn = true; // Ensure turn order remains correct
        }
    }

// Reset the game when 'R' key is pressed
    void ResetGame() {
        for (int i=0;i<3;i++) for (int j=0;j<3;j++) board[i][j] = ' ';
        playerXTurn = true; gameOver = false; winner = ' ';
    }
    
    void Run() {
        ResetGame();
        // Texture background for the game
        Texture2D background = LoadTexture("TicTacToeBackground.png");
        // Set Target FPS / SetTargetFPS(int fps)
        SetTargetFPS(60);

        // Game Loop
        while (!WindowShouldClose()) {
        // Detect Key Pressed if the player wants to play single-player or two-player mode or restart the game
        // IsKeyPressed(int key) returns true if the key is pressed
            if (IsKeyPressed(KEY_ONE)) singlePlayer = true;
            if (IsKeyPressed(KEY_TWO)) singlePlayer = false;
            if (IsKeyPressed(KEY_R)) ResetGame();

        // If the game is not over and the left mouse button is pressed, the player can make a move
        // IsMouseButtonPressed(int button)
        // GetMousePosition() returns the mouse position as a Vector2
            if (!gameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            // A Vector2 is a structure that contains two floating-point values (x and y), representing the horizontal and vertical position of the cursor
            // GetMousePosition() is a function from the Raylib library returns the current position of the mouse cursor on the screen as a Vector2 type
            // This declares a variable mousePos of type Vector2, which will store the mouse's current coordinates
                Vector2 mousePos = GetMousePosition();
                int row = mousePos.y / cellSize, col = mousePos.x / cellSize;
                if (row>=0 && row<3 && col>=0 && col<3 && board[row][col]==' '){
                    board[row][col] = playerXTurn ? 'X' : 'O';
                    if (CheckWin()) gameOver = true;
                    else if (IsBoardFull()) gameOver = true;
                    playerXTurn = !playerXTurn;
                    if (!gameOver && singlePlayer && !playerXTurn)
                        AI_Move();
                }
            }
            
            BeginDrawing(); // Start drawing of canvas
                ClearBackground(BLACK); // Black Color Background
                DrawTexture(background, 0, 0, WHITE); // Upload Texture
                DrawBoard();  // Function Call, Draw the board
                DrawMarks(); // Function Call, Draw the 'X' and 'O' marks

            // If the game is over, display the winner or draw message
                if (gameOver) {
                    if (winner != ' ')
                        DrawText(TextFormat("%c Wins", winner), 150, 260, 100, GREEN);
                    else
                        DrawText("DRAW", 160, 260, 100, WHITE);
                    DrawText("Press ENTER to return to Menu", 100, 580, 20, YELLOW);
                }

        // Display the game mode and restart instructions at the bottom of the screen
        // DrawText(const char *text, int posX, int posY, int fontSize, Color color)
                DrawText("Press 1: Single Player", 20, 620, 20, WHITE);
                DrawText("Press 2: Two Players", 300, 620, 20, WHITE);
                DrawText("Press R: Restart", 230, 650, 20, WHITE);
            
            EndDrawing(); // End drawing of canvas
            if (gameOver && IsKeyPressed(KEY_ENTER)) break;
        }
        UnloadTexture(background);
    }
} // end TicTacToeGame

// Mario Platform Game 
namespace MarioGame {

//Texture initialization and global variables
Texture2D Mario;
Texture2D Bowser;
Texture2D goomba;
Texture2D koopa;
Texture2D moving;
Texture2D moving_r;
bool jumping = false;
bool gameOver = false;
bool winner = false;

//SpriteAnimation structure and function to store the textures, store the animation speed and used to draw the sprite animations
typedef struct {
    Texture2D atlas;
    int framesPerSecond;
    Rectangle *rectangles;
    int rectanglesLength;
} SpriteAnimation;

SpriteAnimation _animation;
SpriteAnimation _animation1;
SpriteAnimation _jump;
SpriteAnimation _idle;

//Animation intilization
SpriteAnimation CreateSpriteAnimation(Texture2D atlas, int framesPerSecond, Rectangle rectangles[], int length){
    SpriteAnimation spriteAnimation = { atlas, framesPerSecond, nullptr, length };
    spriteAnimation.rectangles = (Rectangle*)malloc(sizeof(Rectangle) * length);
    if (spriteAnimation.rectangles == NULL){
        TraceLog(LOG_FATAL, "No memory for CreateSpriteAnimation");
        spriteAnimation.rectanglesLength = 0;
        return spriteAnimation;
    }
    for(int i = 0; i < length; i++){
        spriteAnimation.rectangles[i] = rectangles[i];
    }
    return spriteAnimation;
}

void DisposeSpriteAnimation(SpriteAnimation animation){
    free(animation.rectangles);
}

void DrawSpriteAnimationPro(SpriteAnimation animation, Rectangle dest, Vector2 origin, float rotation, Color tint){
    int index = (int)(GetTime() * animation.framesPerSecond) % animation.rectanglesLength;
    Rectangle source = animation.rectangles[index];
    DrawTexturePro(animation.atlas, source, dest, origin, rotation, tint);
}

//Mario's attacks
class Attack {
public:
    //Initializing data members
    Vector2 position;
    Vector2 Velocity;
    float radius;
    //Calculating velocity towards the target (target is where my mouse clicks)
    Attack(Vector2 startPos, Vector2 targetPos, float speed, float r){
        position = startPos;
        radius = r;
        Vector2 direction = { targetPos.x - startPos.x, targetPos.y - startPos.y };
        float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
        direction.x /= magnitude;
        direction.y /= magnitude;
        Velocity = { direction.x * speed, direction.y * speed };
    }
    void Update() { position.x += Velocity.x; position.y += Velocity.y; }
    void Draw() { DrawCircleV(position, radius, RED); }
};

//Platform rendering
class Platform {
public:
    Rectangle rect;

    //Platform will have a constant thickness of one
    Platform(float x, float y, float w) { rect = { x, y, w, 1 }; }
    //Function to draw only a line
    void Draw() { DrawLineEx({rect.x, rect.y}, {rect.x + rect.width, rect.y}, 2, BLACK); }
    //Landing on top of the platforms
    void HandleCollision(float &mario_x, float &mario_y, float mario_w, float mario_h, 
                           float &mario_speed_x, float &mario_speed_y, bool &jumping) {
        if (mario_y + mario_h >= rect.y && //Feet touching platform
            mario_y + mario_h <= rect.y + 15 && //Small buffer for landing  so collision can render when falling quickly
            mario_x + mario_w >= rect.x && //Ensures mario can stand on edges
            mario_x <= rect.x + rect.width)
        {
            mario_y = rect.y - mario_h; //Stay on top
            mario_speed_y = 0; //Stop falling
            jumping = false;
            return;
        }
        //Ensuring Mario doesn't jump through platforms from the bottom (Has buffers here too)
        if (mario_speed_y < 0 &&
            (mario_y <= rect.y + rect.height + 5) &&
            (mario_y >= rect.y + rect.height - 5) &&
            (mario_x + mario_w >= rect.x) &&
            (mario_x <= rect.x + rect.width))
        {
            mario_y = rect.y + rect.height; //Push Mario down so his head is just below the platform and so he falls backdown
            mario_speed_y = 0;
            jumping = false;
            return;
        }
    }
};

//Platform collisions function
void PlatformCollisions(float &mario_x, float &mario_y, float mario_w, float mario_h, 
                        float &mario_speed_x, float &mario_speed_y, bool &jumping, 
                        Platform platforms[], int platform_count) {
    for (int i = 0; i < platform_count; i++) {
        platforms[i].HandleCollision(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping);
    }
}

//Enemy projectile class
class Enemy {
public:
    Rectangle hitbox;
    int health;
    float attackCoolDown;
    double nextAttackTime;
    bool isBoss; //Indicates if the enemy is the boss.
    Texture2D* texture;
    int bossShotsRemaining;
    float bossAttackCoolDown;
    float bossMoveSpeed;
    float bossDirection;
    float draw_width;
    float draw_height;
    float bossAttackSpeed;

    //Constructor for enemy creation
    Enemy(float x, float y, double initialAttackTime, Texture2D *text, 
          float width = 10.0f, float height = 20.0f, float draw_w = 10.0f, 
          float draw_h = 20.0f, int hp = 1, bool boss = false)
    {
        //initializing values
        health = hp;
        attackCoolDown = 4.0f;
        nextAttackTime = initialAttackTime;
        isBoss = boss;
        texture = text;
        draw_width = draw_w;
        draw_height = draw_h;
        hitbox = { x, y, width, height };
        
        //Boss specific movement and attack functionality implementation
        if(isBoss) {
            bossShotsRemaining = 10;
            bossAttackCoolDown = 3.0f;
            bossMoveSpeed = 5.0f;
            bossDirection = 1.0f;
            bossAttackSpeed = 13.0f;
        }
    }

    //Checks whether enough time has passed before another attack can be initiated by the enemies
    bool Attacking() {
        double currentTime = GetTime();
        return (currentTime >= nextAttackTime);
    }

    //Creates an enemy attack based on the attack class 
    //And determines attack speed based on the type of enemy
    Attack FireAttack(Vector2 marioPos, float attackSpeed, float attackRadius) {
        double currentTime = GetTime();
        Vector2 startPos = { hitbox.x + draw_width / 2.0f, hitbox.y + draw_height / 2.0f };
        float projectileSpeed = isBoss ? bossAttackSpeed : attackSpeed;
        Attack enemyAttack(startPos, marioPos, projectileSpeed, attackRadius);
        //Decreases the amount of shots the boss has and implements a long cooldown after 10 shots have been fired
        if(isBoss) {
            if (bossShotsRemaining > 0) {
                bossShotsRemaining--;
                if (bossShotsRemaining == 0) {
                    nextAttackTime = currentTime + bossAttackCoolDown;
                    bossShotsRemaining = 10;
                } else {
                    nextAttackTime = currentTime + 0.2; //Short Cooldown in between shots 
                }
            }
        } else {
            nextAttackTime += attackCoolDown; //Gives nonboss enemies the regular cooldown in between shots
        }
        return enemyAttack; //Return the attack
    }

    //Updating the horizontal position of the boss enemy
    void updateBossPos() {
        if(isBoss) {
            hitbox.x += bossMoveSpeed * bossDirection;
            //Bounce off the left edge of screen
            if (hitbox.x <= 0) { hitbox.x = 0; bossDirection = 1.0f; }
            // Bounce off the right edge
            if (hitbox.x + hitbox.width >= 1280) { hitbox.x = 1280 - hitbox.width; bossDirection = -1.0f; }
        }
    }

    //Draws enemy based on the texture inputted
    void Draw() {
        if(texture != nullptr) {
            Rectangle source = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
            Rectangle dest = { hitbox.x, hitbox.y, draw_width, draw_height };
            DrawTexturePro(*texture, source, dest, {0,0}, 0.0f, WHITE);
        }
    }
};

//Checks if eenmy attack collides with Mario
bool CheckAttackCollision(const Attack &enemyAttack, const Rectangle &rect) {
    return CheckCollisionCircleRec(enemyAttack.position, enemyAttack.radius, rect);
}

//Mario variables 
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
float mario_x = 100;
float mario_y = 550;
float mario_speed_x = 5;
float mario_speed_y = 0;
float mario_w = 50;
float mario_h = 50;
int marioHealth = 3;
float gravity = 0.5f;
float jumpForce = -11.0f;
Rectangle marioRect = { mario_x, mario_y, mario_w, mario_h };

//Level info and level exit information
int Level = 1;
Vector2 levelExitPos = { 200, 190 };
float levelExitRadius = 20;
Vector2 level2ExitPos = { 150, 570 };
float level2ExitRadius = 20;

//Platform arrays for each level
Platform platformsLevel1[] = {
    Platform(0, 650, 1280),
    Platform(0, 340, 400),
    Platform(550, 570, 100),
    Platform(750, 500, 100),
    Platform(450, 430, 120),
    Platform(1000, 200, 100)
};
int platformCountLevel1 = sizeof(platformsLevel1)/sizeof(platformsLevel1[0]);

Platform platformsLevel2[] = {
    Platform(0, 200, 1000),
    Platform(950, 300, 100),
    Platform(600, 400, 300),
    Platform(200, 500, 180),
    Platform(100, 600, 100),
    Platform(650, 700, 100),
    Platform(950, 700, 100),
    Platform(100, 350, 100)
};
int platformCountLevel2 = sizeof(platformsLevel2)/sizeof(platformsLevel2[0]);

Platform platformsLevel3[] = { Platform(0, 650, 1280) };
int platformCountLevel3 = sizeof(platformsLevel3)/sizeof(platformsLevel3[0]);

// Vectors for attacks, enemy projectiles, and enemies
vector<Attack> Attacks;
float AttackSpeed = 10.0f;
float AttackRadius = 5.0f;
float AttackCoolDown = 1.5f;
double lastAttackTime = 0.0f;
vector<Attack> enemyProjectiles;
float enemyAttackSpeed = 7.0f;
float enemyAttackRadius = 5.0f;
vector<Enemy> enemies;

//Initializes all the sprite animations and the first enemies
void initializeGame() {
    SetTargetFPS(60);
    Mario    = LoadTexture("Resources/Mario.png");
    goomba   = LoadTexture("Resources/goomba.png");
    koopa    = LoadTexture("Resources/koopa.png");
    Bowser   = LoadTexture("Resources/Bowser.png");
    moving   = LoadTexture("Resources/moving.png");
    moving_r = LoadTexture("Resources/moving_r.png");

    _idle  = CreateSpriteAnimation(moving, 2, (Rectangle[]){
                    {0, 301, 100, 133},
                 }, 1);
    _animation  = CreateSpriteAnimation(moving, 4, (Rectangle[]){
                    {100, 302, 126, 133},
                    {226, 302, 94, 133},
                    {320, 310, 112, 133},
                 }, 3);
    _animation1 = CreateSpriteAnimation(moving_r, 4, (Rectangle[]){
                    {1820, 300, 123, 133},
                    {1729, 300, 93, 132},
                    {1618, 307, 109, 132},
                 }, 3);
    _jump = CreateSpriteAnimation(moving, 2, (Rectangle[]){
                    {668, 302, 110, 130},
                 }, 1);
    
    mario_x = 100; mario_y = 550; mario_speed_x = 5; mario_speed_y = 0;
    marioHealth = 3; Level = 1; gameOver = false; winner = false;
    Attacks.clear(); enemyProjectiles.clear(); enemies.clear();
    lastAttackTime = 0.0f;

    // Create initial enemies for Level 1
    double baseTime = GetTime();
    enemies.push_back(Enemy(1000, 145, baseTime + 1.0, &goomba, 50.0f, 50.0f, 70.0f, 70.0f));
    enemies.push_back(Enemy(460, 375, baseTime + 1.5, &goomba, 50.0f, 50.0f, 70.0f, 70.0f));
    enemies.push_back(Enemy(750, 445, baseTime + 2.5, &goomba, 50.0f, 50.0f, 70.0f, 70.0f));
}

//Mario movement, collisions, attacks and health
void updateMario() {
    // Gravity for Mario
    mario_speed_y += gravity;
    mario_y += mario_speed_y;
    
    //Horizontal movement for Mario 
    if (IsKeyDown(KEY_RIGHT))
        mario_x += mario_speed_x;
    if (IsKeyDown(KEY_LEFT))
        mario_x -= mario_speed_x;
    
    //Keeping Mario within screen bounds
    if (mario_x < 0) mario_x = 0;
    if (mario_x + mario_w > SCREEN_WIDTH) mario_x = SCREEN_WIDTH - mario_w;
    
    //Mario hitbox
    marioRect.x = mario_x; marioRect.y = mario_y;

    // Platform collisions based on current level
    if (Level == 1)
        PlatformCollisions(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping, platformsLevel1, platformCountLevel1);
    else if (Level == 2)
        PlatformCollisions(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping, platformsLevel2, platformCountLevel2);
    else if (Level == 3)
        PlatformCollisions(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping, platformsLevel3, platformCountLevel3);
    
    //Mario jumping
    if (IsKeyPressed(KEY_UP) && mario_speed_y == 0) { mario_speed_y = jumpForce; jumping = true; }
    double currentTime = GetTime();
    
    //Mario's attack
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastAttackTime >= AttackCoolDown)) {
        Vector2 startPos = { mario_x + mario_w + 1, mario_y };
        Vector2 targetPos = GetMousePosition();
        Attacks.push_back(Attack(startPos, targetPos, AttackSpeed, AttackRadius));
        lastAttackTime = currentTime;
    }
     //Updates attack movements
    for (size_t i = 0; i < Attacks.size(); i++)
        Attacks[i].Update();
}

//Checks collisions between Mario's attacks and enemy hitboxes
void updateAttackCollisions() {
    for (int i = 0; i < (int)Attacks.size(); ) {
        bool attackRemoved = false;
        for (int j = 0; j < (int)enemies.size(); j++) {
            if (CheckAttackCollision(Attacks[i], enemies[j].hitbox)) {
                enemies[j].health -= 1;
                Attacks.erase(Attacks.begin() + i);
                attackRemoved = true;
                break;
            }
        }
        if (!attackRemoved)
            i++;
    }
}

//Function for enemies attacks, hitboxes, rendering and health
void updateEnemies() {
    //Used to track Mario
    Vector2 marioCenter = { mario_x + mario_w / 2.0f, mario_y + mario_h / 2.0f };
    //Updates the attacks by enemies
    for (size_t i = 0; i < enemies.size(); i++) {
        if (enemies[i].Attacking())
            enemyProjectiles.push_back(enemies[i].FireAttack(marioCenter, enemyAttackSpeed, enemyAttackRadius));
    }
    
    for (size_t i = 0; i < enemyProjectiles.size(); i++)
        enemyProjectiles[i].Update();
    //Checks if attacks hit Mario
    for (int i = 0; i < (int)enemyProjectiles.size(); ) {
        if (CheckCollisionCircleRec(enemyProjectiles[i].position, enemyProjectiles[i].radius, marioRect)) {
            marioHealth -= 1;
            enemyProjectiles.erase(enemyProjectiles.begin() + i);
        } else { i++; }
    }

    //If mario falls off the map he loses all health
    if (mario_y >= 900)
        marioHealth = 0;

    //Removes enemies that have 0 health
    for (int i = 0; i < (int)enemies.size(); ) {
        if (enemies[i].health <= 0)
            enemies.erase(enemies.begin() + i);
        else i++;
    }
    //Updates Bowser's position
    for (size_t i = 0; i < enemies.size(); i++)
        enemies[i].updateBossPos();
}

//Combines all the functions for Mario, the enemies and Collisiosn and implements it in the game
void updateGame() {
    updateMario();
    updateEnemies();
    updateAttackCollisions();
    //Level transitions
    if (Level == 1 && CheckCollisionCircleRec(levelExitPos, levelExitRadius, marioRect)) {
        Level = 2;
        Attacks.clear();
        enemyProjectiles.clear();
        enemies.clear();
        mario_x = 200; mario_y = 110;
        double currentTime = GetTime();
        Enemy newEnemy1(645, 639, currentTime + 2.5, &koopa, 70.0f, 70.0f, 90.0f, 90.0f, 2);
        newEnemy1.attackCoolDown = 3.0f;
        enemies.push_back(newEnemy1);
        Enemy newEnemy2(945, 639, currentTime + 3.5, &koopa, 70.0f, 70.0f, 90.0f, 90.0f, 2);
        newEnemy2.attackCoolDown = 4.0f;
        enemies.push_back(newEnemy2);
        Enemy newEnemy3(95, 289, currentTime + 5.0, &koopa, 70.0f, 70.0f, 90.0f, 90.0f, 2);
        newEnemy3.attackCoolDown = 5.0f;
        enemies.push_back(newEnemy3);
    }
    if (Level == 2 && CheckCollisionCircleRec(level2ExitPos, level2ExitRadius, marioRect)) {
        Level = 3;
        Attacks.clear();
        enemyProjectiles.clear();
        enemies.clear();
        mario_x = 100; mario_y = 500;
        enemies.push_back(Enemy((SCREEN_WIDTH - 200) / 2.0f, 50, GetTime(), &Bowser, 160.0f, 160.0f, 160.0f, 160.0f, 5, true));
    }
    if (marioHealth <= 0)
        gameOver = true;
    if (Level == 3 && enemies.empty())
        winner = true;
}

//This function draws Mario, platforms, attacks, enemies, and game state messages like health or game over etc
void drawEverything() {
    BeginDrawing();
        //Game ending screens
        if (gameOver) {
            ClearBackground(BLACK);
            int fontSize = 40;
            int textWidth = MeasureText("Game Over", fontSize);
            DrawText("Game Over", SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2 - fontSize/2, fontSize, WHITE);
            DrawText("Press ENTER to return to Menu", SCREEN_WIDTH/2 - MeasureText("Press ENTER to return to Menu", 20)/2, SCREEN_HEIGHT/2 + fontSize, 20, YELLOW);
        } 
        else if (winner) {
            ClearBackground(WHITE);
            int fontSize = 40;
            int textWidth = MeasureText("You Won", fontSize);
            DrawText("You Won", SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2 - fontSize/2, fontSize, BLACK);
            DrawText("Press ENTER to return to Menu", SCREEN_WIDTH/2 - MeasureText("Press ENTER to return to Menu", 20)/2, SCREEN_HEIGHT/2 + fontSize, 20, DARKBLUE);
        }
        else {
            ClearBackground(GRAY);
            Rectangle dest = { mario_x, mario_y, mario_w, mario_h };
            if (jumping)
                DrawSpriteAnimationPro(_jump, dest, {0,0}, 0, WHITE);
            else if (IsKeyDown(KEY_RIGHT))
                DrawSpriteAnimationPro(_animation, dest, {0,0}, 0, WHITE);
            else if (IsKeyDown(KEY_LEFT))
                DrawSpriteAnimationPro(_animation1, dest, {0,0}, 0, WHITE);
            else
                DrawSpriteAnimationPro(_idle, dest, {0,0}, 0, WHITE);

            //Draws platforms and exit circles based on platforms
            if (Level == 1) {
                for (int i = 0; i < platformCountLevel1; i++)
                    platformsLevel1[i].Draw();
                DrawCircleV(levelExitPos, levelExitRadius, PURPLE);
            } else if (Level == 2) {
                for (int i = 0; i < platformCountLevel2; i++)
                    platformsLevel2[i].Draw();
                DrawCircleV(level2ExitPos, level2ExitRadius, ORANGE);
            } else if (Level == 3) {
                for (int i = 0; i < platformCountLevel3; i++)
                    platformsLevel3[i].Draw();
            }

             //Draws the health bar in the top right corner
            DrawText("Health:", 1110, 10, 20, BLACK);
            for (int i = 0; i < 3; i++) {
                if (i < marioHealth)
                    DrawRectangle(SCREEN_WIDTH - 90 + i * 25, 10, 20, 20, GREEN);
                else
                    DrawRectangle(SCREEN_WIDTH - 90 + i * 25, 10, 20, 20, DARKGRAY);
            }
            //Draws Mario's attacks
            for (size_t i = 0; i < Attacks.size(); i++)
                Attacks[i].Draw();
            //Draws enemie's attacks
            for (size_t i = 0; i < enemyProjectiles.size(); i++)
                enemyProjectiles[i].Draw();
            //Draws enemies and their health on top of their head
            for (size_t i = 0; i < enemies.size(); i++){
                enemies[i].Draw();
                DrawText(TextFormat("%d", enemies[i].health), enemies[i].hitbox.x, enemies[i].hitbox.y - 20, 20, RED);
            }
        }
    EndDrawing();
}

//Where the game runs
void Run() {
    initializeGame();
    while (!WindowShouldClose()) {
        updateGame();
        drawEverything();
        if ((gameOver || winner) && IsKeyPressed(KEY_ENTER))
            break;
    }
    UnloadTexture(Mario);
    UnloadTexture(goomba);
    UnloadTexture(koopa);
    UnloadTexture(Bowser);
    UnloadTexture(moving);
    UnloadTexture(moving_r);
    DisposeSpriteAnimation(_idle);
    DisposeSpriteAnimation(_animation);
    DisposeSpriteAnimation(_animation1);
    DisposeSpriteAnimation(_jump);
}
} // end namespace MarioGame


// Snake Game 
namespace SnakeGame {
    const int screenWidth = 640;
    const int screenHeight = 480;
    const int gridSize = 20;
    const int gridWidth = screenWidth / gridSize;
    const int gridHeight = screenHeight / gridSize;
    
    struct Coord { int x, y; };
    enum Direction { UP, DOWN, LEFT, RIGHT };
    
    void Run() {
        SetTargetFPS(10);
        // Load images
        Image foodImage = LoadImage("AppleSprite.png");
        ImageResize(&foodImage, gridSize, gridSize);
        Texture2D foodSprite = LoadTextureFromImage(foodImage);
        UnloadImage(foodImage);
        Image bgImage = LoadImage("SnakeBackground.png");
        ImageResize(&bgImage, screenWidth, screenHeight);
        Texture2D background = LoadTextureFromImage(bgImage);
        UnloadImage(bgImage);

        // Create initial snake
        vector<Coord> snake;
        snake.push_back({ gridWidth / 2, gridHeight / 2 });
        snake.push_back({ gridWidth / 2 - 1, gridHeight / 2 });
        snake.push_back({ gridWidth / 2 - 2, gridHeight / 2 });
        Direction snakeDir = RIGHT;
        // initialize food and barrier
        Coord food = { GetRandomValue(0, gridWidth - 1), GetRandomValue(0, gridHeight - 1) };
        vector<Coord> barriers;
        bool gameOver = false;
        int score = 0;
        while (!WindowShouldClose()) {
            // Input
            if (IsKeyPressed(KEY_RIGHT) && snakeDir != LEFT) snakeDir = RIGHT;
            else if (IsKeyPressed(KEY_LEFT) && snakeDir != RIGHT) snakeDir = LEFT;
            else if (IsKeyPressed(KEY_UP) && snakeDir != DOWN) snakeDir = UP;
            else if (IsKeyPressed(KEY_DOWN) && snakeDir != UP) snakeDir = DOWN;
            if (!gameOver) {
                // Grid
                DrawLine(screenWidth, screenHeight, screenWidth, 0, BLACK);
                DrawLine(0, screenHeight, screenWidth, screenHeight, BLACK);
                Coord newHead = snake.front();
                switch (snakeDir) {
                    // Movement of snake
                    case RIGHT: newHead.x++; break;
                    case LEFT: newHead.x--; break;
                    case UP: newHead.y--; break;
                    case DOWN: newHead.y++; break;
                }
                // Collision between snake and wall/barrier/food
                if ((newHead.x < 0) || (newHead.y < 0) || (newHead.x >= gridWidth) || (newHead.y >= gridHeight))
                    gameOver = true;
                for (const auto& segment : snake)
                    if (segment.x == newHead.x && segment.y == newHead.y) { 
                        gameOver = true; 
                        break; }
                for (const auto& barrier : barriers)
                    if (barrier.x == newHead.x && barrier.y == newHead.y) { 
                        gameOver = true; 
                        break; }
                if (!gameOver) {
                    snake.insert(snake.begin(), newHead);
                    if (newHead.x == food.x && newHead.y == food.y) {
                        score++;
                        // Generating food
                        bool validFoodPosition = false;
                        while (!validFoodPosition) {
                            food = { GetRandomValue(0, gridWidth - 1), GetRandomValue(0, gridHeight - 1) };
                            validFoodPosition = true;
                            for (const auto& segment : snake)
                                if (segment.x == food.x && segment.y == food.y) { 
                                    validFoodPosition = false; 
                                    break; }
                            for (const auto& barrier : barriers)
                                if (barrier.x == food.x && barrier.y == food.y) { 
                                    validFoodPosition = false; 
                                    break; }
                        }
                        // Generate barrier
                        bool validBarrierPosition = false;
                        Coord newBarrier = { 0, 0 };
                        while (!validBarrierPosition) {
                            newBarrier = { GetRandomValue(0, gridWidth - 1), GetRandomValue(0, gridHeight - 1) };
                            validBarrierPosition = true;
                            for (const auto& segment : snake)
                                if (segment.x == newBarrier.x && segment.y == newBarrier.y) { validBarrierPosition = false; break; }
                            if (newBarrier.x == food.x && newBarrier.y == food.y)
                                validBarrierPosition = false;
                        }
                        barriers.clear();
                        barriers.push_back(newBarrier);
                    } else {
                        // If snake does not eat any food, remove the tail
                        snake.pop_back();
                    }
                }
            }
            BeginDrawing();
                ClearBackground(WHITE);
                DrawTexture(background, 0, 0, WHITE);
                if (gameOver) {
                    DrawText("GAME OVER", screenWidth/2 - MeasureText("GAME OVER",20)/2, screenHeight/2 - 10, 20, RED);
                    DrawText("Press ENTER to return to Menu", 100, screenHeight - 40, 20, YELLOW);
                }
                // Draw sprites
                DrawTexture(foodSprite, food.x * gridSize, food.y * gridSize, WHITE);
                for (const auto& barrier : barriers)
                    DrawRectangle(barrier.x * gridSize, barrier.y * gridSize, gridSize - 1, gridSize - 1, BLACK);
                for (const auto& segment : snake)
                    DrawRectangle(segment.x * gridSize, segment.y * gridSize, gridSize, gridSize, DARKGREEN);
                DrawText(TextFormat("Score: %i", score), 10, 10, 20, BLACK);
            EndDrawing();
            if (gameOver && IsKeyPressed(KEY_ENTER)) break;
        }
        UnloadTexture(foodSprite);
        UnloadTexture(background);
        SetTargetFPS(60);
    }
} // end SnakeGame

// Main Menu and Combined Game Loop
int main() {
    const int windowWidth = 1280;
    const int windowHeight = 720;
    InitWindow(windowWidth, windowHeight, "Combined Games");
    SetTargetFPS(60);
    
    GameState currentState = MENU;
    
    while (!WindowShouldClose()) {
        if (currentState == MENU) {
            BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Main Menu", windowWidth/2 - MeasureText("Main Menu",50)/2, 100, 50, WHITE);
                DrawText("Press I for Space Invaders", windowWidth/2 - 200, 150, 30, WHITE);
                DrawText("Press M for Tic Tac Toe", windowWidth/2 - 200, 200, 30, WHITE);
                DrawText("Press N for Mario Platform", windowWidth/2 - 200, 250, 30, WHITE);
                DrawText("Press B for Snake", windowWidth/2 - 200, 300, 30, WHITE);
            EndDrawing();

            // Check key presses to change game state
            if (IsKeyPressed(KEY_I)) { 
                currentState = SPACEINVADERS; 
            }
            else if (IsKeyPressed(KEY_M)) { 
                currentState = TIC_TAC_TOE; 
            }
            else if (IsKeyPressed(KEY_N)) { 
                currentState = MARIO; 
            }
            else if (IsKeyPressed(KEY_B)) { 
                currentState = SNAKE; 
            }
        }
        // Game State Checks
        else if (currentState == SPACEINVADERS) {
            SpaceInvadersGame::Run();
            currentState = MENU;
        }
        else if (currentState == MARIO) {
            MarioGame::Run();
            currentState = MENU;
        }
        else if (currentState == SNAKE) {
            SnakeGame::Run();
            currentState = MENU;
        }
        else if (currentState == TIC_TAC_TOE) {
            TicTacToeGame::Run();
            currentState = MENU;
        }
    }

    // Clean up and close the window
    CloseWindow();
    return 0;
}
