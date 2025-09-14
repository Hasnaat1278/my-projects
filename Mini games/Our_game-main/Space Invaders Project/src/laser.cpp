#include "laser.hpp"
#include <iostream>

// Constructor: Creates a Laser object
Laser::Laser(Vector2 position, int speed) {
    this->position = position;  // Set the laser's position
    this->speed = speed;        // Set the laser's speed
    active = true;              // Set the laser to active
}

// Draw function: Renders the laser on the screen
void Laser::Draw() {
    if (active)  // Only draw if the laser is active
        // Draw a rectangle representing the laser
        DrawRectangle(position.x, position.y, 4, 15, {243, 216, 63, 255});
        // The color {243, 216, 63, 255} is a shade of yellow
}

// getRect function: Returns the laser's collision rectangle
Rectangle Laser::getRect() {
    Rectangle rect;
    rect.x = position.x;  // X position of the laser
    rect.y = position.y;  // Y position of the laser
    rect.width = 4;       // Width of the laser 
    rect.height = 15;     // Height of the laser 
    return rect;          // Return the collision rectangle
}

// Update function: Moves the laser and checks if it goes off-screen
void Laser::Update() {
    position.y += speed;  // Move the laser vertically based on its speed
    if (active) {
        // Deactivate the laser if it goes off-screen
        if (position.y > GetScreenHeight() - 100 || position.y < 25) {
            active = false;
        }
    }
}