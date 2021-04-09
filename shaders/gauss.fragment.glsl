#version 330 core

out vec3 color;

float sigma = 0.48;
float amplitude = (1 / (2 * 3.14 * sigma * sigma));

float weight(float x, float y) {
  return amplitude * exp(-1 * (x * x + y * y) / (2 * sigma * sigma));
}

void main(void) {
  vec2 uv = (gl_FragCoord.xy - vec2(2560.0, 720.0)) / vec2(2560.0, 720.0);

  color = vec3(0.0, 0.0, 0.0);
  color += vec3(1.5, 1.5, 2.0) * weight(uv.x, uv.y);
}
