#version 330 core
layout (location = 0) in vec2 v_pos;
uniform float POSITION;
uniform vec2 GRID_COUNT;
void main() {
    vec2 GRID_SIZE = vec2(2.0 / GRID_COUNT.x, 2.0 / GRID_COUNT.y);
    vec2 pos_center = vec2((mod(POSITION, GRID_COUNT.x) * GRID_SIZE.x) - 1.0 + (GRID_SIZE.x) / 2.0,
                           1.0 - GRID_SIZE.y - (floor(POSITION / GRID_COUNT.x) * GRID_SIZE.y) + (GRID_SIZE.y / 2.0));
    gl_Position = vec4(
        pos_center.x - (GRID_SIZE.x / 2.0) + (v_pos.x * GRID_SIZE.x),
        pos_center.y - (GRID_SIZE.y / 2.0) + (v_pos.y * GRID_SIZE.y),
        0.0, 1.0
    );
}