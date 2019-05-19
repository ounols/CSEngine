#version 300 es

precision highp float;
precision highp int;

in vec3 v_worldPositon;
out vec4 FragColor;

//[EquirectangularMap]//
uniform sampler2D u_equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main() {
	vec2 uv = SampleSphericalMap(normalize(v_worldPositon));
	vec3 color = texture(u_equirectangularMap, uv).rgb;

//	FragColor = vec4(color, 1.0);
	FragColor = vec4(0.5, 1.0, 1.0, 1);
}