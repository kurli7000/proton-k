uniform sampler2D depthtex;
uniform float depth;
uniform vec2 samplepos;
uniform float brite;
uniform vec2 pixel;

varying float bri;


void main() {

  bri=0.0;
  for (int y=-2; y<2; y++) for (int x=-2; x<2; x++) {
    float d=texture2DLod(depthtex, (samplepos*0.5+0.5)+vec2(x, y)*pixel, 0.0).r;
    if (d>depth) bri+=1.0;
  }
  bri*=brite*0.04;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = ftransform();
}

