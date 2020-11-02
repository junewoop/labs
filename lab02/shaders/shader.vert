#version 330

layout(location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main() {
    gl_Position = vec4(position, 1.0);
    gl_Position = model * gl_Position;
    gl_Position = view * gl_Position;
    gl_Position = perspective * gl_Position;
}
