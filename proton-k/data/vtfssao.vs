uniform sampler2D vertextex;
uniform float psize;
uniform float br;
uniform float time;
uniform float frq;
uniform float power;
varying vec4 color;
varying float dist;
varying float life;
varying float brz;
#define pi2 6.2831853

void main() {
  vec2 tc = gl_MultiTexCoord1.xy;
  vec4 paske = texture2DLod(vertextex, tc, 0.0);
  vec4 pos = vec4(paske.rgb, 1.0);
  float ptime = clamp(time-paske.a, 0.0, 100.0);
  float kak = (-cos(ptime*pi2)+1.0)/2.0;
  
  //kak=pow(kak, 0.5);
  
  if (ptime==0.0) life=0.0;
  else life=1.0;

  pos = mix(pos, vec4(pos.z*0.0025, 106.0+pos.y*0.01, -pos.x*0.001, 1.0), ptime);
  pos.x += sin(pos.y*0.01274-sin(pos.z*0.016712))*64.0*kak;
  pos.y += sin(pos.x*0.01921-sin(pos.z*0.015742))*110.0*kak;
  pos.z += sin(pos.y*0.01828-sin(pos.x*0.017982))*128.0*kak;

  if (ptime>1.0) pos.y -= 1000.0;

  float dd = -(gl_ModelViewMatrix*pos).z;

  color = gl_Color*br;
  brz = br;
  dist = dd;
  gl_PointSize = clamp((1.0 + 512.0/dd)*psize, 1.0, 16.0);
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = gl_ModelViewProjectionMatrix*pos;
}

