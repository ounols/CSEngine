precision highp float;

//Uniforms
//[main.albedo]//
uniform sampler2D u_buffer;
//Varying
in mediump vec2 v_textureCoordOut;
out vec4 FragColor;

void main(void) {
	FragColor = vec4(texture(u_buffer, v_textureCoordOut).rgb, 1.0);
}