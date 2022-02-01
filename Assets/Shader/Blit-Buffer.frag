precision highp float;

//Uniforms
//[a.color]//
uniform sampler2D u_a_color;
//[b.color]//
uniform sampler2D u_b_color;
//[a.depth]//
uniform sampler2D u_a_depth;
//[b.depth]//
uniform sampler2D u_b_depth;
//Varying
in mediump vec2 v_textureCoordOut;
out vec4 FragColor;

void main(void) {

	float a_depth = texture(u_a_depth, v_textureCoordOut).r;
	float b_depth = texture(u_b_depth, v_textureCoordOut).r;

	vec4 color = vec4(1.0);
	float depth = 0.f;

	if(a_depth < b_depth) {
		color = vec4(texture(u_a_color, v_textureCoordOut).rgb, 1.0);
		depth = a_depth;
	}
	else {
		color = vec4(texture(u_b_color, v_textureCoordOut).rgb, 1.0);
		depth = b_depth;
	}

	FragColor = color;
	gl_FragDepth = depth;
}