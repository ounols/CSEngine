#version 300 es

precision highp float;
precision highp int;

layout (location = 0) out vec3 result_position;
layout (location = 1) out vec3 result_normal;
layout (location = 2) out vec3 result_albedo;
layout (location = 3) out vec3 result_material;

//Uniforms
//[gbuffer.texture.albedo]//
uniform sampler2D u_sampler_albedo;
//[gbuffer.texture.metallic]//
uniform sampler2D u_sampler_metallic;
//[gbuffer.texture.roughness]//
uniform sampler2D u_sampler_roughness;
//[gbuffer.texture.ao]//
uniform sampler2D u_sampler_ao;

//[gbuffer.vec3.albedo]//
uniform vec3 u_albedo;
//[gbuffer.float.metallic]//
uniform float u_metallic;
//[gbuffer.float.roughness]//
uniform float u_roughness;
//[gbuffer.float.ao]//
uniform float u_ao;

//Varying
in mediump vec3 v_eyespaceNormal;
in mediump vec2 v_textureCoordOut;
in mediump vec3 v_worldPosition;

out vec4 FragColor;

//Defined
const lowp float c_zero = 0.0f;
const lowp float c_one = 1.0f;


void main(void) {

	lowp vec3  albedo    = u_albedo.r < c_zero ? pow(texture(u_sampler_albedo, v_textureCoordOut).rgb, vec3(2.2f)) : u_albedo;
	lowp float metallic  = u_metallic < c_zero ? texture(u_sampler_metallic, v_textureCoordOut).r : u_metallic;
	lowp float roughness = u_roughness < c_zero ? texture(u_sampler_roughness, v_textureCoordOut).r : u_roughness;
	lowp float ao        = u_ao < c_zero ? texture(u_sampler_ao, v_textureCoordOut).r : u_ao;

	vec3 N = normalize(v_eyespaceNormal);

	result_position = v_worldPosition;
	result_normal = N;
	result_albedo = albedo;
	result_material = vec3(metallic, roughness, ao);
}