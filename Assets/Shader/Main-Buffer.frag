precision highp float;

//Uniforms
//[main.albedo]//
uniform sampler2D u_buffer;
//Varying
in mediump vec2 v_textureCoordOut;
out vec4 FragColor;

void main(void) {
	vec3 result = texture(u_buffer, v_textureCoordOut).rgb;
	result = mix(vec3(0, 0, 0), result, cos(length(v_textureCoordOut.xy - vec2(0.5)) * 2.2));
	FragColor = vec4(result, 1.0);
}