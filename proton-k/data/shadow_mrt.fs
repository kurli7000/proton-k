//#define SHADOWSAMPLER

#ifdef SHADOWSAMPLER
uniform sampler2DShadow shadowmap;
#else
uniform sampler2D shadowmap;
#endif
uniform sampler3D noise3d;
uniform float smap_pixel;
uniform vec2 glow;
uniform float shadowbr;

varying vec4 shadowcoord;
varying float brite;
varying float distance;
varying vec3 pos;
varying vec3 tpos;

const float ambient=0.05;

float lookup(vec2 offset) {
  vec4 offs = vec4(offset.x * smap_pixel * shadowcoord.w, offset.y * smap_pixel * shadowcoord.w, 0.05, 0.0);
#ifdef SHADOWSAMPLER
	return shadow2DProj(shadowmap, shadowcoord+offs).w;
#else
  return texture2DProj(shadowmap, shadowcoord+offs).r;
#endif
}

void main() {	
  vec4 scoord = shadowcoord/shadowcoord.w;
 	float shadow = 1.0;

  if (shadowcoord.w > 0.0) {
#ifdef SHADOWSAMPLER
    shadow = lookup(vec2(0.0, 0.0));
#else
    float pier=0.0;
    for (float yy = -2.5; yy<=2.5; yy+=1.0) for (float xx = -2.5; xx<=2.5; xx+=1.0) { 
      if (scoord.z < lookup(vec2(xx*0.5, yy*0.5))) pier+=1.0/36.0;
    }
    shadow=pier;
#endif
  }

  vec3 tpos2=tpos*0.5-mod(tpos, 10.0);
  float tex=texture3D(noise3d, tpos2*0.0025).r;
  tex = tex-texture3D(noise3d, tpos*0.02).r*0.6;
  tex = 1.0-tex*0.333;
  
  shadow=mix(1.0, shadow, shadowbr);
  
  vec3 col1 = brite*shadow*vec3(0.9, 0.9, 0.85);
  vec3 col2 = ambient*vec3(0.7, 0.7, 1.0);
	//float br=(brite*shadow+ambient)*(1.0-kakka*0.2);
  
  gl_FragData[0] = vec4((col1+col2)*tex*glow.y+vec3(0.35, 0.5, 1.0)*glow.x, 1.0);
  gl_FragData[1].r = distance;
}
