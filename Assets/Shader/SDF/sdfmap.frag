precision highp float;
precision mediump sampler3D;

const float c_p_d = 1.00000036;

const mat3 c_pv_m0 = mat3(
vec3(0., 0., 1.),
vec3(0., -1., 0.),
vec3(1., 0., 0.)
);
const mat3 c_pv_m1 = mat3(
vec3(0., 0., -1.),
vec3(0., -1., 0.),
vec3(-1., 0., 0.)
);
const mat3 c_pv_m2 = mat3(
vec3(-1., 0., 0.),
vec3(0., 0., 1.),
vec3(0., 1., 0.)
);
const mat3 c_pv_m3 = mat3(
vec3(-1., 0., 0.),
vec3(0., 0., -1.),
vec3(0., -1., 0.)
);
const mat3 c_pv_m4 = mat3(
vec3(-1., 0., 0.),
vec3(0., -1., 0.),
vec3(0., 0., 1.)
);
const mat3 c_pv_m5 = mat3(
vec3(1., 0., 0.),
vec3(0., -1., 0.),
vec3(0., 0., -1.)
);


//Uniforms
//[sdf.tex]//
uniform sampler3D u_sdf_tex;
//[sdf.env.size]//
uniform int u_env_size;
//[sdf.cell.size]//
uniform vec2 u_cell_size;
//[sdf.node.size]//
uniform vec3 u_node_size;
//[sdf.node.space]//
uniform float u_node_space;
//[sdf.frame.count]//
uniform int u_frame;
//[buffer.source.size]//
uniform vec2 u_src_size;


//[matrix.view.inv]//
uniform mat4 u_viewInvMatrix;
//[matrix.projection]//
uniform mat4 u_projectionMatrix;
//[vec3.camera]//
uniform vec3 u_cameraPosition;

//[light.type]//
uniform int u_lightType[MAX_LIGHTS];
//[light.matrix]//
uniform mat4 u_lightMatrix[MAX_LIGHTS];
//[light.radius]//
uniform float u_lightRadius[MAX_LIGHTS];
//[light.color]//
uniform vec3 u_lightColor[MAX_LIGHTS];
//[light.shadow_map]//
uniform sampler2D u_shadowMap[MAX_LIGHTS];
//[light.shadow_mode]//
uniform lowp int u_shadowMode[MAX_LIGHTS];
//[light.size]//
uniform int u_lightSize;
//[light.position]//
uniform vec4 u_lightPosition[MAX_LIGHTS];

//Varying
in mediump vec2 v_textureCoordOut;
out vec4 FragColor;

vec3 getNormal(vec3 v){
    const float eps = 1e-3;
    vec3 p = vec3(
    texture(u_sdf_tex,v+vec3(eps,  0,  0)).x,
    texture(u_sdf_tex,v+vec3(0,  eps,  0)).x,
    texture(u_sdf_tex,v+vec3(0,  0,  eps)).x);
    vec3 n = vec3(
    texture(u_sdf_tex,v-vec3(eps,  0,  0)).x,
    texture(u_sdf_tex,v-vec3(0,  eps,  0)).x,
    texture(u_sdf_tex,v-vec3(0,  0,  eps)).x);
    return normalize(p-n);
}

#define AABB_SIZE 4. * 1.1370995 * 4.

vec2 RayAABBIntersection(vec3 ro, vec3 rd) {

    vec3 aabbmin = vec3(-AABB_SIZE/2.) * 0.5;
    vec3 aabbmax =  vec3(AABB_SIZE/2.) * 0.5;

    vec3 invR = vec3(1.0) / rd;

    vec3 tbbmin = invR * (aabbmin - ro);
    vec3 tbbmax = invR * (aabbmax - ro);

    vec3 tmin = min(tbbmin, tbbmax);
    vec3 tmax = max(tbbmin, tbbmax);

    float tnear = max(max(tmin.x, tmin.y), tmin.z);
    float tfar  = min(min(tmax.x, tmax.y), tmax.z);

    return tfar > tnear ? vec2(tnear, tfar) : vec2(-999.);
}

