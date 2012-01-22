uniform sampler2D colortex;
uniform sampler2D depthtex;
uniform sampler2D noisetex;
uniform float dist;
uniform float radius;
uniform float ramp;
uniform vec2 pixel;
uniform vec2 rpos;

const int steps = 50;

void main(void) {
  vec2 texcoord = gl_TexCoord[0].xy;
  float totalcontrib = 1.0;
  vec3 ccol = texture2D(colortex, texcoord).rgb*totalcontrib;
  float cdist = texture2D(depthtex, texcoord).r;
  float coc = clamp(abs(cdist-dist)*ramp, 0.0, 1.0)*radius;
  vec2 rp=rpos+texcoord;

  for (int i=0; i<steps; i++) {
    vec3 n=texture2D(noisetex, rp).rgb;
    //vec2 offs=normalize(n.rg-0.5)*n.b*coc*pixel;
    vec2 offs=(n.rg-0.5)*2.0*coc*pixel;
    vec2 tc=texcoord+offs;
    vec3 col=texture2D(colortex, tc).rgb;
    float d=texture2D(depthtex, tc).r;
    float contrib=(d>cdist)?coc:(clamp(abs(d-dist)*ramp, 0.0, 1.0)*radius);
    ccol+=col*contrib;
    totalcontrib+=contrib;
    rp+=tc*0.124;
  }

  gl_FragColor = vec4(ccol/totalcontrib, 1.0);
}
