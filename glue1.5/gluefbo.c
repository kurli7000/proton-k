
#ifndef FORGET_INCLUDES
#include "gluefbo.h"
#endif

#define STACKSIZE 256
static int currentfbo=0;
static Framebuffer *fbostack[STACKSIZE];
static Framebuffer *screentarget;

PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT= NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

PFNGLDRAWBUFFERSPROCGLUE glDrawBuffersARB = NULL;



glue_static void glueInitfbo() {

  if (!checkext("GL_EXT_framebuffer_object")) glueError("GL_EXT_framebuffer_object not supported, KTHXBYE!");

  glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC) glueGetProcAddress("glIsRenderbufferEXT");
  glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) glueGetProcAddress("glBindRenderbufferEXT");
  glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) glueGetProcAddress("glDeleteRenderbuffersEXT");
  glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) glueGetProcAddress("glGenRenderbuffersEXT");
  glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) glueGetProcAddress("glRenderbufferStorageEXT");
  glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) glueGetProcAddress("glGetRenderbufferParameterivEXT");
  glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) glueGetProcAddress("glIsFramebufferEXT");
  glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) glueGetProcAddress("glBindFramebufferEXT");
  glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) glueGetProcAddress("glDeleteFramebuffersEXT");
  glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) glueGetProcAddress("glGenFramebuffersEXT");
  glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) glueGetProcAddress("glCheckFramebufferStatusEXT");
  glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC) glueGetProcAddress("glFramebufferTexture1DEXT");
  glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) glueGetProcAddress("glFramebufferTexture2DEXT");
  glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC) glueGetProcAddress("glFramebufferTexture3DEXT");
  glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) glueGetProcAddress("glFramebufferRenderbufferEXT");
  glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) glueGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
  glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC) glueGetProcAddress("glGenerateMipmapEXT");

  if (!glGenRenderbuffersEXT) glueError("Error procaddessing something related to FBO");

	if (!checkext("GL_ARB_draw_buffers")) glueError("GL_ARB_draw_buffers not supported, go fuck yourself");
	glDrawBuffersARB = (PFNGLDRAWBUFFERSPROCGLUE)wglGetProcAddress("glDrawBuffersARB");
	

  screentarget=malloc(sizeof(Framebuffer));
  screentarget->num=0;
  screentarget->texnum=0;
  screentarget->texnum2=0;
  screentarget->dtexnum=0;
  screentarget->depthbuffer=0;
  screentarget->xres=glueXres;
  screentarget->yres=glueYres;
  fbostack[0]=screentarget;
}

glue_static void checkfbostatus() {
  GLenum status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

  switch(status) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
      glueError("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n");
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      glueError("GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
      glueError("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      glueError("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      glueError("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      glueError("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      glueError("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n");
      break;
    default:
      glueError("UNKNWON\n");
  }
}


void glueCheckfbo(Framebuffer *fbo) {
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->num);
  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbostack[currentfbo]->num);
}



glue_static Framebuffer *glueFramebuffer(int xres, int yres, int flags) {
  Framebuffer *fbo;

  fbo=malloc(sizeof(Framebuffer));

  fbo->xres=xres;
  fbo->yres=yres;
  fbo->texnum2=0;
  fbo->dtexnum=0;

  glGenFramebuffersEXT(1, &fbo->num);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->num);

  // depth
  glGenRenderbuffersEXT(1, &fbo->depthbuffer);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo->depthbuffer);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, xres, yres);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->depthbuffer);

  // colortexture
  glGenTextures(1, &fbo->texnum);
	glBindTexture(GL_TEXTURE_2D, fbo->texnum);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  if (!(flags&GLUE_NO_MIPMAP))
    glGenerateMipmapEXT(GL_TEXTURE_2D);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo->texnum, 0);


  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glueLoading();

  return fbo;
}


glue_static Framebuffer *glueFramebuffer2(int xres, int yres, int flags) {
  Framebuffer *fbo;

  fbo=malloc(sizeof(Framebuffer));

  fbo->xres=xres;
  fbo->yres=yres;

  glGenFramebuffersEXT(1, &fbo->num);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->num);

  // depth
  glGenRenderbuffersEXT(1, &fbo->depthbuffer);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo->depthbuffer);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, xres, yres);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->depthbuffer);

  // colortexture
  glGenTextures(1, &fbo->texnum);
	glBindTexture(GL_TEXTURE_2D, fbo->texnum);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  if (!(flags&GLUE_NO_MIPMAP))
    glGenerateMipmapEXT(GL_TEXTURE_2D);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo->texnum, 0);
 
  // colortexture 2
  glGenTextures(1, &fbo->texnum2);
	glBindTexture(GL_TEXTURE_2D, fbo->texnum2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  if (!(flags&GLUE_NO_MIPMAP))
    glGenerateMipmapEXT(GL_TEXTURE_2D);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, fbo->texnum2, 0);


  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glueLoading();

  return fbo;
}





glue_static void glueViewport(void) {

  if (currentfbo>0) {
    glViewport(0, 0, fbostack[currentfbo]->xres, fbostack[currentfbo]->yres);
  } else {
    glViewport(0, (int)(glueYres*(1.0-glueWidescreen)*0.5), glueXres, (int)(glueYres*glueWidescreen));
  }

}




