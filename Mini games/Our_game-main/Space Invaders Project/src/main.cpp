#include <raylib.h>
#include "game.hpp"
#include <string>

// Helper function: Formats a number with leading zeros
std::string FormatWithLeadingZeros(int number, int width) {
    std::string numberText = std::to_string(number);  // Convert number to string
    int leadingZeros = width - numberText.length();   // Calculate number of leading zeros needed
    return numberText = std::string(leadingZeros, '0') + numberText;  // Add leading zeros
}

// Main function: Entry point of the program
int main() {
    // Define colors
    Color grey = {29, 29, 27, 255};    // Dark grey colour
    Color blue = {60, 230, 247, 255}; // blue colour

    // Define window dimensions and offset
    int offset = 50;                    // Offset for UI elements
    int windowWidth = 750;               // Width of the game area
    int windowHeight = 700;              // Height of the game area

    // Initialize the window
    InitWindow(windowWidth + offset, windowHeight + 2 * offset, "Space Invaders");
    InitAudioDevice();  // Initialize the audio device

    // Load assets
    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);  // Load custom font
    Texture2D spaceshipImage = LoadTexture("Graphics/spaceship.png");  // Load spaceship texture

    SetTargetFPS(60);  // Set the target frame rate to 60 FPS

    Game game;  // Create a Game object

    // Main game loop
    while (WindowShouldClose() == false) {
        game.HandleInput();
        game.Update();
    
        BeginDrawing();
            ClearBackground(grey);
            DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2, blue);
            DrawLineEx({25, 730}, {775, 730}, 3, blue);
    
            if (game.run) {
                // Display current level dynamically
                std::string levelText = "LEVEL " + std::to_string(game.level);  // Use game.level
                if (game.level < 10) {
                    levelText.insert(levelText.end() - 1, '0');  // Add leading zero for single-digit levels
                }
                DrawTextEx(font, levelText.c_str(), {570, 740}, 34, 2, blue);
            } else {
                DrawTextEx(font, "GAME OVER", {570, 740}, 34, 2, blue);
            }

        // Draw lives (spaceship icons)
        float x = 50.0;
        for (int i = 0; i < game.lives; i++) {
            DrawTextureV(spaceshipImage, {x, 745}, WHITE);  // Draw spaceship icon
            x += 50;  // Move to the next position
        }

        // Draw score
        DrawTextEx(font, "SCORE", {50, 15}, 34, 2, blue);  // Draw "SCORE" text
        std::string scoreText = FormatWithLeadingZeros(game.score, 5);  // Format score with leading zeros
        DrawTextEx(font, scoreText.c_str(), {50, 40}, 34, 2, blue);  // Draw formatted score

        // Draw high score
        DrawTextEx(font, "HIGH-SCORE", {570, 15}, 34, 2, blue);  // Draw "HIGH-SCORE" text
        std::string highscoreText = FormatWithLeadingZeros(game.highscore, 5);  // Format high score with leading zeros
        DrawTextEx(font, highscoreText.c_str(), {655, 40}, 34, 2, blue);  // Draw formatted high score

        game.Draw();  // Draw the game objects (aliens, lasers, etc.)
        EndDrawing(); // End drawing
    }

        game.HandleInput();
        game.Update();
    
        BeginDrawing();
            ClearBackground(grey);
            DrawRectangleRoundedLinesEx({10, 10, 780, 780}, 0.18f, 20, 2, blue);
            DrawLineEx({25, 730}, {775, 730}, 3, blue);
    
            if (game.run) {
                // Display current level
                std::string levelText = "LEVEL " + std::to_string(game.level);
                DrawTextEx(font, levelText.c_str(), {570, 740}, 34, 2, blue);
            } else {
                DrawTextEx(font, "GAME OVER", {570, 740}, 34, 2, blue);
            }
    
            // Draw lives, score, high score, etc.
            game.Draw();
        EndDrawing();

    // Clean up
    CloseWindow();      // Close the window
}