#version 330 core
out vec4 fragment_color;
uniform vec3 COLOR;
void main() {
    fragment_color = vec4(COLOR, 1.0);
}