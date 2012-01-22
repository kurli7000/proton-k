void main() {
  gl_TexCoord[0] = gl_TextureMatrix[0]*gl_MultiTexCoord0;
	vec4 pos = gl_ModelViewMatrix*gl_Vertex;
	gl_Position = ftransform();
}
