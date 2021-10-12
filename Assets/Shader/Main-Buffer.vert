#version 300 es

#define MAX_LIGHTS 8


// Attributes
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_textureCoordIn;

// Varying
out vec2 v_textureCoordOut;

void main(void) {
    v_textureCoordOut = a_textureCoordIn;
    gl_Position = vec4(a_position, 1.0);
}