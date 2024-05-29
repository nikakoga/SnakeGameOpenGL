#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShaderProgram.h" 

#define GRID_COUNT_X 8
#define GRID_COUNT_Y 8
#define SPEED 8

//#define COLOR_SNAKE_TAIL (GLfloat[3]){0.6, 0.88, 0.6}
//#define COLOR_SNAKE_HEAD (GLfloat[3]){0.5,1.0,0.5}
//#define COLOR_FOOD (GLfloat[3]){1.0,0.5,0.5}
//#define COLOR_BACKGROUND (GLfloat[3]){0.14,0.16,0.18}

GLfloat color_tail[] = { 0.6, 0.88, 0.6 };

GLfloat color_head[] = { 0.5,1.0,0.5 };

GLfloat color_food [] ={ 1.0, 0.5, 0.5 };

GLfloat color_background[] = { 0.14,0.16,0.18 };

GLfloat verticles[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
};

GLuint indices[] = {
    0, 1, 2,
    2, 3, 0
};

struct Button {
    GLboolean down, last, pressed;
};
struct Keyboard {
    struct Button keys[GLFW_KEY_LAST];
};
struct Keyboard keyboard = {};

enum Directions {
    LEFT = 1,
    RIGHT = 2,
    UP = 4,
    DOWN = 8,
    PAUSE = 16
};

struct Snake {
    GLuint counter;
    GLuint positions[GRID_COUNT_X * GRID_COUNT_Y];
    GLuint direction;
};

struct Snake snake = {};
GLuint foodPosition = 0;

GLdouble TIME_NOW, TIME_LAST, TIME_DELTA, TIME_SUM;

GLuint generateRandomPosition(void)
{
    GLuint RND = (GLuint)(rand() % (GRID_COUNT_X * GRID_COUNT_Y));
    GLboolean free_position;
    do {
        free_position = GL_TRUE;
        for (GLuint i = 0; i < snake.counter; i++)
        {
            if (snake.positions[i] == RND)
            {
                free_position = GL_FALSE;
                RND++;
                if (RND >= (GRID_COUNT_X * GRID_COUNT_Y))
                    RND %= (GRID_COUNT_X * GRID_COUNT_Y);
            }
        }
    } while (!free_position);
    return RND;
}

void snakeLogic(void)
{
    // if pause do nothing
    if (snake.direction & PAUSE)
    {
        return;
    }

    /* check wall hit */
    if (
        (snake.positions[0] % GRID_COUNT_X == 0
            && snake.direction == LEFT) ||
        (snake.positions[0] % GRID_COUNT_X == GRID_COUNT_X - 1
            && snake.direction == RIGHT) ||
        (snake.positions[0] / GRID_COUNT_X == 0
            && snake.direction == UP) ||
        (snake.positions[0] / GRID_COUNT_X == GRID_COUNT_Y - 1
            && snake.direction == DOWN)
        )
    {
        snake.counter = 1;
        snake.direction = PAUSE;
        return;
    }

    
    GLuint lastSnakeHeadPosition = snake.positions[0];
    GLuint lastSnakeLength = snake.counter;
    

    //update snake head position
    if (snake.direction == LEFT)
        snake.positions[0] -= 1;
    else if (snake.direction == RIGHT)
        snake.positions[0] += 1;
    else if (snake.direction == UP)
        snake.positions[0] -= GRID_COUNT_X;
    else if (snake.direction == DOWN)
        snake.positions[0] += GRID_COUNT_X;

    /* FOOOOOD eat check */
    if (snake.positions[0] == foodPosition)
        snake.counter += 1;

    /* check if game won */
    if (snake.counter >= GRID_COUNT_X * GRID_COUNT_Y)
        snake.counter = 1;

    /* update snake_tail_positions */
    if (snake.counter > 2)
        for (GLuint i = 0; i < snake.counter - 2; i++)
        {
            snake.positions[snake.counter - 1 - i] =
            snake.positions[snake.counter - 1 - i - 1];
        }
    if (snake.counter > 1)
        snake.positions[1] = lastSnakeHeadPosition;

    /* check for self destruction */
    for (GLuint i = 0; i < snake.counter - 1; i++)
    {
        if (snake.positions[0] == snake.positions[i + 1])
            snake.counter = 1;
    }


    /* spawn new food if it was eaten before */
    if (lastSnakeLength != snake.counter)
        foodPosition = generateRandomPosition();

}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    if (key < 0) {
        return;
    }

    switch (action) {
    case GLFW_PRESS:
        keyboard.keys[key].down = GL_TRUE;
        break;
    case GLFW_RELEASE:
        keyboard.keys[key].down = GL_FALSE;
        break;
    default:
        break;
    }
}

