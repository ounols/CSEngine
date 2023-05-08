precision highp float;

//Uniforms
//[post.color]//
uniform sampler2D u_color;
//[post.depth]//
uniform sampler2D u_depth;
//Varying
in mediump vec2 v_textureCoordOut;
out vec4 FragColor;


void main(void) {

    float depth = texture(u_depth, v_textureCoordOut).r;
    float dof_depth = min(depth, 0.48);
    vec3 color = bokeh(u_color, u_depth, v_textureCoordOut, max(cos((dof_depth + 0.06) * 6.283184) * 4. + 4., 0.) * 15.);

    FragColor = vec4(ColorBalance(color, sha, mid, hig), 1.);
    gl_FragDepth = depth;
}