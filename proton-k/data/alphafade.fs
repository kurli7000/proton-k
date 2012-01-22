uniform sampler2D t_color;
uniform float limit;
uniform float ramp;

void main(void) {
  vec4 col = texture2D(t_color, gl_TexCoord[0].xy*vec2(1.0, -1.0));
  float alpha = (col.a-limit)*ramp+limit;
  gl_FragColor = vec4(col.rgb, 1.0-clamp(alpha, 0.0, 1.0));
}
