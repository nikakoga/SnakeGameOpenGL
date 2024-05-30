#version 330 core
layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 v_texCoord; // Nowy atrybut dla tekstury
out vec2 texCoord; // Nowy atrybut dla tekstury
flat out int isSnake; // Poprawnie umieszczony kwalifikator interpolacji

uniform float POSITION;
uniform vec2 GRID_COUNT;
uniform bool IS_SNAKE; // Nowy parametr przekazywany do shadera

void main() {
    vec2 GRID_SIZE = vec2(2.0 / GRID_COUNT.x, 2.0 / GRID_COUNT.y);
    vec2 pos_center = vec2((mod(POSITION, GRID_COUNT.x) * GRID_SIZE.x) - 1.0 + (GRID_SIZE.x) / 2.0,
                           1.0 - GRID_SIZE.y - (floor(POSITION / GRID_COUNT.x) * GRID_SIZE.y) + (GRID_SIZE.y / 2.0));
    gl_Position = vec4(
        pos_center.x - (GRID_SIZE.x / 2.0) + (v_pos.x * GRID_SIZE.x),
        pos_center.y - (GRID_SIZE.y / 2.0) + (v_pos.y * GRID_SIZE.y),
        0.0, 1.0
    );

    texCoord = v_texCoord; // Przekazujemy wspolrzêdne tekstury
    isSnake = int(IS_SNAKE); // Konwertujemy bool na int i przekazujemy
}