void _button_array_update(struct Button* buttons) {
    for (int i = 0; i < GLFW_KEY_LAST; i++) {
        buttons[i].pressed = buttons[i].down && !buttons[i].last;
        buttons[i].last = buttons[i].down;
    }
}

void keyFunctions(GLFWwindow* window) {

    _button_array_update(keyboard.keys);
    GLuint _last_snake_direction = snake.direction;

    if (keyboard.keys[GLFW_KEY_ESCAPE].down)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (keyboard.keys[GLFW_KEY_SPACE].pressed)
        snake.direction |= PAUSE;

    if (keyboard.keys[GLFW_KEY_LEFT].pressed)
        if ((snake.direction != LEFT) && !(snake.direction & RIGHT))
            snake.direction = LEFT;

    if (keyboard.keys[GLFW_KEY_RIGHT].pressed)
        if (!(snake.direction & LEFT) && (snake.direction != RIGHT))
            snake.direction = RIGHT;

    if (keyboard.keys[GLFW_KEY_UP].pressed)
        if ((snake.direction != UP) && !(snake.direction & DOWN))
            snake.direction = UP;

    if (keyboard.keys[GLFW_KEY_DOWN].pressed)
        if (!(snake.direction & UP) && (snake.direction != DOWN))
            snake.direction = DOWN;

    if (snake.direction != _last_snake_direction)
    {
        snakeLogic();
        TIME_SUM = 0.0;
    }

}

void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który nale¿y wykonaæ po zakoñczeniu pêtli g³ównej************
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);

int main(int argc, char const* argv[]) {
    srand(time(NULL));
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW!\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "SNAKE", NULL, NULL);

    if (!window) { // Je¿eli okna nie uda³o siê utworzyæ, to zamknij program
        fprintf(stderr, "Nie mo¿na utworzyæ okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); // Od tego momentu kontekst okna staje siê aktywny i polecenia OpenGL bêd¹ dotyczyæ w³aœnie jego.
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1); // Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

    // Inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize GLAD!\n");
        return -1;
    }

    GLuint shader_programm = createShaderProgram("vertexShader.glsl", "fragmentShader.glsl");

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticles), verticles, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    snake.counter = 1;
    snake.positions[0] = (GLuint)(rand() % (GRID_COUNT_X * GRID_COUNT_Y));
    snake.direction = PAUSE;

    foodPosition = generateRandomPosition();

    TIME_NOW = TIME_LAST = glfwGetTime();
    TIME_DELTA = TIME_SUM = 0.0;

    while (!glfwWindowShouldClose(window)) { // Tak d³ugo jak okno nie powinno zostaæ zamkniête

        glClearColor(
            color_background[0],
            color_background[1],
            color_background[2],
            1.0);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Wyczyœæ bufor koloru i g³êbokoœci

        keyFunctions(window);

        TIME_NOW = glfwGetTime();
        TIME_DELTA = TIME_NOW - TIME_LAST;
        TIME_LAST = TIME_NOW;
        TIME_SUM += TIME_DELTA;

        if (TIME_SUM >= 1.0/SPEED)
        {
            snakeLogic();
            TIME_SUM = 0.0;
        }

        glUseProgram(shader_programm);
        glBindVertexArray(VAO);
        glUniform2f(glGetUniformLocation(shader_programm, "GRID_COUNT"), 
            (GLfloat)GRID_COUNT_X, 
            (GLfloat)GRID_COUNT_Y);

        //draw Snake
        for (GLuint i = 0; i < snake.counter; i++)
        {
            glUniform1f(
                glGetUniformLocation(shader_programm, "POSITION"), 
                snake.positions[i]);

            if (i == 0)
            {
                glUniform3f(
                    glGetUniformLocation(shader_programm, "COLOR"),
                    color_head[0],
                    color_head[1],
                    color_head[2]);
            }
            else
            {
                glUniform3f(
                    glGetUniformLocation(shader_programm, "COLOR"),
                    color_tail[0],
                    color_tail[1],
                    color_tail[2]);
            }
            
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        //draw Food
        glUniform1f(glGetUniformLocation(shader_programm, "POSITION"), 
            foodPosition);

        glUniform3f(
            glGetUniformLocation(shader_programm, "COLOR"),
            color_food[0],
            color_food[1],
            color_food[2]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        glfwPollEvents(); // Wykonaj procedury callback w zale¿noœci od zdarzeñ jakie zasz³y
        glfwSwapBuffers(window); // Zamieñ bufory
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window); // Usuñ kontekst OpenGL i okno
    glfwTerminate(); // Zwolnij zasoby zajête przez GLFW
    exit(EXIT_SUCCESS);
}
