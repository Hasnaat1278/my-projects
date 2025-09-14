#include <iostream>
#include <raylib.h>
#include <stdlib.h>
#include <cmath>
#include <array>
#include <vector>

using namespace std;

//Texture initialization and global variables
Texture2D Mario;
Texture2D Bowser;
Texture2D goomba;
Texture2D koopa;
int framesPerSecond;
bool jumping = false;
bool gameOver = false;
bool winner = false;

//SpriteAnimation structure and function to store the textures, store the animation speed and used to draw the sprite animations
typedef struct{
    Texture2D atlas;
    int framesPerSecond;

    Rectangle *rectangles;
    int rectanglesLength;
} SpriteAnimation;

SpriteAnimation CreateSpriteAnimation(Texture2D atlas, int framesPerSecond, Rectangle rectangles[], int length);
void DisposeSpriteAnimation(SpriteAnimation animation);

void DrawSpriteAnimationPro (SpriteAnimation animation, Rectangle dest, Vector2 origin, float rotation, Color tint);

//Animation intilization
Texture2D moving;
SpriteAnimation _animation;
Texture2D moving_r;
SpriteAnimation _animation1;
SpriteAnimation _jump;
SpriteAnimation _idle;

//Mario's attacks 
class Attack{
    public: 
    //Initializing data members
    Vector2 position;
    Vector2 Velocity;
    float radius = 0;

    //Calculating velocity towards the target (target is where my mouse clicks)
    Attack(Vector2 startPos, Vector2 targetPos, float speed, float r){
        position = startPos;
        radius = r;
        //Finding the magnitude of the line using the pythagorean theorem
        Vector2 direction = { targetPos.x - startPos.x, targetPos.y - startPos.y };
        float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
        //Normalizing the vector so that we get just the direction
        direction.x /= magnitude;
        direction.y /= magnitude;
        Velocity = { direction.x * speed, direction.y * speed };
    }

    void Update() {
        position.x += Velocity.x;
        position.y += Velocity.y;
    }

    void Draw() {
        DrawCircleV(position, radius, RED);
    }
};

//Platform rendering
class Platform {
    public:
        Rectangle rect;
        
        //Platform will have a constant thickness of one
        Platform(float x, float y, float w) {
            rect = {x, y, w, 1};  
        }

        //  Function to draw only a line
        void Draw() {
            DrawLineEx((Vector2){rect.x, rect.y}, (Vector2){rect.x + rect.width, rect.y}, 2, BLACK);
        }

        //Landing on top of the platforms
        void HandleCollision(float &mario_x, float &mario_y, float mario_w, float mario_h, float &mario_speed_x, float &mario_speed_y, bool &jumping) {

            if (mario_y + mario_h >= rect.y && mario_y + mario_h <= rect.y + 15 && mario_x + mario_w >= rect.x && mario_x <= rect.x + rect.width )  
              {
                mario_y = rect.y - mario_h;  // Stay on top
                mario_speed_y = 0;  // Stop falling
                jumping = false;
                return;
            }

            //Ensuring Mario doesn't jump through platforms from the bottom (Has buffers here too)
        if (mario_speed_y < 0 &&
            (mario_y <= rect.y + rect.height + 5) && (mario_y >= rect.y + rect.height - 5) && (mario_x + mario_w >= rect.x ) && (mario_x <= rect.x + rect.width ))             
        {
            mario_y = rect.y + rect.height;  // Push Mario down so his head is just below the platform and so he falls backdown
            mario_speed_y = 0; 
            jumping = false;
            return;              
        }
    }
};

    //Platform collisions function
    void PlatformCollisions(float &mario_x, float &mario_y, float mario_w, float mario_h, float &mario_speed_x, float &mario_speed_y, bool &jumping, Platform platforms[], int platform_count);

    void PlatformCollisions(float &mario_x, float &mario_y, float mario_w, float mario_h, float &mario_speed_x, float &mario_speed_y, bool &jumping, Platform platforms[], int platform_count) {
        for (int i = 0; i < platform_count; i++) {
            platforms[i].HandleCollision(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping);
        }
    }

//Enemy projectile class
class Enemy {
    public:
    //Initializing data members
    Rectangle hitbox;
    int health;
    float attackCoolDown;
    double nextAttackTime;
    bool isBoss;             //Indicates if the enemy is the boss.
    Texture2D* texture;
    int bossShotsRemaining;    
    float bossAttackCoolDown;  
    float bossMoveSpeed;       
    float bossDirection; 
    float draw_width;
    float draw_height;
    float bossAttackSpeed;
    
