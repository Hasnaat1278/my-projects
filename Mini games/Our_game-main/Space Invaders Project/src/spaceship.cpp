#include "spaceship.hpp" 

// Constructor: Creates a Spaceship object
Spaceship::Spaceship() {
    image = LoadTexture("Graphics/spaceship.png");  // Load the spaceship texture
    position.x = (GetScreenWidth() - image.width) / 2;  // Center the spaceship horizontally
    position.y = GetScreenHeight() - image.height - 100;  // Position the spaceship near the bottom
    lastFireTime = 0.0;  // Initialize the last fire time
    laserSound = LoadSound("Sounds/laser.ogg");  // Load the laser sound
}

// Destructor: Cleans up resources
Spaceship::~Spaceship() {
    UnloadTexture(image);    // Unload the spaceship texture
    UnloadSound(laserSound); // Unload the laser sound
}

// Draw function: Renders the spaceship on the screen
void Spaceship::Draw() {
    DrawTextureV(image, position, WHITE);  // Draw the spaceship texture at its position
}

// MoveLeft function: Moves the spaceship left
void Spaceship::MoveLeft() {
    position.x -= 7;  // Move the spaceship left by 7 units
    if (position.x < 25) {  // Prevent the spaceship from moving off the left edge
        position.x = 25;
    }
}

// MoveRight function: Moves the spaceship right
void Spaceship::MoveRight() {
    position.x += 7;  // Move the spaceship right by 7 units
    if (position.x > GetScreenWidth() - image.width - 25) {  // Prevent the spaceship from moving off the right edge
        position.x = GetScreenWidth() - image.width - 25;
    }
}

// FireLaser function: Fires a laser from the spaceship
void Spaceship::FireLaser() {
    if (GetTime() - lastFireTime >= 0.35) {  // Check if enough time has passed since the last laser was fired
        lasers.push_back(Laser({position.x + image.width / 2 - 2, position.y}, -6));  // Create a new laser
        lastFireTime = GetTime();  // Update the last fire time
        PlaySound(laserSound);     // Play the laser sound
    }
}

// getRect function: Returns the spaceship's collision rectangle
Rectangle Spaceship::getRect() {
    return {position.x, position.y, float(image.width), float(image.height)};  // Return the spaceship's bounding box
}

// Reset function: Resets the spaceship to its initial position and clears lasers
void Spaceship::Reset() {
    position.x = (GetScreenWidth() - image.width) / 2.0f;  // Center the spaceship horizontally
    position.y = GetScreenHeight() - image.height - 100;   // Position the spaceship near the bottom
    lasers.clear();  // Clear all lasers
}