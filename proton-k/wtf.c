


void initvtf() {
	int maxunits;

  #ifdef GLUE_FLOAT_TEXTURE
  if (!checkext("GL_ARB_texture_float")) glueError("Seems your gpu is not supporting GL_ARB_texture_float");
  #endif

  #ifdef GLUE_VTF
  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxunits);
  if (maxunits<1) glueError("Your GPU does not support vertex texture fetch");
  #endif

}



#ifdef GLUE_FLOAT_TEXTURE
#ifdef GLUE_VTF

Framebuffer *glueVertextofbo(Vertex *vertices, int vc, Vertex **texcoords) {
  Framebuffer *fbo=malloc(sizeof(Framebuffer));
  float *temppi;
  int x, y, i, xres, yres;
  Vertex *tex;

  xres=yres=ceil(sqrt(vc));
  temppi=malloc(sizeof(float)*xres*yres*4);

  fbo->xres=xres;
  fbo->yres=yres;
  fbo->texnum2=0;

  float min=0.0, max=0.0;
  i=0;
  for (x=0; x<vc; x++) {
  	float a;
    temppi[i*4+0]=vertices[x].x;
    temppi[i*4+1]=vertices[x].y;
    temppi[i*4+2]=vertices[x].z;
    a=fmod(atan2(vertices[x].x, vertices[x].z)/pi+1.125, 1.0)*1.5;
    a+=(1.0-(vertices[x].y-90.0)/145.5)*2.0+ranf()*0.5;
    //a=fabs(vertices[x].y-162.0)+ranf()*0.5;
    //a=v_len2(vertices[x])-493.0;
    //a=vertices[x].x>vertices[x].z?vertices[x].x:vertices[x].z;
    //a=sqrt(a*a+vertices[x].y*vertices[x].y);
    //a=a/200.0;
    temppi[i*4+3]=a;
    if (a<min) min=a;
    if (a>max) max=a;
    i++;
  }
 // for (x=0; x<vc; x++) temppi[i*4+3]/=max;
  //glueNoticef("min:%5.2f max:%5.2f", min, max);

  tex=malloc(sizeof(Vertex)*vc);
  for (y=0; y<yres; y++) for (x=0; x<xres; x++) {
  	int i=y*xres+x;
  	float halfx=0.5/xres;
  	float halfy=0.5/yres;
    if (i<vc)	tex[i]=new_v((float)x/(float)xres+halfx, (float)y/(float)yres+halfy, 0.0);
  }
  texcoords[0]=tex;


  // generate fbo
  glGenFramebuffersEXT(1, &fbo->num);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->num);

  // depth
  glGenRenderbuffersEXT(1, &fbo->depthbuffer);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo->depthbuffer);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, xres, yres);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->depthbuffer);

  // laddader texturedata
  glGenTextures(1, &fbo->texnum);
  glBindTexture(GL_TEXTURE_2D, fbo->texnum);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAPS, GL_FALSE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, xres, yres, 0, GL_RGBA, GL_FLOAT, temppi);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo->texnum, 0);


  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glueLoading();

  free(temppi);
  return fbo;
}

#endif
#endif



#ifdef GLUE_FLOAT_TEXTURE
Framebuffer *glueFramebuffer_format(int xres, int yres, int intformat, int format, int type, int flags) {
  Framebuffer *fbo=malloc(sizeof(Framebuffer));

  fbo->xres=xres;
  fbo->yres=yres;
  fbo->texnum2=0;

  // generate fbo
  glGenFramebuffersEXT(1, &fbo->num);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->num);

  // depth
  glGenRenderbuffersEXT(1, &fbo->depthbuffer);
  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fbo->depthbuffer);
  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, xres, yres);
  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fbo->depthbuffer);

  // color
  glGenTextures(1, &fbo->texnum);
  glBindTexture(GL_TEXTURE_2D, fbo->texnum);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAPS, GL_FALSE);
  glTexImage2D(GL_TEXTURE_2D, 0, intformat, xres, yres, 0, format, type, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo->texnum, 0);

  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glueLoading();

  return fbo;
}


Framebuffer *glueFramebuffer2_format(int xres, int yres, int intformat, int format, int type, int intformat2, int format2, int type2, int flags) {
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
  glTexImage2D(GL_TEXTURE_2D, 0, intformat, xres, yres, 0, format, type, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo->texnum, 0);

  // colortexture 2
  glGenTextures(1, &fbo->texnum2);
	glBindTexture(GL_TEXTURE_2D, fbo->texnum2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, intformat2, xres, yres, 0, format2, type2, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, fbo->texnum2, 0);

  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glueLoading();

  return fbo;
}

Framebuffer *glueFramebuffer2_format_data(int xres, int yres, int intformat, int format, int type, int intformat2, int format2, int type2, float *data1, float *data2, int flags) {
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
  glTexImage2D(GL_TEXTURE_2D, 0, intformat, xres, yres, 0, format, type, data1);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo->texnum, 0);

  // colortexture 2
  glGenTextures(1, &fbo->texnum2);
	glBindTexture(GL_TEXTURE_2D, fbo->texnum2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, intformat2, xres, yres, 0, format2, type2, data2);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, fbo->texnum2, 0);

  checkfbostatus();
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

  glueLoading();

  return fbo;
}
#endif



////////////////////////////////////  VBO SHOIT

PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;


void init_vbo() {
	if (!checkext( "GL_ARB_vertex_buffer_object" )) glueError("GPU does not support vbo's");

	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");

  if (!glGenBuffersARB || !glBindBufferARB || !glBufferDataARB || !glDeleteBuffersARB) glueError("error getting VBO procaddresses");
}

/////////////////////////////////// POINT SPRITES

void init_point_sprite() {
	if (!checkext("GL_ARB_point_sprite")) glueError("Your GFX card must be from the 90s or made by Intel, since ARB_point_sprite is not supported");
}


///////////////////////////////////////




