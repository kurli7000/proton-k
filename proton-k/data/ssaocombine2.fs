
uniform sampler2D colortex;
uniform sampler2D ssaotex;
uniform sampler2D depthtex;
uniform vec2 pixel;

const float maxstep = 15.0;
const float halfrad = 2.5;

vec2 getstuff(vec2 tc) {
  float br = texture2D(ssaotex, tc).a;
  float d = texture2D(depthtex, tc).r;
  return vec2(br, d);
}

void main() {
	vec2 tc = gl_TexCoord[0].xy;
	vec4 color = texture2D(colortex, tc);

  float u, v;
  float div=0.0;
  float middepth=texture2D(depthtex, tc).r;
  float br=0.0;
  for (v=-halfrad; v<=halfrad; v++) for (u=-halfrad; u<=halfrad; u++) {
    vec2 stuff = getstuff(gl_TexCoord[0].xy+vec2(u, v)*pixel);
    if (abs(middepth-stuff.g)<maxstep) {
      br+=stuff.r;
      div+=1.0;
    }
  } 
  br/=div;

  gl_FragColor = vec4(color.rgb*br, 1.0);
  //gl_FragColor = vec4(br, br, br, 1.0);
}
