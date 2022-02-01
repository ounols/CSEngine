#define MAX_WEIGHTS 3

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
//[matrix.joint]//
uniform mat4 u_jointMatrix[MAX_JOINTS];

//[matrix.skinning_mode]//
uniform lowp int u_isSkinning;


// Varying
out mediump vec3 v_eyespaceNormal;//EyespaceNormal
out lowp vec3 v_lightPosition[MAX_LIGHTS];
//out lowp vec4 v_fragPosLightSpace[MAX_LIGHTS];
out mediump vec2 v_textureCoordOut;
out lowp float v_distance[MAX_LIGHTS];
out mediump vec3 v_worldPosition;
//varying vec3 v_vertPosition;


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


    for(int i = 0; i < u_lightSize; ++i) {
        lowp vec4 lightPurePosition = u_modelViewNoCameraMatrix * position_final;
        lowp vec4 positionLight = lightPurePosition;
        vec4 directionLight = u_lightPosition[i];

        //direction & position light
        if (u_lightType[i] == 1) {
            positionLight = vec4(c_zero, c_zero, c_zero, c_one);
        }

        //for positional light
        vec3 aux = vec3(directionLight - positionLight);

        // Varying
        v_lightPosition[i] = normalize(aux);
        v_distance[i] = length(aux);
        // Varying Shadow
//        if (u_shadowMode[i] == 1) {
//            v_fragPosLightSpace[i] = u_lightMatrix[i] * lightPurePosition;
//        } else {
//            v_fragPosLightSpace[i] = vec4(c_zero, c_zero, c_zero, c_one);
//        }
    }

	//vec4 vertPosition = u_modelViewMatrix * a_position;
	//v_vertPosition = vec3(vertPosition) / vertPosition.w;


    //vertex position
    gl_Position = u_projectionMatrix * u_modelViewMatrix * position_final;

}