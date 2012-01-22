varying vec3 pos;
uniform sampler3D noise3d;
uniform float brite;
uniform float time;

void main() {
  vec3 pos1=pos*0.081+vec3(0.0, 0.0, -time*0.141);
  vec3 pos2=pos*0.047+vec3(0.0, 0.0, -time*0.197);
  float col1 = texture3D(noise3d, pos1).r;
  float col2 = texture3D(noise3d, pos2).r;
  vec3 col3 = col1*col1*vec3(0.4, 0.25, 0.6)+col2*col2*vec3(0.25, 0.5, 0.6);
 // float bw = (col1.r+col1.g+col1.b+col2.r+col2.g+col2.b)*0.1666;

  gl_FragColor = vec4((col3*col3)*brite, 1.0);
}