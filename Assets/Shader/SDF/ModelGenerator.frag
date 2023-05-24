precision highp float;
precision highp int;


//Uniforms
//[texture.albedo]//
uniform sampler2D u_sampler_albedo;
//[vec3.albedo]//
uniform vec3 u_albedo;

//Varying
in mediump vec2 v_textureCoordOut;

out vec4 FragColor;


void main(void) {

	vec3 albedo = texture(u_sampler_albedo, v_textureCoordOut).rgb * u_albedo;
	FragColor = vec4(0, 1, 0, 1.0);

}