vec2 sdTexture(vec3 tp, vec3 direction, float distance, vec3 vol_size) {
    float steps = distance / 64.;

    for (float t = 0.0; t < distance; t += steps) {
        // Get the current position along the ray
        vec3 currentPos = tp + direction * t;

        // Get normalized density from volume
        vec4 density = texture(u_sdf_tex, currentPos / vol_size);
        if(density.a <= 0.5) continue;

        return vec2(length(currentPos), t - steps);
    }
    return vec2(0.);
}

vec3 renderTexture(vec3 origin, vec3 direction) {
    vec2 isct = RayAABBIntersection(origin, direction);

    if (isct.x <= -999.) {
        return vec3(0.);
    }

    if (isct.x < 0.) {
        isct.x = 0.2;
    }

    float D = abs(isct.y - isct.x);

    vec3 wp = origin + direction * isct.x;
    vec3 vol_size = vec3(AABB_SIZE);
    vec3 tp = wp + (vol_size * 0.5);
    float steps = D / 1024.;

    // Evaluate from 0 to D...
    for (float t = 0.0; t < D; t += steps) {
        // Get the current position along the ray
        vec3 currentPos = tp + direction * t;
        vec3 uvw = currentPos / vol_size;

        // Get normalized density from volume
        vec4 density = texture(u_sdf_tex, uvw);
        if (density.a <= 0.5) continue;

        // Get color from transfer function given the normalized density
        vec4 src = vec4(density);
        vec3 n = -getNormal(uvw);

        // Simple diffuse shading
        vec3 co = vec3(0.);
        for(int i = 0; i < 3; ++i) {
            vec3 l = u_lightPosition[i].xyz;
            vec3 H = normalize(n + l);
            vec2 shadow = sdTexture(currentPos + l * (D / 63.), l, D, vol_size);
            float dif = (shadow.x > 0.) ? 0. : clamp(dot(n, l), 0.01, 1.);
            co += dif * u_lightColor[i];
        }
        src = src * vec4(co, 1.);
        return (src.rgb + src.a * src.rgb * 1.5) * (1. - (t / D) * 1.);  // restore color
    }

    return vec3(0.);
}

vec2 rand( vec2  p ) { p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) ); return fract(sin(p)*43758.5453) * 10.; }

void main(void) {

    vec2 renderFrame = rand(v_textureCoordOut);
    int isRender = int(mod(renderFrame.x * renderFrame.y, 20.));
    if(isRender != int(mod(float(u_frame), 20.))) discard;

    float index_pos_y = u_node_size.x * u_node_size.y * u_node_size.z;
    float node_index = floor(mod(v_textureCoordOut.x * u_cell_size.x, u_cell_size.x))
                    + floor(mod(v_textureCoordOut.y * u_cell_size.y, u_cell_size.y)) * u_cell_size.x;
    float pos_index = floor(node_index / 6.);
    if(pos_index > index_pos_y) discard;


    vec3 pos = vec3(floor(mod(pos_index, u_node_size.x)),
                    floor(mod(pos_index / u_node_size.x, u_node_size.y)),
                    floor(pos_index / (u_node_size.x * u_node_size.y)));

    // Setting New UV
    vec2 new_uv = vec2(1. - fract(v_textureCoordOut.x / (1. / u_cell_size.x)),
                       fract(v_textureCoordOut.y / (1. / u_cell_size.y)));

    vec3 color = vec3(0, 0, 0);
    {
        // camera
        vec3 ro = vec3(0, -0.2, 0) - pos * u_node_space + u_node_size * u_node_space * 0.5;

        vec2 p = vec2(2. * (new_uv - 0.5));

        // Setting View Matrix
        mat3 viewMat = mat3(0.);
        {
            int i = int(floor(mod(node_index, 6.)));
            if(i == 0) viewMat = c_pv_m0;
            if(i == 1) viewMat = c_pv_m1;
            if(i == 2) viewMat = c_pv_m2;
            if(i == 3) viewMat = c_pv_m3;
            if(i == 4) viewMat = c_pv_m4;
            if(i == 5) viewMat = c_pv_m5;
        }

        // ray direction
        vec3 rd = viewMat * normalize(vec3(p.xy, c_p_d));

        // render
        color = renderTexture(ro, rd).rgb;
    }

    FragColor = vec4(color, 1.);
    gl_FragDepth = 0.;
}