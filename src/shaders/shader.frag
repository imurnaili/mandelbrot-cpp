#version 450

// Complex Number math by julesb
// https://github.com/julesb/glsl-util
// Additions by Johan Karlsson (DonKarlssonSan)

#define cx_mul(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)

in vec2 fragWindowSize;
out vec4 FragColor;

void main() {
	FragColor = vec4(gl_FragCoord.x / fragWindowSize.x, gl_FragCoord.y / fragWindowSize.y, 0.0f, 1.0f);

	vec2 c = (2 * gl_FragCoord.xy / fragWindowSize) - vec2(1.0f, 1.0f);

	vec2 z = vec2(0.0f, 0.0f);
	for(int i = 0; i < 100; i++) {
		z = cx_mul(z, z) + c;
		if (length(z) > 2.0f) {
			FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		}
	}
	
}
