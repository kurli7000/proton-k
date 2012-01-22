

typedef struct {
  Framebuffer *map;
  float lviewmat[16];
  float lprojmat[16];
} Shadowmapfbo;

#define SAMPLING GL_LINEAR
//#define SAMPLING GL_NEAREST

Framebuffer *glueFramebuffer_depth(int xres, int yres) {
  Framebuffer *fbo=malloc(sizeof(Framebuffer));

  fbo->xres=xres;
  fbo->yres=yres;
  fbo->texnum=0;
  fbo->texnum2=0;

  // generate depthtex
  glGenTextures(1, &fbo->dtexnum);
  glBindTexture(GL_TEXTURE_2D, fbo->dtexnum);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SAMPLING);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SAMPLING);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); 				
*/
  //float *dataz=tmpmalloc(sizeof(float)*xres*yres);
  //for (int i=0; i<xres*yres; i++) dataz[i]=ranf();  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, xres, yres, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
  //tmpfree(dataz);
  glBindTexture(GL_TEXTURE_2D, 0);

  // generate fbo
  glGenFramebuffersEXT(1, &fbo->num);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->num);
  // no color
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  // depth component as texture not renderbuffer
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, fbo->dtexnum, 0);

  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glueLoading();

  return fbo;
}



Shadowmapfbo *shadowmap(int xres, int yres) {
  Shadowmapfbo *sm=malloc(sizeof(Shadowmapfbo));
  
  sm->map=glueFramebuffer_depth(xres, yres);
  
  return sm;
}



void glslbind_shadowmap(int shader, char *pname, int unit, Shadowmapfbo *sm) {
	float biasMatrix[16]={
    0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f};	
	int i;
	int texunit;

  glUniform1iARB(glGetUniformLocationARB(shader, pname), unit);

	switch (unit) {
		case 0: texunit = GL_TEXTURE0_ARB; break;
		case 1: texunit = GL_TEXTURE1_ARB; break;
		case 2: texunit = GL_TEXTURE2_ARB; break;
		case 3: texunit = GL_TEXTURE3_ARB; break;
		case 4: texunit = GL_TEXTURE4_ARB; break;
		case 5: texunit = GL_TEXTURE5_ARB; break;
		case 6: texunit = GL_TEXTURE6_ARB; break;
		case 7: texunit = GL_TEXTURE7_ARB; break;
		default: texunit = GL_TEXTURE0_ARB;
	}
	
  glClientActiveTexture(texunit);
	glActiveTexture(texunit);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sm->map->dtexnum);
	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SAMPLING);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SAMPLING);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY); 				
*/
  glMatrixMode(GL_TEXTURE);
	glLoadMatrixf(biasMatrix);
	glMultMatrixf(sm->lprojmat);
	glMultMatrixf(sm->lviewmat);
	glMatrixMode(GL_MODELVIEW);

}



void beginrender_shadowmap(Shadowmapfbo *sm, Vertex lpos, Vertex ltgt, float fov, float znear, float zfar) {

  glueBeginrender(sm->map);
  gluePrepareflags2(black);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, 1.0, znear, zfar);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();  
  gluLookAt(lpos.x, lpos.y, lpos.z, ltgt.x, ltgt.y, ltgt.z, 0.0, 1.0, 0.0);
 
	glGetFloatv(GL_MODELVIEW_MATRIX, sm->lviewmat);
  glGetFloatv(GL_PROJECTION_MATRIX, sm->lprojmat);

  glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	glShadeModel(GL_FLAT);
	glColorMask(0, 0, 0, 0);

}


void endrender_shadowmap() {
	glueEndrender();

  glueDisabletexture();
  glDisable(GL_POLYGON_OFFSET_FILL);
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
}


