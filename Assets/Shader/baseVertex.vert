//[PROJECTION_MATRIX]//
uniform mat4 u_projectionMatrix;
//[MODELVIEW_MATRIX]//
uniform mat4 u_modelViewMatrix;
//[POSITION]//
in vec4 a_position;

void main() {
gl_Position = u_projectionMatrix * u_modelViewMatrix * a_position;
}
