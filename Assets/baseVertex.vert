precision highp float;
precision highp int;

uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewMatrix;
attribute vec4 a_position;

void main() {
gl_Position = u_modelViewMatrix * a_position;
}
