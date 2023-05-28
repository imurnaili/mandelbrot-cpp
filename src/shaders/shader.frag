#version 450

// Complex Number math by julesb
// https://github.com/julesb/glsl-util
// Additions by Johan Karlsson (DonKarlssonSan)

#define cx_mul(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)
#define squareLength(a) abs(a.x*a.x+a.y*a.y)

#define centerColor vec4(0.0f, 0.0f, 0.0f, 1.0f)
#define iterations 100

vec4 getColor(int i);

in vec2 fragWindowSize;
out vec4 FragColor;

void main() {
	vec2 c = (4 * gl_FragCoord.xy / fragWindowSize) - vec2(2.0f, 2.0f);

	vec2 z = vec2(0.0f, 0.0f);
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