void glueBeginrender(Framebuffer *fbo) {

  if (!fbo) glueError("ouch fuck shit fbo");

  currentfbo++;
  if (currentfbo>=STACKSIZE) glueError("fbo stack overflow");

  fbostack[currentfbo]=fbo;
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbostack[currentfbo]->num);

  if (fbo->texnum && fbo->texnum2) {
    GLenum buffers[2] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
    glDrawBuffersARB(2, buffers);
  } else if (fbo->texnum) {
    GLenum buffers[2] = { GL_COLOR_ATTACHMENT0_EXT, GL_NONE };
    glDrawBuffersARB(2, buffers);
  } else {
    GLenum buffers[2] = { GL_NONE, GL_NONE };
    glDrawBuffersARB(2, buffers);
  }
  
  glueViewport();
}

void glueBeginrender1(Framebuffer *fbo) {

  if (!fbo) glueError("ouch fuck shit fbo");
  	
  currentfbo++;
  if (currentfbo>=STACKSIZE) glueError("fbo stack overflow");

  fbostack[currentfbo]=fbo;
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbostack[currentfbo]->num);

  GLenum buffers[2] = { GL_COLOR_ATTACHMENT0_EXT, GL_NONE };
  glDrawBuffersARB(2, buffers);
 
  glueViewport();
}

void glueBeginrender2(Framebuffer *fbo) {

  if (!fbo) glueError("ouch fuck shit fbo");
  if (!fbo->texnum2) glueError("you fucking stupid cunt!");
  	
  currentfbo++;
  if (currentfbo>=STACKSIZE) glueError("fbo stack overflow");

  fbostack[currentfbo]=fbo;
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbostack[currentfbo]->num);

  GLenum buffers[2] = { GL_COLOR_ATTACHMENT1_EXT, GL_NONE };
  glDrawBuffersARB(2, buffers);
 
  glueViewport();
}



void glueEndrender() {

  if (currentfbo<=0) glueError("fbo stack underflow");
  currentfbo--;

  glFlush();
  
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbostack[currentfbo]->num);
  glueViewport();
}


// ykköstargetin versiot

glue_static void glueOverlayfbo(Framebuffer *fbo, float br, int flags) {

  gluePush();

  glueSet2d(flags);
  glueDisabletexture();
  glueBindtexture_fbo(fbo, GL_TEXTURE0_ARB, flags|GLUE_CLAMP|GLUE_NO_MIPMAP, 0, 0);

  if (flags&GLUE_BLEND_ALPHA) glColor4f(1, 1, 1, br);
  else glColor4f(br, br, br, br);

  glue2dquad();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  gluePop();
}


glue_static void glueOverlayfbo_color(Framebuffer *fbo, float *col, int flags) {

  gluePush();

  glueSet2d(flags);
  glueDisabletexture();
  glueBindtexture_fbo(fbo, GL_TEXTURE0_ARB, flags|GLUE_CLAMP|GLUE_NO_MIPMAP, 0, 0);

  glColor4f(col[0], col[1], col[2], col[3]);

  glue2dquad();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  gluePop();
}

glue_static void glueBindtexture_fbo(Framebuffer *fbo, int tu, int flags, int blendflags, int alphablendflags) {

  glActiveTexture(tu);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fbo->texnum);
  texflags(flags);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

}

// kakkostargetin versiot

glue_static void glueOverlayfbo2(Framebuffer *fbo, float br, int flags) {

  gluePush();

  glueSet2d(flags);
  glueDisabletexture();
  glueBindtexture_fbo2(fbo, GL_TEXTURE0_ARB, flags|GLUE_CLAMP|GLUE_NO_MIPMAP, 0, 0);

  if (flags&GLUE_BLEND_ALPHA) glColor4f(1, 1, 1, br);
  else glColor4f(br, br, br, br);

  glue2dquad();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  gluePop();
}


glue_static void glueOverlayfbo2_color(Framebuffer *fbo, float *col, int flags) {

  gluePush();

  glueSet2d(flags);
  glueDisabletexture();
  glueBindtexture_fbo2(fbo, GL_TEXTURE0_ARB, flags|GLUE_CLAMP|GLUE_NO_MIPMAP, 0, 0);

  glColor4f(col[0], col[1], col[2], col[3]);

  glue2dquad();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  gluePop();
}


glue_static void glueBindtexture_fbo2(Framebuffer *fbo, int tu, int flags, int blendflags, int alphablendflags) {

  glActiveTexture(tu);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fbo->texnum2);
  texflags(flags);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

}

#if GLUE_USE_ARBSHADOW==1
glue_static void glueOverlayfbo_depth(Framebuffer *fbo, float br, int flags) {

  gluePush();

  glueSet2d(flags);
  glueDisabletexture();

  glActiveTexture(GL_TEXTURE0_ARB);
  glClientActiveTexture(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, fbo->dtexnum);
  texflags(flags|GLUE_CLAMP|GLUE_NO_MIPMAP);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  glColor4f(br, br, br, br);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE );
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
  glue2dquad();

  gluePop();
}
#endif



