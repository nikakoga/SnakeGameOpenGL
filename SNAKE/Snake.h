#ifndef SNAKE_H
#define SNAKE_H

#include <GLFW/glfw3.h>

enum Directions {
    LEFT = 1,
    RIGHT = 2,
    UP = 4,
    DOWN = 8,
    PAUSE = 16
};

class Snake {
public:
    Snake(GLuint gridX, GLuint gridY);
    ~Snake();
    void update();
    void setDirection(GLuint direction);
    GLuint getDirection();
    GLuint* getPositions();
    GLuint getCounter();
    GLuint getFoodPosition();
    void reset();
    GLuint generateRandomPosition();
    bool checkWallHit();
    bool checkTailBite();

private:
    GLuint counter;
    GLuint* positions;
    GLuint direction;
    GLuint gridX;
    GLuint gridY;
    GLuint foodPosition;

    void snakeLogic();
};

#endif // SNAKE_H