precision highp float;
precision highp int;

// Attributes
attribute vec4 a_position;//Position;
attribute vec3 a_normal;
attribute vec3 a_diffuseMaterial;
//attribute vec2 a_textureCoordIn;

// Uniforms
uniform mat4 u_projectionMatrix;//Projection;
uniform mat4 u_modelViewMatrix;//Modelview;
uniform mat3 u_normalMatrix;//NormalMatrix;
uniform vec4 u_lightPosition;//LightPosition;


uniform lowp int u_isDirectional;

// Varying
varying vec3 v_eyespaceNormal;//EyespaceNormal
varying vec3 v_lightPosition;
varying vec3 v_diffuse;//Diffuse;
//varying vec2 v_textureCoordOut;
varying float v_distance;
//varying vec3 v_vertPosition;


//defined
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;

void main(void) {
	
	v_eyespaceNormal = u_normalMatrix * a_normal;
	v_diffuse = a_diffuseMaterial;

	vec4 positionLight;
	vec4 directionLight = u_lightPosition;

	//direction & position light
	if(u_isDirectional == 1) {
		positionLight = vec4(c_zero, c_zero, c_zero, c_one);
	}else {
		positionLight = u_modelViewMatrix * a_position;
	}

	//for positional light
	vec3 aux = vec3(directionLight - positionLight);

	// Varying
	v_lightPosition = normalize(aux);
	v_distance = length(aux);

	//vec4 vertPosition = u_modelViewMatrix * a_position;
	//v_vertPosition = vec3(vertPosition) / vertPosition.w;


	//vertex position
	gl_Position = u_projectionMatrix * u_modelViewMatrix * a_position;
}