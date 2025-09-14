#include "raylib.h"
#include <vector>

// Screen settings
const int screenWidth = 640;
const int screenHeight = 480;
const int gridSize = 20;               
const int gridWidth = screenWidth / gridSize;
const int gridHeight = screenHeight / gridSize;

// Structure to hold grid coordinates
struct Coord {
    int x, y;
};

// Movement directions for the snake
enum Direction { UP, DOWN, LEFT, RIGHT };

int main()
{
    // Initialization: create the window and set FPS
    InitWindow(screenWidth, screenHeight, "Snake Game - Raylib C++");
    SetTargetFPS(10);  

    // Load and resize the food sprite to fit a grid cell
    Image foodImage = LoadImage("AppleSprite.png");
    ImageResize(&foodImage, gridSize, gridSize); // Resize image
    Texture2D foodSprite = LoadTextureFromImage(foodImage);
    UnloadImage(foodImage); // Free the image 

    // Resize background
    Image bgImage = LoadImage("SnakeBackground.png");
    ImageResize(&bgImage, screenWidth, screenHeight); // Resize image
    Texture2D background = LoadTextureFromImage(bgImage);
    UnloadImage(bgImage); // Free the image 

    // Initialize snake
    std::vector<Coord> snake;
    {
        snake.push_back({ gridWidth / 2, gridHeight / 2 });
        snake.push_back({ gridWidth / 2 - 1, gridHeight / 2 });
        snake.push_back({ gridWidth / 2 - 2, gridHeight / 2 });
    }

    Direction snakeDir = RIGHT;  // Initial direction

    // Generate the first food position 
    Coord food = { GetRandomValue(0, gridWidth - 1), GetRandomValue(0, gridHeight - 1) };

    // Barrier storage
    std::vector<Coord> barriers;

    bool gameOver = false;
    int score = 0;

    // Main game loop
    while (!WindowShouldClose())
    {
        // Input/movement
        if (IsKeyPressed(KEY_RIGHT) && snakeDir != LEFT)
        {
            snakeDir = RIGHT;
        }
        else if (IsKeyPressed(KEY_LEFT) && snakeDir != RIGHT)
        {
            snakeDir = LEFT;
        }
        else if (IsKeyPressed(KEY_UP) && snakeDir != DOWN)
        {
            snakeDir = UP;
        }
        else if (IsKeyPressed(KEY_DOWN) && snakeDir != UP)
        {
            snakeDir = DOWN;
        }

        if (!gameOver)
        {
            // New head position from the current head
            Coord newHead = snake.front();
            switch (snakeDir)
            {
                case RIGHT:
                    newHead.x++;
                    break;
                case LEFT:
                    newHead.x--;
                    break;
                case UP:
                    newHead.y--;
                    break;
                case DOWN:
                    newHead.y++;
                    break;
            }

            // Check collision with the walls
            if ((newHead.x < 0) || (newHead.y < 0) || (newHead.x >= gridWidth) || (newHead.y >= gridHeight))
            {
                gameOver = true;
            }

            // Check collision with the snake body
            for (const auto& segment : snake)
            {
                if ((segment.x == newHead.x) && (segment.y == newHead.y))
                {
                    gameOver = true;
                    break;
                }
            }

            // Check collision with barriers
            for (const auto& barrier : barriers)
            {
                if ((barrier.x == newHead.x) && (barrier.y == newHead.y))
                {
                    gameOver = true;
                    break;
                }
            }

            if (!gameOver)
            {
                // Add the new head to the snake body
                snake.insert(snake.begin(), newHead);

                // Check if the snake has eaten the food
                if ((newHead.x == food.x) && (newHead.y == food.y))
                {
                    score++;

                    // Generate new food at a valid position
                    bool validFoodPosition = false;
                    while (!validFoodPosition)
                    {
                        food = { GetRandomValue(0, gridWidth - 1), GetRandomValue(0, gridHeight - 1) };
                        validFoodPosition = true;

                        // Ensure the food does not appear on the snake
                        for (const auto& segment : snake)
                        {
                            if ((segment.x == food.x) && (segment.y == food.y))
                            {
                                validFoodPosition = false;
                                break;
                            }
                        }

                        // Ensure the food does not appear on an existing barrier
                        for (const auto& barrier : barriers)
                        {
                            if ((barrier.x == food.x) && (barrier.y == food.y))
                            {
                                validFoodPosition = false;
                                break;
                            }
                        }
                    }

                    // Generate a new barrier at a valid position.
                    bool validBarrierPosition = false;
                    Coord newBarrier = { 0, 0 };
                    while (!validBarrierPosition)
                    {
                        newBarrier = { GetRandomValue(0, gridWidth - 1), GetRandomValue(0, gridHeight - 1) };
                        validBarrierPosition = true;

                        // Ensure the barrier does not appear on the snake
                        for (const auto& segment : snake)
                        {
                            if ((segment.x == newBarrier.x) && (segment.y == newBarrier.y))
                            {
                                validBarrierPosition = false;
                                break;
                            }
                        }

                        // Ensure the barrier does not appear in the same cell as the food
                        if ((newBarrier.x == food.x) && (newBarrier.y == food.y))
                        {
                            validBarrierPosition = false;
                        }
                    }
                    
                    // Replace any existing barrier with the new one,
                    barriers.clear();
                    barriers.push_back(newBarrier);
                }
                else
                {
                    // If no food is eaten, remove the tail of the snake
                    snake.pop_back();
                }
            }
        }

        // Drawing
        BeginDrawing();
        {
            ClearBackground(WHITE);

            // Draw the background image
            DrawTexture(background, 0, 0, WHITE);

            if (gameOver)
            {
                DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 20) / 2, screenHeight / 2 - 10, 20, RED);
            }

            // Draw the food sprite
            DrawTexture(foodSprite, food.x * gridSize, food.y * gridSize, WHITE);

            // Draw the barrier (only one barrier exists)
            for (const auto& barrier : barriers)
            {
                DrawRectangle(barrier.x * gridSize, barrier.y * gridSize, gridSize - 1, gridSize - 1, BLACK);
            }

            // Draw the snake as a series of green squares
            for (const auto& segment : snake)
            {
                DrawRectangle(segment.x * gridSize, segment.y * gridSize, gridSize, gridSize, DARKGREEN);
            }

            // Display the score in the upper-left corner
            DrawText(TextFormat("Score: %i", score), 10, 10, 20, BLACK);
        }
        EndDrawing();
    }

    // Cleanup
    UnloadTexture(foodSprite);
    UnloadTexture(background);
    CloseWindow();
    return 0;
}