precision highp float;

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

//Varying
in mediump vec2 v_textureCoordOut;
out vec4 FragColor;

float sdSphere(vec3 p, float s) {
    return length(p)-s;
}

float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

vec2 opU(vec2 d1, vec2 d2) {
    return (d1.x<d2.x) ? d1 : d2;
}

vec2 map(in vec3 pos)
{
    vec2 res = vec2(pos.y, 0.);
    res = vec2(sdSphere(pos-vec3(-0.2f, -1.3f, -0.2f), 0.1), 20.);
    res = opU(vec2(sdBox(pos-vec3(0, 0, 0), vec3(0.1, 0.1, 0.1)), 20.), res);
    res = opU(vec2(sdSphere(pos-vec3(0.1f, -1.45f, 0.12f), 0.1), 20.), res);

    return res;
}

vec2 raycast(in vec3 ro, in vec3 rd)
{
    vec2 res = vec2(-1.0, -1.0);

    float tmin = 0.001;
    float tmax = 20.0;

    float t = tmin;
    for (int i=0; i<70 && t<tmax; i++)
    {
        vec2 h = map(ro+rd*t);
        if (abs(h.x)<(0.0001*t))
        {
            res = vec2(t, h.y);
            break;
        }
        t += h.x;
    }

    return res;
}

vec3 calcNormal(in vec3 pos)
{
    #if 0
    vec2 e = vec2(1.0, -1.0)*0.5773*0.0005;
    return normalize(e.xyy*map(pos + e.xyy).x +
    e.yyx*map(pos + e.yyx).x +
    e.yxy*map(pos + e.yxy).x +
    e.xxx*map(pos + e.xxx).x);
    #else
    // inspired by tdhooper and klems - a way to prevent the compiler from inlining map() 4 times
    vec3 n = vec3(0.0);
    for (int i=0; i<4; i++)
    {
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1), ((i>>1)&1), (i&1))-1.0);
        n += e*map(pos+0.0005*e).x;
        //if( n.x+n.y+n.z>100.0 ) break;
    }
    return normalize(n);
    #endif
}

vec4 render(in vec3 ro, in vec3 rd)
{
    // background
    vec4 col = vec4(0, 1., 0, 0);

    // raycast scene
    vec2 res = raycast(ro, rd);
    float t = res.x;
    float m = res.y;
    if (m>-0.5)
    {
        vec3 pos = ro + t*rd;
        vec3 nor = calcNormal(pos);
        vec3 ref = reflect(rd, nor);
        col = vec4(nor, 1.);
    }
    return vec4(clamp(col, 0.0, 1.0));
}


mat3 setCamera(in vec3 ro, in vec3 ta, float cr)
{
    vec3 cw = normalize(ta-ro);
    vec3 cp = vec3(sin(cr), cos(cr), 0.0);
    vec3 cu = normalize(cross(cw, cp));
    vec3 cv =          (cross(cu, cw));
    return mat3(cu, cv, cw);
}

#define AABB_SIZE 4. * 1.1370995 * 4.

vec2 RayAABBIntersection(vec3 ro, vec3 rd) {

    vec3 aabbmin = vec3(-AABB_SIZE/2) * 0.5;
    vec3 aabbmax =  vec3(AABB_SIZE/2) * 0.5;

    vec3 invR = vec3(1.0) / rd;

    vec3 tbbmin = invR * (aabbmin - ro);
    vec3 tbbmax = invR * (aabbmax - ro);

    vec3 tmin = min(tbbmin, tbbmax);
    vec3 tmax = max(tbbmin, tbbmax);

    float tnear = max(max(tmin.x, tmin.y), tmin.z);
    float tfar  = min(min(tmax.x, tmax.y), tmax.z);

    return tfar > tnear ? vec2(tnear, tfar) : vec2(-999.);
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
    float steps = D / 512.f;

    // Evaluate from 0 to D...
    for (float t = 0.0; t < D; t += steps) {
        // Get the current position along the ray
        vec3 currentPos = tp + direction * t;

        // Get normalized density from volume
        vec4 density = texture(u_sdf_tex, currentPos / vol_size);

        // Get color from transfer function given the normalized density
        vec4 src = vec4(density);

        if (src.a > 0.5) {
            return (src.rgb + src.a * src.rgb * 1.5) * (1. - (t / D) * 1.);  // restore color
        }
    }

    return vec3(0.);
}

vec2 rand( vec2  p ) { p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) ); return fract(sin(p)*43758.5453) * 10.; }

void main(void) {

    vec2 renderFrame = rand(v_textureCoordOut);
    int isRender = int(mod(renderFrame.x + renderFrame.y, 10));
    if(isRender != int(mod(u_frame, 10))) discard;

    float index_pos_y = u_node_size.x * u_node_size.y * u_node_size.z;
    float node_index = floor(mod(v_textureCoordOut.x * 6., 6.))
                    + floor(mod(v_textureCoordOut.y * index_pos_y, index_pos_y)) * 6.;
    float pos_index = floor(node_index / 6.);

    vec3 pos = vec3(floor(mod(pos_index, u_node_size.x)),
                    floor(mod((pos_index / u_node_size.x) , u_node_size.y)),
                    floor(pos_index / (u_node_size.x * u_node_size.y)));

    // Setting New UV
    vec2 new_uv = vec2(1. - fract(v_textureCoordOut.x / (1./6.)),
                       fract(v_textureCoordOut.y / (1. / index_pos_y)));

    vec3 color = vec3(0, 0, 0);
    {
        // camera
        vec3 ro = vec3(0, 0, 0) - pos * u_node_space + u_node_size * u_node_space * 0.5;

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