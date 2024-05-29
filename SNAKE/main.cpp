#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


GLfloat verticles[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
};

GLuint indices[] = {
    0, 1, 2,
    3, 0, 1
};

const GLchar* vShader_Code =
"#version 330 core\n"
"layout (location =0) in vec2 v_pos;\n"
"void main(){\n"
"gl_Position = vec4(v_pos, 0.0 1.0);\n"
"}\0";

const GLchar* fShader_Code =
"#version 330 core\n"
"out vec4 fragment_color;\n"
"void main(){\n"
"fragment_color = vec4(1.0,0.5,0.5,1.0);\n"
"}\0";

//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    if (action == GLFW_PRESS) {
//        //if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
//        //if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
//        //if (key == GLFW_KEY_UP) speed_y = PI / 2;
//        //if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
//    }
//    if (action == GLFW_RELEASE) {
//        //if (key == GLFW_KEY_LEFT) speed_x = 0;
//        //if (key == GLFW_KEY_RIGHT) speed_x = 0;
//        //if (key == GLFW_KEY_UP) speed_y = 0;
//        //if (key == GLFW_KEY_DOWN) speed_y = 0;
//    }
//}

void initOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
    glClearColor(0, 1, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Wyczyœæ bufor koloru i g³êbokoœci
    //glfwSetWindowSizeCallback(window, windowResizeCallback);
    //glfwSetKeyCallback(window, keyCallback);
}

void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który nale¿y wykonaæ po zakoñczeniu pêtli g³ównej************
}

int main(int argc, char const* argv[]) {

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
    glfwSwapInterval(1); // Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

    // Inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize GLAD!\n");
        return -1;
    }

    GLuint vShader, fShader, shader_programm;

    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShader_Code, NULL);
    glCompileShader(vShader);

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShader_Code, NULL);
    glCompileShader(fShader);

    shader_programm = glCreateProgram();
    glAttachShader(shader_programm, vShader);
    glAttachShader(shader_programm, fShader);

    glLinkProgram(shader_programm);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

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


    while (!glfwWindowShouldClose(window)) { // Tak d³ugo jak okno nie powinno zostaæ zamkniête

        initOpenGLProgram(window); // Operacje inicjuj¹ce

        glBindVertexArray(VAO);
        glUseProgram(shader_programm);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //drawScene(window, angle_x, angle_y); // Wykonaj procedurê rysuj¹c¹

        glfwSwapBuffers(window); // Zamieñ bufory
        glfwPollEvents(); // Wykonaj procedury callback w zale¿noœci od zdarzeñ jakie zasz³y
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window); // Usuñ kontekst OpenGL i okno
    glfwTerminate(); // Zwolnij zasoby zajête przez GLFW
    exit(EXIT_SUCCESS);

    //#ifdef __APPLE__
    //    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //#endif
}
