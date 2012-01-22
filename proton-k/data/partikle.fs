uniform sampler2D spritetex;
varying float bri;

void main(void) {
  vec4 tex=texture2D(spritetex, gl_TexCoord[0].xy);
  gl_FragColor = tex*bri;
}

