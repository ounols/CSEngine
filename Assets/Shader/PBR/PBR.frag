#version 300 es
//#version 100
#define MAX_LIGHTS 16

precision highp float;
precision highp int;


//Uniforms
//[TEX2D_ALBEDO]//
uniform sampler2D u_sampler_2d;
//[TEXCUBE_IRRADIANCE]//
uniform samplerCube u_sampler_irradiance;
//[LIGHT_TYPE]//
uniform int u_lightType[MAX_LIGHTS];
//[LIGHT_RADIUS]//
uniform float u_lightRadius[MAX_LIGHTS];
//[LIGHT_COLOR]//
uniform vec3 u_lightColor[MAX_LIGHTS];


//Varying
in mediump vec3 v_eyespaceNormal;//EyespaceNormal;
in vec3 v_lightPosition[MAX_LIGHTS];
in lowp vec3 v_diffuse;//Diffuse;
in mediump vec2 v_textureCoordOut;
in float v_distance[MAX_LIGHTS];
in vec3 v_vertPosition;

out vec4 FragColor;

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
vec3 calcLightDif(int index);
vec3 calcLightAmb(int index);
vec3 calcLightSpec(int index);
float calcLightAtt(int index);


void main(void) {
	
	vec3 color = vec3(c_zero, c_zero, c_zero);
	vec3 colors[MAX_LIGHTS];

	for(int i = 0; i < MAX_LIGHTS; i++) {
	    colors[i] = vec3(c_zero, c_zero, c_zero);

	    int lightMode = MODE_COMPLETE;

        	if (lightMode == MODE_DIF || lightMode == MODE_AMB_DIF || lightMode == MODE_DIF_SPEC ||
        		lightMode == MODE_COMPLETE) {
        		colors[i] += calcLightDif(i);
        	}
        	if (lightMode == MODE_AMB || lightMode == MODE_AMB_DIF || lightMode == MODE_AMB_SPEC ||
        		lightMode == MODE_COMPLETE) {
        		colors[i] += calcLightAmb(i);
        	}
        	if (lightMode == MODE_SPEC || lightMode == MODE_DIF_SPEC || lightMode == MODE_AMB_SPEC ||
        		lightMode == MODE_COMPLETE) {
        		colors[i] += calcLightSpec(i);
        	}

        	//Attenuation Factor
        	colors[i] *= calcLightAtt(i);
	}

    for(int i = 0; i < MAX_LIGHTS; i++) {
        color += colors[i];
    }

    lowp vec3 diffuse = texture2D(u_sampler_2d, v_textureCoordOut).rgb;

	FragColor = vec4(color, 1.0) * vec4(diffuse, 1.0);

}

// only diffuse light
vec3 calcLightDif(int index) {
//	vec3 N = normalize(v_eyespaceNormal);
//	vec3 L = normalize(v_lightPosition[index]);
//	float df = max(c_zero, dot(N, L));
//
//	return vec3(u_lightSources[index].u_diffuseLight) * v_diffuse * df;
	return vec3(1);
}

// only ambient light
vec3 calcLightAmb(int index) {
//	vec3 globalAmbient = vec3(u_lightSources[index].u_ambientLight) * vec3(0.05); // * gl_LightSource[0].ambient
//	vec3 ambient = u_ambientMaterial * vec3(u_lightSources[index].u_ambientLight);
//
//	return globalAmbient + ambient;
	return vec3(1);
}

// only specular light
vec3 calcLightSpec(int index) {

//	vec3 N = normalize(v_eyespaceNormal);
//	vec3 L = normalize(v_lightPosition[index]);
//	vec3 E = vec3(c_zero, c_zero, c_one);
//	vec3 H = normalize(L + E);
//	vec3 R = reflect(-L, N);
//
//	//float sf = max(c_zero, dot(N, H));	// Blinn model
//	float sf = max(c_zero, dot(R, E));		// Phong model
//
//	sf = pow(sf, u_shininess);
//
//	return (u_specularMaterial * vec3(u_lightSources[index].u_specularLight) * sf);
	return vec3(1);
}

// only attenuation Factor
float calcLightAtt(int index) {

	if (u_lightType[index] > 1) return c_one;

	float dist = v_distance[index];
	float att = c_one;

	float d = max(dist, c_zero);

	float r = u_lightRadius[index];
	d = max(dist - r, c_zero);
	//L = normalize(L / dist);

	// calculate basic attenuation
	float denom = d / r + 1.0;

	att = c_one / (denom*denom);

	if (att <= c_zero) return c_one;

	return att;
}