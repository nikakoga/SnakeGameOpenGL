#include "Snake.h"
#include <cstdlib>
#include <iostream>

Snake::Snake(GLuint gridX, GLuint gridY) : gridX(gridX), gridY(gridY), foodPosition(generateRandomPosition()) {
    
    srand(time(NULL));

    counter = 1;
    positions = new GLuint[gridX * gridY];
    positions[0] = (gridX * gridY / 2);
    direction = PAUSE;
}

Snake::~Snake() {
    delete[] positions;
}

GLuint Snake::generateRandomPosition() {
    GLuint RND = (GLuint)(rand() % (gridX * gridY));
    GLboolean free_position;
    do {
        free_position = GL_TRUE;
        for (GLuint i = 0; i < counter; i++) {
            if (positions[i] == RND) {
                free_position = GL_FALSE;
                RND++;
                if (RND >= (gridX * gridY))
                    RND %= (gridX * gridY);
            }
        }
    } while (!free_position);
    return RND;
}

bool Snake::checkWallHit()
{
    if ( //czy moja glowa znajduje sie na koncu siatki i probuje sie ruszyc w tym kierunku?
        (positions[0] % gridX == 0 && direction == LEFT) ||
        (positions[0] % gridX == gridX - 1 && direction == RIGHT) ||
        (positions[0] / gridX == 0 && direction == UP) ||
        (positions[0] / gridX == gridY - 1 && direction == DOWN)
        )
    {

        return true;
    }

    return false;
}

bool doesGameJustStart = true;

void Snake::snakeLogic() {

    if (direction & PAUSE) {
        return;
    }

    if (!doesGameJustStart)
    {
        if (checkWallHit())
        {
            reset();
            return;
        }
    }


    GLuint lastSnakeHeadPosition = positions[0];
    GLuint lastSnakeLength = counter;

    switch (direction) {
    case LEFT:
        positions[0]--;
        break;
    case RIGHT:
        positions[0]++;
        break;
    case UP:
        positions[0] -= gridX;
        break;
    case DOWN:
        positions[0] += gridX;
        break;
    }

    if (positions[0] == foodPosition)
        counter += 1;

    if (counter >= gridX * gridY)
        counter = 1;

    if (counter > 2)
        for (GLuint i = 0; i < counter - 2; i++) {
            positions[counter - 1 - i] = positions[counter - 1 - i - 1];
        }
    if (counter > 1)
        positions[1] = lastSnakeHeadPosition;

    for (GLuint i = 0; i < counter - 1; i++) {
        if (positions[0] == positions[i + 1])
            counter = 1;
    }

    if (lastSnakeLength != counter)
        foodPosition = generateRandomPosition();

    doesGameJustStart = false;
}

void Snake::update() {
    snakeLogic();
}

void Snake::setDirection(GLuint newDirection) {
    direction = newDirection;
}

GLuint Snake::getDirection() {
    return direction;
}

GLuint* Snake::getPositions() {
    return positions;
}

GLuint Snake::getCounter() {
    return counter;
}

GLuint Snake::getFoodPosition() {
    return foodPosition;
}

void Snake::reset() {
    counter = 1;
    positions[0] = GLuint(gridX * gridY / 2);
    direction = PAUSE;
    foodPosition = generateRandomPosition();
    doesGameJustStart = true;
}
