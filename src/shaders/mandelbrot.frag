#version 450

// Complex Number math by julesb
// https://github.com/julesb/glsl-util
// Additions by Johan Karlsson (DonKarlssonSan)

#define cx_mul(a, b) dvec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)
#define squareLength(a) abs(a.x*a.x+a.y*a.y)

#define centerColor vec4(0.0f, 0.0f, 0.0f, 1.0f)
#define iterations 1000

//https://gist.github.com/983/e170a24ae8eba2cd174f
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 getColor(int i);

uniform dvec2 windowSize;
uniform dvec2 topLeftCorner;
uniform dvec2 bottomRightCorner;
in vec2 uv;
out vec4 FragColor;

void main() {
	dvec2 screenPos = gl_FragCoord.xy / windowSize;
	dvec2 c = mix(topLeftCorner, bottomRightCorner, screenPos);

	dvec2 z = dvec2(0.0f, 0.0f);
	int j = iterations;
	for(int i = 0; i < iterations; i++) {
		z = cx_mul(z, z) + c;
		if (squareLength(z) > 4.0f) {
			j = min(j, i);
		}
	}

	FragColor = j == iterations ? centerColor : getColor(j);
}

vec4 getColor(int i) {
	return vec4(i/100.0f, i/100.0f, 0.0f, 1.0f);
}