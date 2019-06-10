#version 300 es
precision highp float;
precision highp int;

layout (location = 0) in vec3 aPos;

out vec3 v_worldPos;

//[PROJECTION_MATRIX]//
uniform mat4 u_projection;
//[VIEW_MATRIX]//
uniform mat4 u_view;

void main()
{
    v_worldPos = aPos;
    gl_Position =  u_projection * u_view * vec4(v_worldPos, 1.0);
}