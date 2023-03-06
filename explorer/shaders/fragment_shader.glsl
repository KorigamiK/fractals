#version 300 es

precision highp float;

out vec4 out_color;

in vec2 v_texcoord;
uniform vec2 u_center;
uniform float u_zoom;
uniform int u_max_iterations;

void main() {
  vec2 c = vec2(v_texcoord.x - 0.5, v_texcoord.y - 0.5) / u_zoom + u_center;
  vec2 z = vec2(0.0, 0.0);
  int i;
  for(i = 0; i < u_max_iterations; ++i) {
    vec2 z_new = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
    if(length(z_new) > 2.0)
      break;
    z = z_new;
  }
  float t = float(i) / float(u_max_iterations);
  out_color = vec4(t, t, t, 1.0);
}
