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
    //************Tutaj umieszczaj kod, kt�ry nale�y wykona� raz, na pocz�tku programu************
    glClearColor(0, 1, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Wyczy�� bufor koloru i g��boko�ci
    //glfwSetWindowSizeCallback(window, windowResizeCallback);
    //glfwSetKeyCallback(window, keyCallback);
}

void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, kt�ry nale�y wykona� po zako�czeniu p�tli g��wnej************
}

int main(int argc, char const* argv[]) {

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW!\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "SNAKE", NULL, NULL);

    if (!window) { // Je�eli okna nie uda�o si� utworzy�, to zamknij program
        fprintf(stderr, "Nie mo�na utworzy� okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); // Od tego momentu kontekst okna staje si� aktywny i polecenia OpenGL b�d� dotyczy� w�a�nie jego.
    glfwSwapInterval(1); // Czekaj na 1 powr�t plamki przed pokazaniem ukrytego bufora

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


    while (!glfwWindowShouldClose(window)) { // Tak d�ugo jak okno nie powinno zosta� zamkni�te

        initOpenGLProgram(window); // Operacje inicjuj�ce

        glBindVertexArray(VAO);
        glUseProgram(shader_programm);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //drawScene(window, angle_x, angle_y); // Wykonaj procedur� rysuj�c�

        glfwSwapBuffers(window); // Zamie� bufory
        glfwPollEvents(); // Wykonaj procedury callback w zale�no�ci od zdarze� jakie zasz�y
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window); // Usu� kontekst OpenGL i okno
    glfwTerminate(); // Zwolnij zasoby zaj�te przez GLFW
    exit(EXIT_SUCCESS);

    //#ifdef __APPLE__
    //    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //#endif
}
