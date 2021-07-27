#version 330 core

#define MAX_WEIGHTS 3
#define MAX_JOINTS 60



// Attributes
//[POSITION]//
in vec4 a_position;
//[NORMAL]//
in vec3 a_normal;
//[JOINT_INDICES]//
in vec3 a_jointIndices;
//[WEIGHTS]//
in vec3 a_weights;
//[TEX_UV]//
in vec2 a_textureCoordIn;

// Uniforms
//[PROJECTION_MATRIX]//
uniform mat4 u_projectionMatrix;//Projection;
//[MODELVIEW_MATRIX]//
uniform mat4 u_modelViewMatrix;//Modelview;
//[MODELVIEW_NOCAMERA_MATRIX]//
uniform mat4 u_modelViewNoCameraMatrix;//Modelview - no camera matrix;
//[JOINT_MATRIX]//
uniform mat4 u_jointMatrix[MAX_JOINTS];

//[SKINNING_MODE]//
uniform lowp int u_isSkinning;


// Varying
out mediump vec3 v_eyespaceNormal;//EyespaceNormal
out mediump vec2 v_textureCoordOut;
out mediump vec3 v_worldPosition;


//defined
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;

void main(void) {
	vec4 position_final = vec4(0.0);
	vec4 normal_final = vec4(0.0);

	//skinning
    if(u_isSkinning == 1) {
        vec4 totalNormal = vec4(0.0);

        for(int i=0; i<MAX_WEIGHTS; ++i) {
            mat4 jointTransform = u_jointMatrix[int(a_jointIndices[i])];
            vec4 posePosition = jointTransform * a_position;
            position_final += posePosition * a_weights[i];

            vec4 worldNormal = jointTransform * vec4(a_normal, 0.0);
            normal_final += worldNormal * a_weights[i];
        }
        position_final = vec4(position_final.xyz, 1.0);
    } else {
        position_final = a_position;
        normal_final = vec4(a_normal, 0);
    }


	v_eyespaceNormal = mat3(u_modelViewNoCameraMatrix) * normal_final.xyz;
	v_textureCoordOut = a_textureCoordIn;
    v_worldPosition = vec3(u_modelViewNoCameraMatrix * position_final);

    //vertex position
    gl_Position = u_projectionMatrix * u_modelViewMatrix * position_final;

}