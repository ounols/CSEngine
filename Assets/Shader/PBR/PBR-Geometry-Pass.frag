precision highp float;
precision highp int;

layout (location = 0) out vec4 result_normal_mr;
layout (location = 1) out vec4 result_a_albedo;

//Uniforms
//[texture.albedo]//
uniform sampler2D u_sampler_albedo;
//[texture.metallic]//
uniform sampler2D u_sampler_metallic;
//[texture.roughness]//
uniform sampler2D u_sampler_roughness;
//[texture.ao]//
uniform sampler2D u_sampler_ao;

//[vec3.albedo]//
uniform vec3 u_albedo;
//[float.metallic]//
uniform float u_metallic;
//[float.roughness]//
uniform float u_roughness;
//[float.ao]//
uniform float u_ao;

//Varying
in mediump vec3 v_eyespaceNormal;
in mediump vec2 v_textureCoordOut;

//Defined
const lowp float c_zero = 0.0f;
const lowp float c_one = 1.0f;


void main(void) {

	lowp vec3  albedo    = pow(texture(u_sampler_albedo, v_textureCoordOut).rgb, vec3(2.2f)) * u_albedo;
	lowp float metallic  = u_metallic < c_zero ? texture(u_sampler_metallic, v_textureCoordOut).r : u_metallic;
	lowp float roughness = u_roughness < c_zero ? texture(u_sampler_roughness, v_textureCoordOut).r : u_roughness;
	lowp float ao        = u_ao < c_zero ? texture(u_sampler_ao, v_textureCoordOut).r : u_ao;

	vec3 N = normalize(v_eyespaceNormal);

	result_normal_mr = vec4(N.x, N.y, metallic, roughness);
	result_a_albedo = vec4(albedo, ao);
}