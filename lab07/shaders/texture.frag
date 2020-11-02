#version 330 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

void main(){
    // TODO: [Task 8] Sample the texture "tex" at the given UV-coordinates.
    fragColor = texture(tex, uv);
}
