#version 330 core

#define MAX_LIGHTS 8


// Attributes
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_textureCoordIn;

// Varying
out vec2 v_textureCoordOut;


// Uniform
//[matrix.modelview_nc]//
uniform mat4 u_modelViewNoCameraMatrix;//Modelview - no camera matrix;
//[light.position]//
uniform vec4 u_lightPosition[MAX_LIGHTS];//LightPosition;
//[light.matrix]//
uniform mat4 u_lightMatrix[MAX_LIGHTS];
//[light.shadow_mode]//
uniform lowp int u_shadowMode[MAX_LIGHTS];
//[light.size]//
uniform int u_lightSize;
//[light.type]//
uniform int u_lightType[MAX_LIGHTS];

//defined
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;

void main(void) {

    v_textureCoordOut = a_textureCoordIn;
    gl_Position = vec4(a_position, 1.0);

}