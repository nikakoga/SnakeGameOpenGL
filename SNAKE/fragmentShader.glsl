#version 330 core
out vec4 fragment_color;
uniform vec3 COLOR;
uniform sampler2D textureSampler;
in vec2 texCoord;

void main() {
    vec4 texColor = texture(textureSampler, texCoord);
    fragment_color = vec4(texColor.rgb * COLOR, 1.0);
}
