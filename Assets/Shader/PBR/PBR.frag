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
//[vec3.camera]//
uniform vec3 u_cameraPosition;


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
vec3 GetShadowTextureInArray(int index, vec2 uv);

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
	vec3 V0 = normalize(u_cameraPosition - v_worldPosition);
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
		// Cook-Torrance BRDF 모델을 사용합니다.
		// 빛의 색상(u_lightColor[i])과 거리에 따른 감쇠(attenuation)로 radiance를 계산합니다.
		// shadow는 그림자(shadow)의 영향을 고려한 값입니다.

		vec3 radiance = (u_lightColor[i] * attenuation) * (1.0 - shadow);

		// DistributionGGX 함수를 이용하여, 빛의 입사 각도(V)와 노말(N) 사이의 거리 분포 함수값(NDF)을 계산합니다.
		// GeometrySmith 함수를 이용하여, 양방향 반사 분포 함수(BRDF)에 대한 가중치값(G)을 계산합니다.
		// fresnelSchlick 함수를 이용하여, 빛이 입사한 각도와 물체의 입사각에 대한 Schlick 근사(F) 값을 계산합니다.

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		// nominator는 BRDF의 분자를 나타내며, NDF, G, F 값들의 곱으로 이루어져 있습니다.
		// denominator는 BRDF의 분모를 나타내며, NdotV와 NdotL 값의 곱에 4를 곱한 값과 0.001을 더한 값입니다.
		// nominator와 denominator를 이용하여 specular 값을 계산합니다.

		vec3 nominator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 specular = nominator / denominator; // NdotV=0.0 or NdotL=0.0일 경우 분모가 0이 되는 것을 방지합니다.

		// kS는 Fresnel 값을 나타냅니다.
		vec3 kS = F;

		// 물체의 diffuse한 성질을 나타내는 kD는 1-kS 값입니다.
		// kD값을 linear blend로 계산하면서, metallic 값에 따라 non-metal 또는 partly-metal인 경우에만 diffuse lighting을 사용합니다.
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0f - metallic;

		// NdotL 값과 kD 값을 이용하여, diffuse한 빛의 반사도를 계산합니다.
		float NdotL = max(dot(N, L), 0.0);

		// 이제 BRDF의 값에 색상과 NdotL값을 곱한 후, Lo(최종 렌더링 결과)에 더합니다.
		// 이 때, specular 값은 이미 kS 값과 BRDF의 곱으로 계산되었기 때문에 kS값과 다시 곱하지 않습니다.
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
	float closestDepth = GetShadowTextureInArray(index, projCoords.xy).r;
	//	float closestDepth = texture(u_shadowMap[index], projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.005 * (1.0 - dot(N, D)), 0.0005);
	// check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(c_shadow_width, c_shadow_height);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = GetShadowTextureInArray(index, projCoords.xy + vec2(x, y) * texelSize).r;
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

vec3 GetShadowTextureInArray(int index, vec2 uv) {
	if(index >= MAX_LIGHTS) return vec3(0.0f);
	if(index == 0) return texture(u_shadowMap[0], uv).rgb;
	if(index == 1) return texture(u_shadowMap[1], uv).rgb;
	if(index == 2) return texture(u_shadowMap[2], uv).rgb;
	if(index == 3) return texture(u_shadowMap[3], uv).rgb;
	if(index == 4) return texture(u_shadowMap[4], uv).rgb;
	//	if(index == 5) return texture(u_shadowMap[5], uv).rgb;
	//	if(index == 6) return texture(u_shadowMap[6], uv).rgb;
	//	if(index == 7) return texture(u_shadowMap[7], uv).rgb;

	return texture(u_shadowMap[0], uv).rgb;
}