uniform sampler2D depthtex;
uniform sampler2D noise;
uniform float t;
uniform float ballsize;
uniform float darken;
uniform vec2 pixel;

const float maxstep = 15.0;
const int steps = 20;

void main() {
  float dd = texture2D(depthtex, gl_TexCoord[0].xy).r;
  vec3 pos = vec3(gl_TexCoord[0].xy, dd);
  float bri = 1.0;
  vec2 xy = (gl_TexCoord[0].xy+vec2(t, t+0.523))*4.11;
  vec2 normal;

  normal.x = dd-texture2D(depthtex, gl_TexCoord[0].xy-vec2(pixel.x, 0.0)).r;
  normal.y = dd-texture2D(depthtex, gl_TexCoord[0].xy-vec2(0.0, pixel.y)).r;
  normal = normalize(normal);
 
  for (int i = 0; i < steps; i ++) {
    vec2 rand = normalize(-1.0+2.0*texture2D(noise, xy).rg);
    float rd = texture2D(noise, xy).b;
    vec2 ray2d = normalize(normal+rand);
    vec3 ray3d = normalize(vec3(ray2d, -1.0))*rd*ballsize;
    float nz = texture2D(depthtex, pos.xy+ray3d.xy/pos.z).r;
    float diff;
    
    diff = (pos.z+ray3d.z)-nz;
    if (diff>0 && diff<maxstep) bri -= 1.0/steps*darken;

    xy.x -= 0.24512;
    xy.y += 0.13843;
  }

  gl_FragColor.a = bri;
}
