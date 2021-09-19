#version 300 es

#define MAX_WEIGHTS 3
#define MAX_JOINTS 60



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
uniform mat4 u_projectionMatrix;//Projection;
//[matrix.modelview]//
uniform mat4 u_modelViewMatrix;//Modelview;
//[matrix.modelview_nc]//
uniform mat4 u_modelViewNoCameraMatrix;//Modelview - no camera matrix;
//[matrix.joint]//
uniform mat4 u_jointMatrix[MAX_JOINTS];

//[matrix.skinning_mode]//
uniform lowp int u_isSkinning;


// Varying
out mediump vec3 v_eyespaceNormal;//EyespaceNormal
out mediump vec2 v_textureCoordOut;
out mediump vec3 v_worldPosition;


//defined
const lowp float c_zero = 0.0f;
const lowp float c_one = 1.0f;

void main(void) {
	vec4 position_final = vec4(c_zero);
	vec4 normal_final = vec4(c_zero);

	//skinning
    if(u_isSkinning == 1) {
        vec4 totalNormal = vec4(c_zero);

        for(int i=0; i<MAX_WEIGHTS; ++i) {
            mat4 jointTransform = u_jointMatrix[int(a_jointIndices[i])];
            vec4 posePosition = jointTransform * a_position;
            position_final += posePosition * a_weights[i];

            vec4 worldNormal = jointTransform * vec4(a_normal, c_zero);
            normal_final += worldNormal * a_weights[i];
        }
        position_final = vec4(position_final.xyz, c_one);
    } else {
        position_final = a_position;
        normal_final = vec4(a_normal, c_zero);
    }


	v_eyespaceNormal = mat3(u_modelViewNoCameraMatrix) * normal_final.xyz;
	v_textureCoordOut = a_textureCoordIn;
    v_worldPosition = vec3(u_modelViewNoCameraMatrix * position_final);

    //vertex position
    gl_Position = u_projectionMatrix * u_modelViewMatrix * position_final;

}