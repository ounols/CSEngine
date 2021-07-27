#version 330 core
//#version 100

precision highp float;
precision highp int;

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
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;


void main(void) {

	lowp vec3 albedo     = pow(texture(u_sampler_albedo, v_textureCoordOut).rgb, vec3(2.2f)) * u_albedo;
	lowp float metallic  = texture(u_sampler_albedo, v_textureCoordOut).b / 2.0f;
	lowp float roughness = 0.5f - (texture(u_sampler_albedo, v_textureCoordOut).b / 2.0f);
	lowp float ao        = 1.f;

	vec3 N = normalize(v_eyespaceNormal);
	vec3 V0 = normalize(N - v_worldPosition);

	FragColor = vec4(color, 1.0);

}