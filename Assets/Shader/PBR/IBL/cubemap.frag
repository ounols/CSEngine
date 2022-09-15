precision highp float;
precision highp int;

out vec4 FragColor;
in vec3 v_worldPos;

//[ENV_MAP]//
uniform samplerCube u_environmentMap;


void main() {
    FragColor = texture(u_environmentMap, v_worldPos);
}