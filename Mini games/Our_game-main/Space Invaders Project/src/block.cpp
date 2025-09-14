#include "block.hpp" 

// Constructor: Creates a Block object
Block::Block(Vector2 position) {
    this->position = position;  // Set the block's position
}

// Draw function: Renders the block on the screen
void Block::Draw() {
    // Draw a small rectangle  at the block's position
    DrawRectangle(position.x, position.y, 3, 3, {243, 216, 63, 255});
}

// getRect function: Returns the block's collision rectangle
Rectangle Block::getRect() {
    Rectangle rect;
    rect.x = position.x;  // X position of the block
    rect.y = position.y;  // Y position of the block
    rect.width = 3;       // Width of the block
    rect.height = 3;      // Height of the block
    return rect;          // Return the collision rectangle
}