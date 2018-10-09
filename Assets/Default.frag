//#version 100
precision highp float;
precision highp int;

//Uniforms
uniform vec4 u_diffuseLight;
uniform vec4 u_ambientLight;
uniform vec4 u_specularLight;
uniform vec3 u_ambientMaterial;//AmbientMaterial;
uniform vec3 u_specularMaterial;//SpecularMaterial;
uniform float u_shininess;

uniform lowp int u_lightMode;
uniform lowp int u_isDirectional;


//Varying
varying mediump vec3 v_eyespaceNormal;//EyespaceNormal;
varying highp vec3 v_lightPosition;
varying lowp vec3 v_diffuse;//Diffuse;
//varying mediump vec2 v_textureCoordOut;
varying highp float v_distance;
varying vec3 v_vertPosition;



//Defined
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;

const lowp int MODE_AMB = 1;
const lowp int MODE_DIF = 2;
const lowp int MODE_SPEC = 3;
const lowp int MODE_AMB_DIF = 12;
const lowp int MODE_AMB_SPEC = 13;
const lowp int MODE_DIF_SPEC = 23;
const lowp int MODE_COMPLETE = 123;



//Functions
vec3 calcLightDif();
vec3 calcLightAmb();
vec3 calcLightSpec();


void main(void) {
	
	vec3 color = vec3(c_zero, c_zero, c_zero);

	bool isAtt = u_isDirectional == 0;

	if(u_lightMode == MODE_DIF || u_lightMode == MODE_AMB_DIF || u_lightMode == MODE_DIF_SPEC ||
		u_lightMode == MODE_COMPLETE) {
		color += calcLightDif();
	}
	if (u_lightMode == MODE_AMB || u_lightMode == MODE_AMB_DIF || u_lightMode == MODE_AMB_SPEC ||
		u_lightMode == MODE_COMPLETE) {
		color += calcLightAmb();
	}
	if (u_lightMode == MODE_SPEC || u_lightMode == MODE_DIF_SPEC || u_lightMode == MODE_AMB_SPEC ||
		u_lightMode == MODE_COMPLETE) {
		color += calcLightSpec();
	}


	gl_FragColor = vec4(color, 0.8);

}

// only diffuse light
vec3 calcLightDif() {
	vec3 N = normalize(v_eyespaceNormal);
	vec3 L = normalize(v_lightPosition);
	float df = max(c_zero, dot(N, L));

	return vec3(u_diffuseLight) * v_diffuse * df;
}

// only ambient light
vec3 calcLightAmb() {
	vec3 globalAmbient = vec3(u_ambientLight) * vec3(0.05); // * gl_LightSource[0].ambient
	vec3 ambient = u_ambientMaterial * vec3(u_ambientLight);

	return globalAmbient + ambient;
}

// only specular light
vec3 calcLightSpec() {

	vec3 N = normalize(v_eyespaceNormal);
	vec3 L = normalize(v_lightPosition);
	vec3 E = vec3(c_zero, c_zero, c_one);
	vec3 H = normalize(L + E);
	vec3 R = reflect(-L, N);

	//float sf = max(c_zero, dot(N, H));	// Blinn model
	float sf = max(c_zero, dot(R, E));		// Phong model

	sf = pow(sf, u_shininess);

	return (u_specularMaterial * vec3(u_specularLight) * sf);
}