    //Constructor for enemy creation
    Enemy(float x, float y, double initialAttackTime, Texture2D *text, float width = 10.0f, float height = 20.0f, float draw_w = 10.0f, float draw_h = 20.0f, int hp = 1, bool boss = false)
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

    //Creates an enemy attack and determines attack speed based on the type of enemy
    Attack FireAttack(Vector2 marioPos, float attackSpeed, float attackRadius) {
        double currentTime = GetTime();
        Vector2 startPos = { hitbox.x + draw_width / 2.0f, hitbox.y + draw_height / 2.0f };
        float projectileSpeed;
        //Boss specific attacks
        if (isBoss) {
            projectileSpeed = bossAttackSpeed;
        } else {
            projectileSpeed = attackSpeed;
        }
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
            // Bounce off the left edge of screen
            if (hitbox.x <= 0) {
                hitbox.x = 0;
                bossDirection = 1.0f;
            }
            // Bounce off the right edge
            if (hitbox.x + hitbox.width >= 1280) {
                hitbox.x = 1280 - hitbox.width;
                bossDirection = -1.0f;
            }
        }
        
    }

    //Draws enemy based on the texture inputted
    void Draw() {
        if(texture != nullptr) {
            Rectangle source = { 0.0f, 0.0f, (float)texture->width, (float)texture->height };
            Rectangle dest = {hitbox.x, hitbox.y, draw_width, draw_height};
            DrawTexturePro(*texture, source, dest, Vector2{0, 0}, 0.0f, WHITE);
        } 
    }
    };

    //Checks if eenmy attack collides with Mario
    bool CheckAttackCollision(const Attack &enemyAttack, const Rectangle &rect);

    bool CheckAttackCollision(const Attack &enemyAttack, const Rectangle &rect) {
        return CheckCollisionCircleRec(enemyAttack.position, enemyAttack.radius, rect);
    }

    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    
    //Mario variables 
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
    
    Platform platformsLevel3[] = {
        Platform(0, 650, 1280)
    };
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
    
    //Function prototypes
    void initializeGame();   
    void updateMario();         
    void updateEnemies();      
    void updateAttackCollisions(); 
    void updateGame();         
    void drawEverything();      
    

    //Where the game runs
    int main() {
        initializeGame();
        while (!WindowShouldClose()){
            updateGame();
            drawEverything();
        }
        CloseWindow();
        return 0;
    }
    
    //Initializes all the sprite animations and the first enemies
    void initializeGame() {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My first RAYLIB program!");
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
        
        // Create initial enemies for Level 1
        enemies.push_back(Enemy(1000, 145, 3.0, &goomba, 50.0f, 50.0f, 70.0f, 70.0f));
        enemies.push_back(Enemy(460, 375, 5.5, &goomba, 50.0f, 50.0f, 70.0f, 70.0f));
        enemies.push_back(Enemy(750, 445, 7.5, &goomba, 50.0f, 50.0f, 70.0f, 70.0f));
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
        if (mario_x < 0)
            mario_x = 0;
        if (mario_x + mario_w > SCREEN_WIDTH)
            mario_x = SCREEN_WIDTH - mario_w;
        
        //Mario hitbox
        marioRect.x = mario_x;
        marioRect.y = mario_y;
        
        // Platform collisions based on current level
        if (Level == 1)
            PlatformCollisions(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping, platformsLevel1, platformCountLevel1);
        else if (Level == 2)
            PlatformCollisions(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping, platformsLevel2, platformCountLevel2);
        else if (Level == 3)
            PlatformCollisions(mario_x, mario_y, mario_w, mario_h, mario_speed_x, mario_speed_y, jumping, platformsLevel3, platformCountLevel3);
        
        //Mario jumping
        if (IsKeyPressed(KEY_UP) && mario_speed_y == 0) {
            mario_speed_y = jumpForce;
            jumping = true;
        }
        
        //Mario's attack
        double currentTime = GetTime();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (currentTime - lastAttackTime >= AttackCoolDown)) {
            Vector2 startPos = { mario_x + mario_w + 1, mario_y };
            Vector2 targetPos = GetMousePosition();
            Attacks.push_back(Attack(startPos, targetPos, AttackSpeed, AttackRadius));
            lastAttackTime = currentTime;
        }
        //Updates attack movements
        for (size_t i = 0; i < Attacks.size(); i++) {
            Attacks[i].Update();
        }
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
        for (size_t i = 0; i < enemyProjectiles.size(); i++) {
            enemyProjectiles[i].Update();
        }

        //Checks if attacks hit Mario
        for (int i = 0; i < (int)enemyProjectiles.size(); ) {
            if (CheckCollisionCircleRec(enemyProjectiles[i].position, enemyProjectiles[i].radius, marioRect)) {
                marioHealth -= 1;
                enemyProjectiles.erase(enemyProjectiles.begin() + i);
            } else {
                i++;
            }
        }

        //If mario falls off the map he loses all health
        if (mario_y >= 900){
            marioHealth = 0;
        }
    
        //Removes enemies that have 0 health
        for (int i = 0; i < (int)enemies.size(); ) {
            if (enemies[i].health <= 0)
                enemies.erase(enemies.begin() + i);
            else
                i++;
        }

        //Updates Bowser's position
        for (size_t i = 0; i < enemies.size(); i++) {
            enemies[i].updateBossPos();
        }
    }
    
    // Combines all the functions for Mario, the enemies and Collisiosn and implements it in the game
    void updateGame() {
        updateMario();
        updateEnemies();
        updateAttackCollisions();
        
        // Level transitions
        if (Level == 1 && CheckCollisionCircleRec(levelExitPos, levelExitRadius, marioRect)) {
            //Resets eveything and creates enemies for level 2
            Level = 2;
            Attacks.clear();
            enemyProjectiles.clear();
            enemies.clear();
            mario_x = 200;
            mario_y = 110;
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
            //Resets eveything and creates enemies for level 3
            Level = 3;
            Attacks.clear();
            enemyProjectiles.clear();
            enemies.clear();
            mario_x = 100;
            mario_y = 500;
            enemies.push_back(Enemy((SCREEN_WIDTH - 200) / 2.0f, 50, GetTime(), &Bowser, 160.0f, 160.0f, 160.0f, 160.0f, 5, true));
        }
        //Gameover screen if Mario health is 0, Game winning screen if Bowser is defeated
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
                DrawText("Game Over", SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, WHITE);
            } 
            else if (winner) {
                ClearBackground(WHITE);
                int fontSize = 40;
                int textWidth = MeasureText("You Won", fontSize);
                DrawText("You Won", SCREEN_WIDTH / 2 - textWidth / 2, SCREEN_HEIGHT / 2 - fontSize / 2, fontSize, BLACK);
            }
            else {
            ClearBackground(GRAY);
            //Draws Mario animation based on what key is pressed
            Rectangle dest = { mario_x, mario_y, mario_w, mario_h };
            if (jumping)
                DrawSpriteAnimationPro(_jump, dest, {0, 0}, 0, WHITE);
            else if (IsKeyDown(KEY_RIGHT))
                DrawSpriteAnimationPro(_animation, dest, {0, 0}, 0, WHITE);
            else if (IsKeyDown(KEY_LEFT))
                DrawSpriteAnimationPro(_animation1, dest, {0, 0}, 0, WHITE);
            else
                DrawSpriteAnimationPro(_idle, dest, {0, 0}, 0, WHITE);
    
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
            for (size_t i = 0; i < Attacks.size(); i++) {
                Attacks[i].Draw();
            }
            
            //Draws enemie's attacks
            for (size_t i = 0; i < enemyProjectiles.size(); i++) {
                enemyProjectiles[i].Draw();
            }
            
            //Draws enemies and their health on top of their head
            for (size_t i = 0; i < enemies.size(); i++) {
                enemies[i].Draw();
                DrawText(TextFormat("%d", enemies[i].health), enemies[i].hitbox.x, enemies[i].hitbox.y - 20, 20, RED);
            }
        }
        EndDrawing();
    }
    
    
    
    
