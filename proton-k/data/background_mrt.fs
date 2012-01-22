varying float distance;
varying vec3 pos;

uniform sampler2D texture;
uniform sampler2D slit1;
uniform float slitpos, slitbr;

void main() {	
  vec4 col = texture2D(texture, gl_TexCoord[0].xy);
  float yy = abs(1.0/pos.y)/8000.0;
  float xpos = pos.x*yy*512.0+0.5;
  xpos *= sign(pos.y);
  float ypos = sqrt(yy*0.5)+slitpos;
  vec4 slitcol = texture2D(slit1, vec2(ypos, xpos));
  gl_FragData[0] = col+slitcol*slitbr*pos.z;
  gl_FragData[1].r = distance;
}
