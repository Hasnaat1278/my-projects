#include "game.hpp"
#include <iostream>
#include <fstream>   // For file handling (e.g., saving/loading high score)


// Constructor
Game::Game() {
    level = 1;
    run = false;
    ShowStartScreen();  // Show the start screen before initializing the game
}

void Game::ShowStartScreen() {
    // Display the start screen until the user presses ENTER
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);  // Set background to black

        // Draw the "Press ENTER to start" message in the center of the screen
        DrawText("Press ENTER to start", 
                 GetScreenWidth() / 2 - MeasureText("Press ENTER to start", 40) / 2, 
                 GetScreenHeight() / 2,  // Centered vertically
                 40, RAYWHITE);

        EndDrawing();

        // Wait for the user to press ENTER to start the game
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();  // Initialize the game state
            run = true;  // Start the game
            break;       // Exit the start screen and proceed with the game
        }
    }
}

// Destructor
Game::~Game() {
    Alien::UnloadImages();  // Clean up alien images to free memory
}

// Update function: Handles game logic
void Game::Update() {
    if (run) {  // If the game is running
        // Update all spaceship lasers
        for (auto& laser : spaceship.lasers) {
            laser.Update();
        }

        MoveAliens();        // Move aliens
        AlienShootLaser();   // Make aliens shoot lasers
        // Update all alien lasers
        for (auto& laser : alienLasers) {
            laser.Update();
        }

        DeleteInactiveLasers();  // Remove inactive lasers
        CheckForCollisions();    // Check for collisions

    } else {  // If the game is not running
        while (!WindowShouldClose()) {  // Stay in this loop until window is closed
            BeginDrawing();
            
            ClearBackground(BLACK);  // Clear the entire screen with a black background
    
            // Draw "Press ENTER to restart" in the center of the screen
            DrawText("Press ENTER to restart", 
                     GetScreenWidth() / 2 - MeasureText("Press ENTER to restart", 20) / 2, 
                     GetScreenHeight() / 2 - 10, 
                     20, 
                     RAYWHITE);
    
            EndDrawing();
    
            // Check for ENTER key to restart and break the loop
            if (IsKeyPressed(KEY_ENTER)) {
                Reset();    // Reset the game state
                InitGame(); // Reinitialize the game
                run = true; // Start the game again
                break;      // Exit the loop and go back to the game
            }
        }
    }
}

// Draw function: Renders all game objects
void Game::Draw() {
    spaceship.Draw();  // Draw the spaceship
    // Draw all spaceship lasers
    for (auto& laser : spaceship.lasers) {
        laser.Draw();
    }
    // Draw all aliens
    for (auto& alien : aliens) {
        alien.Draw();
    }
    // Draw all alien lasers
    for (auto& laser : alienLasers) {
        laser.Draw();
    }
}

// HandleInput function: Processes player input
void Game::HandleInput() {
    if (run) {  // If the game is running
        if (IsKeyDown(KEY_LEFT)) {  // Move spaceship left
            spaceship.MoveLeft();
        } else if (IsKeyDown(KEY_RIGHT)) {  // Move spaceship right
            spaceship.MoveRight();
        } else if (IsKeyDown(KEY_SPACE)) {  // Fire spaceship laser
            spaceship.FireLaser();
        }
    }
}

// DeleteInactiveLasers function: Removes inactive lasers
void Game::DeleteInactiveLasers() {
    // Remove inactive spaceship lasers
    for (auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();) {
        if (!it->active) {
            it = spaceship.lasers.erase(it);  // Remove laser if inactive
        } else {
            ++it;  // Move to next laser
        }
    }
    // Remove inactive alien lasers
    for (auto it = alienLasers.begin(); it != alienLasers.end();) {
        if (!it->active) {
            it = alienLasers.erase(it);  // Remove laser if inactive
        } else {
            ++it;  // Move to next laser
        }
    }
}

// CreateAliens function: Creates a grid of aliens
std::vector<Alien> Game::CreateAliens() {
    std::vector<Alien> aliens;
    // Create 5 rows of aliens
    for (int row = 0; row < 5; row++) {
        // Create 11 columns of aliens
        for (int column = 0; column < 11; column++) {
            int alienType;  // Determine alien type based on row
            if (row == 0) {
                alienType = 3;  // Top row: type 3
            } else if (row == 1 || row == 2) {
                alienType = 2;  // Middle rows: type 2
            } else {
                alienType = 1;  // Bottom rows: type 1
            }
            // Calculate alien position
            float x = 75 + column * 55;
            float y = 110 + row * 55;
            aliens.push_back(Alien(alienType, {x, y}));  // Add alien
        }
    }
    return aliens;
}

