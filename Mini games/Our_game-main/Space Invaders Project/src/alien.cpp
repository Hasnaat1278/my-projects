#include "alien.hpp"

// Static member initialization: Array to store alien textures
Texture2D Alien::alienImages[3] = {};

// Constructor: Creates an Alien object
Alien::Alien(int type, Vector2 position) {
    this->type = type;        // Set the alien type (1, 2, or 3)
    this->position = position; // Set the alien's position

    // Load the alien texture
    if (alienImages[type - 1].id == 0) {  // Check if texture is not loaded
        switch (type) {
            case 1:
                alienImages[0] = LoadTexture("Graphics/alien_1.png");  // Load type 1 alien texture
                break;
            case 2:
                alienImages[1] = LoadTexture("Graphics/alien_2.png");  // Load type 2 alien texture
                break;
            case 3:
                alienImages[2] = LoadTexture("Graphics/alien_3.png");  // Load type 3 alien texture
                break;
            default:
                alienImages[0] = LoadTexture("Graphics/alien_1.png");  // Default to type 1 alien texture
                break;
        }
    }
}

// Draw function: Renders the alien on the screen
void Alien::Draw() {
    DrawTextureV(alienImages[type - 1], position, WHITE);  // Draw the alien texture at its position
}

// GetType function: Returns the alien's type
int Alien::GetType() {
    return type;  // Return the alien type (1, 2, or 3)
}

// UnloadImages function: Unloads all alien textures from memory
void Alien::UnloadImages() {
    for (int i = 0; i < 3; i++) {  // Loop through all alien textures
        UnloadTexture(alienImages[i]);  // Unload the texture
    }
}

// getRect function: Returns the alien's collision rectangle
Rectangle Alien::getRect() {
    return {
        position.x,  // X position of the alien
        position.y,  // Y position of the alien
        float(alienImages[type - 1].width),   // Width of the alien texture
        float(alienImages[type - 1].height)  // Height of the alien texture
    };
}

// Update function: Moves the alien horizontally
void Alien::Update(int direction) {
    position.x += direction;  // Move the alien left or right based on the direction
}