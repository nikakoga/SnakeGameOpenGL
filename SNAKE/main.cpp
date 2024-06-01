#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShaderProgram.h"
#include "Snake.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define GRID_COUNT_X 17
#define GRID_COUNT_Y 17
#define SPEED 6

GLfloat color_tail[] = { 0.6, 0.88, 0.6 };
GLfloat color_head[] = { 0.5, 1.0, 0.5 };
GLfloat color_food[] = { 1.0, 0.5, 0.5 };
GLfloat color_background[] = { 0.14, 0.16, 0.18 };

GLfloat verticles[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
};

GLfloat textureCoords[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
};

GLuint indices[] = {
    0, 1, 2,
    2, 3, 0
};


Snake snake(GRID_COUNT_X, GRID_COUNT_Y);

GLdouble TIME_NOW, TIME_LAST, TIME_DELTA, TIME_SUM;


void keyFunctions(GLFWwindow* window, int key, int scancode, int action, int mods) {
 
    GLuint _last_snake_direction = snake.getDirection();

    if (action == GLFW_PRESS || GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT)
        {
            if ((snake.getDirection() != LEFT) && !(snake.getDirection() & RIGHT))
                snake.setDirection(LEFT);
        }

        if (key == GLFW_KEY_RIGHT)
        {
            if (!(snake.getDirection() & LEFT) && (snake.getDirection() != RIGHT))
                snake.setDirection(RIGHT);
        }

        if (key == GLFW_KEY_UP)
        {
            if ((snake.getDirection() != UP) && !(snake.getDirection() & DOWN))
                snake.setDirection(UP);
        }

        if (key == GLFW_KEY_DOWN)
        {
            if (!(snake.getDirection() & UP) && (snake.getDirection() != DOWN))
                snake.setDirection(DOWN);
        }
    }

    if (snake.getDirection() != _last_snake_direction)
    {
        snake.update();
        TIME_SUM = 0.0;
    }
}

void freeOpenGLProgram(GLFWwindow* window) {
    // Cleanup code
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);

int main(int argc, char const* argv[]) {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW!\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(900, 900, "SNAKE", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Nie mo�na utworzy� okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyFunctions);
    glfwSwapInterval(1);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticles) + sizeof(textureCoords), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticles), verticles);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(verticles), sizeof(textureCoords), textureCoords);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)(sizeof(verticles)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    TIME_NOW = TIME_LAST = glfwGetTime();
    TIME_DELTA = TIME_SUM = 0.0;

    // Wczytywanie tekstury
    int textureWidth, textureHeight, textureChannels;
    unsigned char* textureData = stbi_load("snake_brown_square.png", &textureWidth, &textureHeight, &textureChannels, 0);
    if (!textureData) {
        std::cout << "Failed to load texture!!!" << std::endl;
        exit(EXIT_FAILURE);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(textureData);

    // Ustawianie parametrow tekstury
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(color_background[0], color_background[1], color_background[2], 1.0);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //keyFunctions(window);
        glfwSetKeyCallback(window, keyFunctions);

        TIME_NOW = glfwGetTime();
        TIME_DELTA = TIME_NOW - TIME_LAST;
        TIME_LAST = TIME_NOW;
        TIME_SUM += TIME_DELTA;

        if (TIME_SUM >= 1.0 / SPEED) {
            snake.update();
            TIME_SUM = 0.0;
        }

        glUseProgram(shader_programm);
        glBindVertexArray(VAO);
        glUniform2f(glGetUniformLocation(shader_programm, "GRID_COUNT"), (GLfloat)GRID_COUNT_X, (GLfloat)GRID_COUNT_Y);

        // Przekazywanie samplera tekstury
        glUniform1i(glGetUniformLocation(shader_programm, "textureSampler"), 0);
        glUniform1i(glGetUniformLocation(shader_programm, "IS_SNAKE"), 1);

        // Rysowanie snake
        GLuint* snakePositions = snake.getPositions();
        for (GLuint i = 0; i < snake.getCounter(); i++) {
            glUniform1f(glGetUniformLocation(shader_programm, "POSITION"), snakePositions[i]);

            if (i == 0) {
                glUniform3f(glGetUniformLocation(shader_programm, "COLOR"), color_head[0], color_head[1], color_head[2]);
            }
            else {
                glUniform3f(glGetUniformLocation(shader_programm, "COLOR"), color_tail[0], color_tail[1], color_tail[2]);
            }

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Rysowanie jedzenia
        glUniform1i(glGetUniformLocation(shader_programm, "IS_SNAKE"), 0);
        glUniform1f(glGetUniformLocation(shader_programm, "POSITION"), snake.getFoodPosition());
        glUniform3f(glGetUniformLocation(shader_programm, "COLOR"), color_food[0], color_food[1], color_food[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

