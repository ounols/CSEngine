precision highp float;
precision highp int;


//Uniforms
//[texture.albedo]//
uniform sampler2D u_sampler_albedo;
//[texture.metallic]//
uniform sampler2D u_sampler_metallic;
//[texture.roughness]//
uniform sampler2D u_sampler_roughness;
//[texture.ao]//
uniform sampler2D u_sampler_ao;

//IBL
//[light.irradiance]//
uniform samplerCube u_sampler_irradiance;
//[light.prefilter]//
uniform samplerCube u_prefilterMap;
//[light.brdf]//
uniform sampler2D u_brdfLUT;

//[vec3.albedo]//
uniform vec3 u_albedo;
//[float.metallic]//
uniform float u_metallic;
//[float.roughness]//
uniform float u_roughness;
//[float.ao]//
uniform float u_ao;
//[FLOAT_IRRADIANCE]//
uniform vec3 u_irradiance;


//[light.type]//
uniform int u_lightType[MAX_LIGHTS];
//[light.matrix]//
uniform mat4 u_lightMatrix[MAX_LIGHTS];
//[light.radius]//
uniform float u_lightRadius[MAX_LIGHTS];
//[light.color]//
uniform vec3 u_lightColor[MAX_LIGHTS];
//[light.shadow_map]//
uniform sampler2D u_shadowMap[MAX_LIGHTS];
//[light.shadow_mode]//
uniform lowp int u_shadowMode[MAX_LIGHTS];
//[light.size]//
uniform int u_lightSize;

//Varying
in mediump vec3 v_eyespaceNormal;//EyespaceNormal;
in lowp vec3 v_lightPosition[MAX_LIGHTS];
//in lowp vec4 v_fragPosLightSpace[MAX_LIGHTS];
in mediump vec2 v_textureCoordOut;
in lowp float v_distance[MAX_LIGHTS];
in mediump vec3 v_worldPosition;

out vec4 FragColor;

//Defined
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;
const float PI = 3.14159265359;

const lowp float c_shadow_width = 1024.f;
const lowp float c_shadow_height = 1024.f;

//Functions
float DistributionGGX(vec3 N, vec3 H, float roughness);
float DistributionPhong(vec3 N, vec3 H, float roughness);
float DistributionGGXMobile(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
float GeometrySmith_Fast(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float ShadowCalculation(int index, vec4 fragPosLightSpace, vec3 N, vec3 D);
vec4 GetTextureInArray(sampler2D src[MAX_LIGHTS], int index, vec2 uv);

//Macro Functions
float ClampedPow(float X, float Y) {
	return pow(max(abs(X),0.000001f),Y);
}


void main(void) {

	lowp vec3 albedo     = pow(texture(u_sampler_albedo, v_textureCoordOut).rgb, vec3(2.2)) * u_albedo;
	lowp float metallic  = texture(u_sampler_albedo, v_textureCoordOut).b / 2.0;
	lowp float roughness = 0.5 - (texture(u_sampler_albedo, v_textureCoordOut).b / 2.0);
	lowp float ao        = 1.f;

	vec3 N = normalize(v_eyespaceNormal);
	vec3 V0 = normalize(N - v_worldPosition);
	vec3 R = reflect(-V0, N);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);

	lowp int index_shadow = 0;
	for(int i = 0; i < u_lightSize; ++i) {

		// calculate per-light radiance
		vec3 V = (u_lightType[i] > 1) ? V0 : N;
		vec3 L = v_lightPosition[i];
		vec3 H = normalize(V + L);
		float distance = v_distance[i];
		float attenuation = 1.0 / (distance * distance);
		float shadow = 0.0;
		if(u_shadowMode[i] == 1) {
			lowp vec4 fragPosLightSpace = u_lightMatrix[i] * vec4(v_worldPosition, c_one);
			shadow = ShadowCalculation(index_shadow, fragPosLightSpace, N, L);
			++index_shadow;
		}
		vec3 radiance = (u_lightColor[i] * attenuation) * (1.0 - shadow);
		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 specular = nominator / denominator; // prevent divide by zero for NdotV=0.0 or NdotL=0.0

		// kS is equal to Fresnel
		vec3 kS = F;

		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0f - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

	}

	// ambient lighting (we now use IBL as the ambient term)
	vec3 kS = fresnelSchlickRoughness(max(dot(N, V0), 0.0), F0, roughness);
	vec3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
	vec3 irradiance = texture(u_sampler_irradiance, N).rgb;
	vec3 diffuse      = irradiance * albedo;

	// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(u_prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(N, V0), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

//	vec3 irradiance = u_irradiance.r < 0.00 ? texture(u_sampler_irradiance, N).rgb : vec3(0.03);
//	vec3 ambient = irradiance * albedo * ao;

	vec3 color = ambient + Lo;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);

}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float m = roughness * roughness;
	float m2 = m * m;
	float NoH = max(dot(N, H), 0.0);
	float d = ( NoH * m2 - NoH ) * NoH + 1.0;	// 2 mad
	return m2 / ( PI * d * d );
}

float DistributionGGXMobile(vec3 N, vec3 H, float roughness) {
	// Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"

	vec3 NxH = cross(v_eyespaceNormal, H);
	float NoH = max(dot(N, H), 0.0);
	float oneMinusNoHSquared = dot(NxH, NxH);


	float a = NoH * roughness;
	float k = roughness / (oneMinusNoHSquared + a * a);
	float d = k * k * (1.0 / PI);
	return d;
}

// [Blinn 1977, "Models of light reflection for computer synthesized pictures"]
float DistributionPhong(vec3 N, vec3 H, float roughness) {
	float m = roughness * roughness;
	float m2 = m * m;
	float n = 2.0 / m2 - 2.0;
	float NoH = max(dot(N, H), 0.0);

	return (n + 2.0) / (2.0 * PI) * ClampedPow( NoH, n ); // 1 mad, 1 exp, 1 mul, 1 log
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float GeometrySmith_Fast(vec3 N, vec3 V, vec3 L, float roughness) {
	// Hammon 2017, "PBR Diffuse Lighting for GGX+Smith Microsurfaces"
	float NoV = max(dot(N, V), 0.0);
	float NoL = max(dot(N, L), 0.0);
	float v = 0.5 / mix(2.0 * NoL * NoV, NoL + NoV, roughness);
	return v;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float ShadowCalculation(int index, vec4 fragPosLightSpace, vec3 N, vec3 D)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = GetTextureInArray(u_shadowMap, index, projCoords.xy).r;
//	float closestDepth = texture(u_shadowMap[index], projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.05 * (1.0 - dot(N, D)), 0.005);
	// check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(c_shadow_width, c_shadow_height);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = GetTextureInArray(u_shadowMap, index, projCoords.xy + vec2(x, y) * texelSize).r;
//			float pcfDepth = texture(u_shadowMap[index], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

vec4 GetTextureInArray(sampler2D src[MAX_LIGHTS], int index, vec2 uv) {
	if(index >= MAX_LIGHTS) return vec4(0.0f);
	if(index == 0) return texture(src[0], uv);
	if(index == 1) return texture(src[1], uv);
	if(index == 2) return texture(src[2], uv);
	if(index == 3) return texture(src[3], uv);
	if(index == 4) return texture(src[4], uv);

	return texture(src[0], uv);
}