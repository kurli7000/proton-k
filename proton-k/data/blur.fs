uniform sampler2D t_color;
uniform vec2 direction;
uniform int radius;
uniform float minus;

void main(void) {
	vec2 texcoord = gl_TexCoord[0].xy;
  vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
  float pier=0.5/float(radius);

  for (int i=-radius; i<radius; i++) {
    col += clamp(texture2D(t_color, texcoord+float(i)*direction)-vec4(minus), 0.0, 1.0);
  }

  gl_FragColor = col*pier;
}
