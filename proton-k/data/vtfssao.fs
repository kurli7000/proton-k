uniform sampler2D vertextex;
uniform sampler2D spritetex;
varying vec4 color;
varying float dist;
varying float life;
varying float brz;

void main(void) {
  vec4 tex=texture2D(spritetex, gl_TexCoord[0].xy);
  gl_FragData[0] = vec4(vec3(color+tex*life*brz), tex.a);
  gl_FragData[1].r = dist;
//  gl_FragColor = vec4(vec3(color+tex*life*brz), tex.a);
}

