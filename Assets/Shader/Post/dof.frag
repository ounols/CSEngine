precision highp float;

//Uniforms
//[post.color]//
uniform sampler2D u_color;
//[post.depth]//
uniform sampler2D u_depth;
//Varying
in mediump vec2 v_textureCoordOut;
out vec4 FragColor;

/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this software, either in source code form or as a compiled binary, for any purpose, commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this software dedicate any and all copyright interest in the software to the public domain.
We make this dedication for the benefit of the public at large and to the detriment of our heirs and successors.
We intend this dedication to be an overt act of relinquishment in perpetuity of all present and future rights to this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

const float GoldenAngle = 2.39996323;

// more is better quality
const float Iterations = 400.0;

const mat2 Rotation = mat2(
cos(GoldenAngle),
sin(GoldenAngle),
-sin(GoldenAngle),
cos(GoldenAngle)
);

const float ContrastAmount = 150.0;
const vec3 ContrastFactor = vec3(9.0);
const float Smooth = 2.0;


///////////
/*
	calculates circle of confusion diameter for each fixel from physical parameters and depth map

	this function is unused

	see http://ivizlab.sfu.ca/papers/cgf2012.pdf, page 10
*/
float blurRadius(
float A, // aperture
float f, // focal length
float S1, // focal distance
float far, // far clipping plane
float maxCoc, // mac coc diameter
vec2 uv,
sampler2D depthMap)
{
    vec4 currentPixel = texture(depthMap, uv);

    float S2 = currentPixel.r * far;

    //https://en.wikipedia.org/wiki/Circle_of_confusion
    float coc = A * (abs(S2 - S1) / S2) * (f / (S1 - f));

    float sensorHeight = 0.024;// 24mm

    float percentOfSensor = coc / sensorHeight;

    // blur factor
    return clamp(percentOfSensor, 0.0, maxCoc);
}
///////////

vec3 bokeh(sampler2D tex, sampler2D depth_tex, vec2 uv, float radius) {
    vec3 num, weight;

    float rec = 1.0;// reciprocal

    vec2 horizontalAngle = vec2(0.0, radius * 0.01 / sqrt(Iterations));

    vec2 aspect = vec2(1080.0/1920.0, 1.0);
    vec3 col = vec3(0.);
    float depth = 0.;

    for (float i; i < Iterations; i++) {
        rec += 1.0 / rec;

        horizontalAngle = horizontalAngle * Rotation;

        vec2 offset = (rec - 1.0) * horizontalAngle;

        vec2 sampleUV = uv + aspect * offset;

        float cur_depth = texture(depth_tex, sampleUV).r;

        if(cur_depth >= depth)
            col = texture(tex, sampleUV).rgb;

        // increase contrast and smooth
        vec3 bokeh = Smooth + pow(col, ContrastFactor) * ContrastAmount;

        num += col * bokeh;
        weight += bokeh;
        depth = cur_depth;
    }
    return num / weight;
}

vec3 ColorBalance(vec3 color, vec3 shadows, vec3 midtones, vec3 highlights) {

    color = shadows * color + midtones * color * color + highlights * color * color * color;

    // Return the modified color
    return color;
}

const vec3 sha = vec3(0.5, 0.5, 0.7);
const vec3 mid = vec3(0.5, 0.5, 0.5);
const vec3 hig = vec3(0.7, 0.53, 0.5);

void main(void) {

    float depth = texture(u_depth, v_textureCoordOut).r;
    float dof_depth = min(depth, 0.48);
    vec3 color = bokeh(u_color, u_depth, v_textureCoordOut, max(cos((dof_depth + 0.06) * 6.283184) * 4. + 4., 0.) * 15.);

    FragColor = vec4(ColorBalance(color, sha, mid, hig), 1.);
    gl_FragDepth = depth;
}