///broski

//Initializes sprite animation
SpriteAnimation CreateSpriteAnimation(Texture2D atlas, int framesPerSecond, Rectangle rectangles[], int length){
    SpriteAnimation spriteAnimation = {
        .atlas = atlas,
        .framesPerSecond = framesPerSecond,
        .rectanglesLength = length
    };

    Rectangle *mem = (Rectangle*)malloc(sizeof(Rectangle) * length);
    if (mem == NULL){
        TraceLog(LOG_FATAL, "No memory for CreateSpriteAnimation");
        spriteAnimation.rectanglesLength = 0;
        return spriteAnimation;
    }

    spriteAnimation.rectangles = mem;
    for(int i = 0; i < length; i++){
            spriteAnimation.rectangles[i] = rectangles[i];
    }

    return spriteAnimation;
}

//Uses memory block for the rectangles for the sprites instead
void DisposeSpriteAnimation(SpriteAnimation animation){
    free (animation.rectangles);
}

//Renders sprite animation onto screen
void DrawSpriteAnimationPro (SpriteAnimation animation, Rectangle dest, Vector2 origin, float rotation, Color tint){
    int index = (int) (GetTime() * animation.framesPerSecond) % animation.rectanglesLength;
    Rectangle source = animation.rectangles[index];
    DrawTexturePro(animation.atlas, source, dest, origin, rotation, tint);
}
