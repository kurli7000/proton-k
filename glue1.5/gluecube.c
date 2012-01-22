
#include "gluecube.h"


/**********************************************************************
  Cubemap stuff
**********************************************************************/


//GLUE_REFRECTION vittuun
// GLUE_CUBEMAP_OK vittuun

int cubesides[]={
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
  GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB};



glue_static Framebuffer *glueCubemap(int res) {
  Framebuffer *fbo;
  int x;
  
  fbo=malloc(sizeof(Framebuffer));
  
  fbo->xres=res;
  fbo->yres=res;
  
  glGenFramebuffersEXT(1, &fbo->num);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->num);
  
  // depth  
  glGenRenderbuffersEXT(1, &fbo->depthbuffer);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo->depthbuffer);  
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, res, res);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->depthbuffer);

  // colortexture
  glGenTextures(1, &fbo->texnum);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, fbo->texnum);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
  for (x=0; x<6; x++) {
    glTexImage2D(cubesides[x], 0, GL_RGBA, res, res, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);  
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, cubesides[x], fbo->texnum, 0);
  }
   
  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  fbo->texnum2=0;

  glueLoading();
   
  return fbo;
}




glue_static void glueRendercubemap(Framebuffer *fbo, skene *s, float t, Vertex pos, Vertex tgt, float zmin, float zmax, int maxface) {
  int x;
  Vertex lookat;
  	
	const float lookz[6][6] = {
		{  0.0,  0.0,  1.0,  0.0, -1.0,  0.0 },
		{  1.0,  0.0,  0.0,  0.0, -1.0,  0.0 },
		{ -1.0,  0.0,  0.0,  0.0, -1.0,  0.0 },
		{  0.0,  1.0,  0.0,  0.0,  0.0,  1.0 },
		{  0.0, -1.0,  0.0,  0.0,  0.0, -1.0 },
		{  0.0,  0.0, -1.0,  0.0, -1.0,  0.0 }
	};	

  lookat.x=tgt.x-pos.x;
  lookat.y=tgt.y-pos.y;
  lookat.z=tgt.z-pos.z;
  normalize(&lookat);

  for (x=0; x<maxface; x++) {

    glueBeginrender(fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, cubesides[x], fbo->texnum, 0);
    //gluePrepareflags2(black);
  	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 1, zmin, zmax);
  	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
		gluLookAt(0, 0, 0, lookz[x][0], lookz[x][1], lookz[x][2], lookz[x][3], lookz[x][4], lookz[x][5]);
    glScalef(1, 1, -1);
    gluLookAt(pos.x, pos.y, pos.z, tgt.x, tgt.y, tgt.z, 0, 1, 0);
    s(t, t, 0, &x, 2|4);
    //checkfbostatus();
    glueEndrender();
  }

  
}



/* ei taida toimia ihan oikein eikä jaksa korjata... * /

void glueCubemap_texmatrix(int tu, Camera *cam) {
  static float m[16];
  

  glMatrixMode(GL_MODELVIEW);
  
  glPushMatrix();
  glLoadIdentity();  

  gluLookAt(
    cam->pos.x,
    cam->pos.y,
    cam->pos.z,
    cam->tgt.x,
    cam->tgt.y,
    cam->tgt.z,
    cam->yvec.x,
    cam->yvec.y,
    cam->yvec.z);

	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glPopMatrix();
	
	invertmatrix(m);
	
	glMatrixMode(GL_TEXTURE);
  glActiveTexture(tu);	
	glMultMatrixf(m);
  glActiveTexture(GL_TEXTURE0_ARB);	
  
  glMatrixMode(GL_MODELVIEW);
}
*/



