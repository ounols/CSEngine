precision highp float;
precision highp int;

//Uniforms
uniform vec4 u_diffuseLight;
uniform vec4 u_ambientLight;
uniform vec4 u_specularLight;
uniform vec3 u_ambientMaterial;//AmbientMaterial;
uniform vec3 u_specularMaterial;//SpecularMaterial;

uniform lowp int u_lightMode;


//Varying
varying mediump vec3 v_eyespaceNormal;//EyespaceNormal;
varying highp vec3 v_lightPosition;
varying lowp vec3 v_diffuse;//Diffuse;
//varying mediump vec2 v_textureCoordOut;
varying highp float v_distance;


//Define
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;


//Functions
vec3 calcLightDif();


void main(void) {
	
	vec3 color = v_diffuse;
	color = calcLightDif();

	gl_FragColor = vec4(color, 0.8);

}

// only diffuse light
vec3 calcLightDif() {
	vec3 N = normalize(v_eyespaceNormal);
	vec3 L = normalize(v_lightPosition);
	float df = max(c_zero, dot(N, L));

	return vec3(u_diffuseLight) * v_diffuse * df;
}