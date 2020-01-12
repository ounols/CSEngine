#version 330 core
#define MAX_LIGHTS 16

precision highp float;
precision highp int;

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



//Uniforms
uniform vec3 u_ambientMaterial;//AmbientMaterial;
uniform vec3 u_specularMaterial;//SpecularMaterial;
uniform float u_shininess;
uniform U_LightSource u_lightSources[MAX_LIGHTS];
uniform lowp int u_lightsSize;
uniform sampler2D u_sampler_2d;



//Varying
varying mediump vec3 v_eyespaceNormal;//EyespaceNormal;
varying vec3 v_lightPosition[MAX_LIGHTS];
varying lowp vec3 v_diffuse;//Diffuse;
varying mediump vec2 v_textureCoordOut;
varying float v_distance[MAX_LIGHTS];
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
vec3 calcLightDif(int index);
vec3 calcLightAmb(int index);
vec3 calcLightSpec(int index);
float calcLightAtt(int index);


void main(void) {
	
	vec3 color = vec3(c_zero, c_zero, c_zero);
	vec3 colors[MAX_LIGHTS];

	for(int i = 0; i < u_lightsSize; i++) {
	    colors[i] = vec3(c_zero, c_zero, c_zero);

	    bool isAtt = u_lightSources[i].u_isDirectional == 0;
	    int lightMode = u_lightSources[i].u_lightMode;

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

    for(int i = 0; i < u_lightsSize; i++) {
        color += colors[i];
    }

    lowp vec3 texture_2d = texture2D(u_sampler_2d, v_textureCoordOut).xyz;

	gl_FragColor = vec4(color, 1.0) * vec4(texture_2d, 1.0);

}

// only diffuse light
vec3 calcLightDif(int index) {
	vec3 N = normalize(v_eyespaceNormal);
	vec3 L = normalize(v_lightPosition[index]);
	float df = max(c_zero, dot(N, L));

	return vec3(u_lightSources[index].u_diffuseLight) * v_diffuse * df;
}

// only ambient light
vec3 calcLightAmb(int index) {
	vec3 globalAmbient = vec3(u_lightSources[index].u_ambientLight) * vec3(0.05); // * gl_LightSource[0].ambient
	vec3 ambient = u_ambientMaterial * vec3(u_lightSources[index].u_ambientLight);

	return globalAmbient + ambient;
}

// only specular light
vec3 calcLightSpec(int index) {

	vec3 N = normalize(v_eyespaceNormal);
	vec3 L = normalize(v_lightPosition[index]);
	vec3 E = vec3(c_zero, c_zero, c_one);
	vec3 H = normalize(L + E);
	vec3 R = reflect(-L, N);

	//float sf = max(c_zero, dot(N, H));	// Blinn model
	float sf = max(c_zero, dot(R, E));		// Phong model

	sf = pow(sf, u_shininess);

	return (u_specularMaterial * vec3(u_lightSources[index].u_specularLight) * sf);
}

// only attenuation Factor
float calcLightAtt(int index) {

	if (u_lightSources[index].u_isAttenuation != 1) return c_one;

	float dist = v_distance[index];
	float att = c_one;

	float d = max(dist, c_zero);

	float Kc = u_lightSources[index].u_attenuationFactor.x;
	float Kl = u_lightSources[index].u_attenuationFactor.y;
	float Kq = u_lightSources[index].u_attenuationFactor.z;

	att = c_one / (Kc + (Kl * d) + (Kq * d * d));

	if (att == c_one) {
		float r = u_lightSources[index].u_lightRadius;
		d = max(dist - r, c_zero);
		//L = normalize(L / dist);

		// calculate basic attenuation
		float denom = d / r + 1.0;

		att = c_one / (denom*denom);
	}

	if (att <= c_zero) return c_one;

	return att;
}