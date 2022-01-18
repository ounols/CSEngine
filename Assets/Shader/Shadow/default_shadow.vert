#define MAX_WEIGHTS 3

// Attributes
//[att.position]//
in vec4 a_position;
//[att.joint_indices]//
in vec3 a_jointIndices;
//[att.weight]//
in vec3 a_weights;

// Uniforms
//[matrix.projection]//
uniform mat4 lightSpaceMatrix;
//[matrix.modelview]//
uniform mat4 model;
//[matrix.joint]//
uniform mat4 u_jointMatrix[MAX_JOINTS];
//[matrix.skinning_mode]//
uniform lowp int u_isSkinning;

void main() {
    vec4 position_final = vec4(0.0);

    //skinning
    if(u_isSkinning == 1) {
        vec4 totalNormal = vec4(0.0);

        for(int i=0; i<MAX_WEIGHTS; i++) {
            mat4 jointTransform = u_jointMatrix[int(a_jointIndices[i])];
            vec4 posePosition = jointTransform * a_position;
            position_final += posePosition * a_weights[i];
        }
        position_final = vec4(position_final.xyz, 1.0);
    } else {
        position_final = a_position;
    }

    gl_Position = lightSpaceMatrix * model * position_final;
}