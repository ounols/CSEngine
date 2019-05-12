#version 300 es

precision highp float;
precision highp int;

//[POSITION]//
in vec3 a_position;

out vec3 v_worldPositon;

//[PROJECTION_MATRIX]//
uniform mat4 u_projectionMatrix;
//[VIEW_MATRIX]//
uniform mat4 u_viewMatrix;


void main() {
    v_worldPositon = a_position;
    gl_Position =  u_projectionMatrix * u_viewMatrix * vec4(v_worldPositon, 1.0);
}