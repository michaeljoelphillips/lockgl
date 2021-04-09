#version 330 core

out vec4 color;
uniform int frame;
uniform sampler2D image;
uniform vec2 screen_size;

// Center screen coordinates at (0,0)
vec2 screen = screen_size * vec2(1, -1);

// The convolution kernel width
int kernel_width = min(frame, 11);

float variance = 16;
float amplitude = (1 / (6.28 * variance));
float divisor = 2 * variance;

// Calculate the weight of a convolution kernel offset using a 2D Gaussian function
float gaussian(vec2 kernel_offset) {
  vec2 square = kernel_offset * kernel_offset;

  return amplitude * exp(-(square.x + square.y) / divisor);
}

// Calculate texel coordinate offsets for bilinear interpolation
//
// Returns a vector containing the x/y texel coordinate offsets for bilinear
// interpolation and the combined kernel weights (z) of the kernel offsets.
vec3 texel_values(vec2 current_offset) {
  vec2 next_offset = current_offset + 1;

  float current_offset_weight = gaussian(current_offset);
  float next_offset_weight = gaussian(next_offset);
  float combined_weights = current_offset_weight + next_offset_weight;

  vec2 offsets = ((current_offset * current_offset_weight) + (next_offset * next_offset_weight)) / combined_weights;

  return vec3(offsets, combined_weights);
}

// Clamp texture samples to the nearest edge
vec2 clamp(vec2 coord) {
  vec2 uv = (gl_FragCoord.xy + coord.xy) / screen;

  return vec2(min(max(uv.x, -1), 1), min(max(uv.y, -1), 1));
}

// Apply a Gaussian filter to a texture
//
// Convolves a texture with a 2D kernel of size `kernel_width ^ 2 / 2`.
//
// Texture sampling is optimized with bilinear interpolation by sampling at
// texel coordinates that are offset in relation to their kernel weights.
void main(void) {
  vec3 result;
  float weight;

  for (int x = -kernel_width; x <= kernel_width; x += 2) {
    for (int y = -kernel_width; y < -1 * x; y += 2) {
      result = texel_values(vec2(x, y));

      color += texture(image, clamp(result.xy)) * result.z;
      color += texture(image, clamp(-1 * result.xy)) * result.z;

      weight += result.z * 2;
    }
  }

  color = color / weight;
}
