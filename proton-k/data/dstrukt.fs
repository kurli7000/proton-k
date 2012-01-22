uniform sampler2D t_color;
uniform sampler2D t_noise;
uniform float blockpow;
uniform float sinpow;
uniform float linepow;
uniform float colorsep;
uniform float time;

float modmod(float a, float b) {
  if (a<0.0) return -mod(-a, b);
  else return mod(a, b);
}

void main(void) {
	vec2 texcoord = gl_TexCoord[0].xy;
  vec4 col1, col2;
  float offs1, offs2;
  float blok, line;
  float blokpos, linepos;
  
  blokpos=sin(texcoord.y*17.5+time*777.0)+texcoord.y*11.7;
  blokpos-=modmod(blokpos, 1.0);
  blok=sin(blokpos)*blockpow;
  
  linepos=sin(texcoord.y*17.5+time*666.0)*texcoord.y*95.7;
  linepos-=modmod(linepos, 2.0);
  line=sin(linepos);

  offs1=sin(texcoord.y*3.7+time*15.0+blok*5.0+sin(colorsep))*sinpow*0.5+blok+line*linepow;
  offs2=sin(texcoord.y*3.7+time*11.0+blok*5.0-sin(colorsep*0.7))*sinpow*0.5+blok+line*linepow;
  
  col1=texture2D(t_color, texcoord+vec2(offs1, 0))*vec4(1.0, 0.0, 0.0, 0.5);
  col2=texture2D(t_color, texcoord+vec2(offs2, 0))*vec4(0.0, 1.0, 1.0, 0.5);
 
  gl_FragColor = (col1+col2)+line*6.0*linepow-blok*2.0;
}
