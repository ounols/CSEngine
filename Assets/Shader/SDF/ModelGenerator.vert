// Attributes
//[att.position]//
in vec4 a_position;
//[att.normal]//
in vec3 a_normal;
//[att.joint_indices]//
in vec3 a_jointIndices;
//[att.weight]//
in vec3 a_weights;
//[att.tex_uv]//
in vec2 a_textureCoordIn;

// Uniforms
//[matrix.projection]//
uniform mat4 u_projectionMatrix;
//[matrix.view]//
uniform mat4 u_viewMatrix;

// Varying
out mediump vec2 v_textureCoordOut;

void main(void) {
    v_textureCoordOut = a_textureCoordIn;
    gl_Position = u_projectionMatrix * a_position;
}