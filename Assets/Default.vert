//#version 100
precision highp float;
precision highp int;

#define MAX_LIGHTS 16

struct U_LightSource {
    vec4 u_diffuseLight;
    vec4 u_ambientLight;
    vec4 u_specularLight;

    int u_lightMode;
    int u_isDirectional;

    /**Attenuation Factor**/
    int u_isAttenuation;
    vec3 u_attenuationFactor;
    float u_lightRadius;

};


// Attributes
attribute vec4 a_position;//Position;
attribute vec3 a_normal;
attribute vec3 a_diffuseMaterial;
//attribute vec2 a_textureCoordIn;

// Uniforms
uniform mat4 u_projectionMatrix;//Projection;
uniform mat4 u_modelViewMatrix;//Modelview;
uniform mat4 u_modelViewNoCameraMatrix;//Modelview - no camera matrix;
uniform mat3 u_normalMatrix;//NormalMatrix;
uniform vec4 u_lightPosition[MAX_LIGHTS];//LightPosition;


uniform lowp int u_isDirectional[MAX_LIGHTS];
uniform U_LightSource u_lightSources[MAX_LIGHTS];
uniform lowp int u_lightsSize;


// Varying
varying vec3 v_eyespaceNormal;//EyespaceNormal
varying vec3 v_diffuse;//Diffuse;
varying vec3 v_lightPosition[MAX_LIGHTS];
//varying vec2 v_textureCoordOut;
varying float v_distance[MAX_LIGHTS];
//varying vec3 v_vertPosition;


//defined
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;

void main(void) {
	
	v_eyespaceNormal = u_normalMatrix * a_normal;
	v_diffuse = a_diffuseMaterial;


    for(int i = 0; i < u_lightsSize; i++) {
        	vec4 positionLight;
        	vec4 directionLight = u_lightPosition[i];

        	//direction & position light
        	if(u_lightSources[i].u_isDirectional == 1) {
        		positionLight = vec4(c_zero, c_zero, c_zero, c_one);
        	}else {
        		positionLight = u_modelViewNoCameraMatrix * a_position;
        	}

        	//for positional light
        	vec3 aux = vec3(directionLight - positionLight);

        	// Varying
        	v_lightPosition[i] = normalize(aux);
        	v_distance[i] = length(aux);
    }

	//vec4 vertPosition = u_modelViewMatrix * a_position;
	//v_vertPosition = vec3(vertPosition) / vertPosition.w;

	//vertex position
	gl_Position = u_projectionMatrix * u_modelViewMatrix * a_position;
}
