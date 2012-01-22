uniform vec3 lightpos;

varying float brite;
varying vec4 shadowcoord;
varying float distance;
varying vec3 pos;
varying vec3 tpos;

	
void main() {
  vec3 normal = normalize(gl_NormalMatrix*gl_Normal);
	vec3 vpos = vec3(gl_ModelViewMatrix*gl_Vertex);
	vec3 lpos = vec3(gl_ModelViewMatrix*vec4(lightpos, 1.0));
	
  shadowcoord = gl_TextureMatrix[0]*gl_Vertex;
	brite = clamp(dot(normal, normalize(lpos-vpos)), 0.0, 1.0);
  distance = -vpos.z;
  pos = gl_Vertex.xyz;
  tpos = gl_MultiTexCoord1.xyz;
  gl_Position = ftransform();
}