void Game::MoveAliens() {
    int speed = (level == 1) ? 1 : 2;  // Level 1: speed 1, Level 2: speed 2
    for (auto& alien : aliens) {
        if (alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25) {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if (alien.position.x < 25) {
            aliensDirection = 1;
            MoveDownAliens(4);
        }
        alien.Update(aliensDirection * speed);  // Apply speed multiplier
    }
}

// MoveDownAliens function: Moves aliens down by a specified distance
void Game::MoveDownAliens(int distance) {
    for (auto& alien : aliens) {
        alien.position.y += distance;  // Move aliens down
    }
}

// AlienShootLaser function: Makes aliens shoot lasers
void Game::AlienShootLaser() {
    double currentTime = GetTime();  // Get current time
    // Check if enough time has passed since the last alien laser
    if (currentTime - timeLastAlienFired >= alienLaserShootInterval && !aliens.empty()) {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);  // Pick a random alien
        Alien& alien = aliens[randomIndex];
        // Create a laser at the alien's position
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type - 1].width / 2,
                                    alien.position.y + alien.alienImages[alien.type - 1].height}, 6));
        timeLastAlienFired = GetTime();  // Update last laser fire time
    }
}

// CheckForCollisions function: Handles all collisions
void Game::CheckForCollisions() {
    // Check collisions between spaceship lasers and aliens
    for (auto& laser : spaceship.lasers) {
        auto it = aliens.begin();
        while (it != aliens.end()) {
            if (CheckCollisionRecs(it->getRect(), laser.getRect())) {  // If collision
                // Update score based on alien type
                if (it->type == 1) {
                    score += 100;
                } else if (it->type == 2) {
                    score += 200;
                } else if (it->type == 3) {
                    score += 300;
                }
                checkForHighscore();  // Check if new high score
                it = aliens.erase(it);  // Remove alien
                laser.active = false;   // Deactivate laser
            } else {
                ++it;  // Move to next alien
            }
        }

        // Check if all aliens are defeated in Level 2
        if (aliens.empty() && level == 2) {
            ShowWinScreen();  // Display win screen for Level 2
        }
    }

    // Check collisions between alien lasers and spaceship
    for (auto& laser : alienLasers) {
        if (CheckCollisionRecs(laser.getRect(), spaceship.getRect())) {  // If collision
            laser.active = false;  // Deactivate laser
            lives--;               // Reduce lives
            if (lives == 0) {     // If no lives left, game over
                GameOver();
            }
        }
    }
}

// GameOver function: Ends the game
void Game::GameOver() {
    run = false;  // Stop the game
}

// InitGame function: Initializes the game state
void Game::InitGame() {
    aliensDirection = 1;            // Set initial alien direction
    timeLastAlienFired = 0.0;       // Reset alien laser timer
    timeLastSpawn = 0.0;            // Reset mystery ship timer
    lives = 3;                      // Set lives to 3
    score = 0;                      // Reset score
    highscore = loadHighscoreFromFile();  // Load high score
    mysteryShipSpawnInterval = GetRandomValue(10, 20);  // Set mystery ship spawn interval

    // Create aliens based on the level
    if (level == 1 || level == 2) {
        aliens = CreateAliens();        // Create aliens for both levels
    }

    run = true;  // Start the game
}

// checkForHighscore function: Updates high score if current score is higher
void Game::checkForHighscore() {
    if (score > highscore) {
        highscore = score;               // Update high score
        saveHighscoreToFile(highscore);  // Save high score to file
    }
}

// saveHighscoreToFile function: Saves high score to a file
void Game::saveHighscoreToFile(int highscore) {
    std::ofstream highscoreFile("highscore.txt");  // Open file for writing
    if (highscoreFile.is_open()) {
        highscoreFile << highscore;  // Write high score
        highscoreFile.close();       // Close file
    } else {
        std::cerr << "Failed to save highscore to file." << std::endl;
    }
}

// loadHighscoreFromFile function: Loads high score from a file
int Game::loadHighscoreFromFile() {
    int loadedHighscore = 0;
    std::ifstream highscoreFile("highscore.txt");  // Open file for reading
    if (highscoreFile.is_open()) {
        highscoreFile >> loadedHighscore;  // Read high score
        highscoreFile.close();             // Close file
    } else {
        std::cerr << "Failed to load highscore from file." << std::endl;
    }
    return loadedHighscore;
}

// Reset function: Resets the game state
void Game::Reset() {
    spaceship.Reset();  // Reset spaceship
    aliens.clear();     // Clear aliens
    alienLasers.clear(); // Clear alien lasers
    score = 0;          // Reset score
    lives = 3;          // Reset lives
    level = 1;          // Reset to level 1
    InitGame();         // Reinitialize game state
}

// ShowWinScreen function: Displays the win screen
void Game::ShowWinScreen() {


    // Set a flag or condition to indicate the game is won (if needed)
    run = false;  // Stop the game from running

    // Display a win screen
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);  // Set background to black

        // Draw the win message in the center of the screen
        DrawText("You Won!", GetScreenWidth() / 2 - MeasureText("You Won!", 40) / 2, GetScreenHeight() / 3, 40, GREEN);

        // Draw the "Press ENTER to restart" message
        DrawText("Press ENTER to restart", 
                 GetScreenWidth() / 2 - MeasureText("Press ENTER to restart", 20) / 2, 
                 GetScreenHeight() / 2 + 50,  // Slightly below the win message
                 20, RAYWHITE);

        EndDrawing();

        // Wait for the user to press ENTER to restart the game
        if (IsKeyPressed(KEY_ENTER)) {
            Reset();    // Reset the game state
            InitGame(); // Reinitialize the game
            run = true; // Start the game again
            break;      // Exit the loop and go back to the game
        }
    }
}