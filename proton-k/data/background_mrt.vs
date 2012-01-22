varying float distance;
varying vec3 pos;

void main() {
	vec3 vpos = vec3(gl_ModelViewMatrix*gl_Vertex);
  distance=-vpos.z;
  pos = gl_Vertex.xyz;
  gl_TexCoord[0] = gl_TextureMatrix[0]*gl_MultiTexCoord0;
	gl_Position = ftransform();
} 
