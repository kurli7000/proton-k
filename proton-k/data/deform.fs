uniform sampler2D tex;
uniform float voima;
uniform float freq;
uniform float time;


void main(void) {
	vec2 texcoord = gl_TexCoord[0].xy;
	vec4 pier = texture2D(tex, texcoord);
	vec3 pos = pier.rgb; 
	vec3 pos2;
  float powah;
  float endfade=clamp((time-pier.a*0.5)-4.0, 0.0, 1.0);
  float alkupier;
  
  if (time<pier.a) alkupier=0.0;
  else alkupier=1.0/(time-pier.a);
  
  //powah=voima*clamp(time-pier.a, 0.0, 1.0);
  //powah=voima*clamp((time-pier.a)*10.0, 0.0, 1.0);
  if (time-pier.a<0.0) powah=0.0;
  else powah=voima*(1.0-endfade)+alkupier*voima*0.001;
  
  pos2.x = pos.x+sin(sin(pos.y*0.3127*freq+time*0.831+sin(pos.z*3.672*freq-time*0.512)*0.1))*powah;
  pos2.y = pos.y+sin(sin(pos.z*0.4824*freq-time*0.827+sin(pos.x*4.927*freq+time*0.672)*0.1))*powah;
  pos2.z = pos.z+sin(sin(pos.x*0.3761*freq+time*0.712+sin(pos.y*3.662*freq-time*0.278)*0.1))*powah;

  pos2 = mix(pos2, vec3(0.0, 101.0, 0.0), 0.01*powah+0.05*voima*endfade);
  
  gl_FragColor = vec4(pos2, pier.a);
}
