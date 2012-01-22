
uniform sampler2D colortex;
uniform sampler2D ssaotex;
uniform sampler2D depth;
uniform float radiusx, radiusy;

float med5(float a, float b, float c, float d, float e) {
  float t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10;

  if (a<b) { t0=a; t1=b; } else { t0=b; t1=a; }
  if (c<d) { t2=c; t3=d; } else { t2=d; t3=c; }
  if (t0>t2) { t4=t3; t5=t0; t6=t1; } else { t4=t1; t5=t2; t6=t3; }
  if (t4<e) { t7=t4; t8=e; } else { t7=e; t8=t4; }
  if (t5>t6) { t9=t6; t10=t7; } else { t9=t8; t10=t5; }
  return min(t9, t10);
}

void main() {
	vec4 color = texture2D(colortex, gl_TexCoord[0].xy);

	vec2 texcoord = gl_TexCoord[0].xy;
	float a, b, c, d, e;
	float rr, gg, bb;
	float v=1.0;
	
	if (color.a > 0.0) {
	  a = texture2D(ssaotex, vec2(texcoord.x-radiusx+(radiusx*0.5), texcoord.y-radiusy+(radiusy*0.5))).a;
  	b = texture2D(ssaotex, vec2(texcoord.x+radiusx+(radiusx*0.5), texcoord.y-radiusy+(radiusy*0.5))).a;
  	c = texture2D(ssaotex, vec2(texcoord.x+radiusx+(radiusx*0.5), texcoord.y+radiusy+(radiusy*0.5))).a;
  	d = texture2D(ssaotex, vec2(texcoord.x-radiusx+(radiusx*0.5), texcoord.y+radiusy+(radiusy*0.5))).a;
  	e = texture2D(ssaotex, texcoord).a;
   
	  float v1 = med5(a, b, c, d, e);
	  float v2 = (a+b+c+d+e)/5.0;
  	v = (v1+v2)*0.5;
  } 
	gl_FragColor = vec4(mix(color.xyz, color.xyz*v, color.a), 1.0);

	//gl_FragColor = vec4(color.xyz, 1.0);
}
