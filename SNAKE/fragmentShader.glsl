#version 330 core
out vec4 fragment_color;
uniform vec3 COLOR;
uniform sampler2D textureSampler;
in vec2 texCoord; // Przekazane wspó³rzêdne tekstury
flat in int isSnake; // Zmieniony typ danych na int

void main() {
    // Jeœli kwadrat jest czêœci¹ wê¿a (IS_SNAKE == true), u¿yj tekstury, w przeciwnym razie u¿yj koloru
    if (isSnake == 1) { // Porównujemy z 1, poniewa¿ int(IS_SNAKE) w vertex shaderze zwraca 0 lub 1
        vec4 texColor = texture(textureSampler, texCoord);
        fragment_color = vec4(texColor.rgb * COLOR, 1.0);
    } else {
        fragment_color = vec4(COLOR, 1.0);
    }
}
