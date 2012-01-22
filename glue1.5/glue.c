
#ifndef FORGET_INCLUDES

#include "glue.h"

#ifdef I_LOVE_BILL
#include "gluewin.h"
#else
#include "gluelin.h"
#endif

#include "gluetxtr.h"
#include "glueext.h"
#include "gluegen.h"
#include "glueobj.h"
#include "glueovrl.h"
#include "gluefbo.h"


#endif //FORGET_INCLUDES

int eftexture_xsize=0;
int eftexture_ysize=0;
int glueFC=0;
int glueVC=0;
int glueXres;
int glueYres;
int glueFullscreen;
int glueLoop;
int glueMultisample;
float glueAspect=1.2;
//float glueWidescreen=(1280/768.)/(16.0/9.0);
float glueWidescreen=(4.0/3.0)/(16.0/9.0);
int glueSbits=0;
int glueShowinformation=0;
int glueShowdebug=0;
int glueLoadOK=0;
Texture *debugtexture;
Texture *infotexture;
int glueLoadsteps=0;
Vertex glueMousepos={0, 0, 0};
int glueMousekey=0;
Vertex glueGolden={0.618*2.0-1.0, -(0.618*2.0-1.0), 0};
float glueGamma=1.0/3.0;
int tinydebugline=0;
int tinyinfoline=0;
int glueTicktable[1024];
int glueTicktablepos=0;
float black[]={0, 0, 0, 1};
float white[]={1, 1, 1, 1};
float gray[]={0.5, 0.5, 0.5, 1};
Texture *glueDummytex=0;

glueLoadproc *glueLoadingbarproc=0;


void *failmalloc(int z) {
  void *p = (malloc)(z); 
  if (!p) glueError("malloc fail");
  return p; 
}



glue_static void testit(void) {
  int rbits, gbits, bbits, sbits, abits;
  char *version;
  int minor, major;

  glGetIntegerv(GL_RED_BITS, &rbits);
  glGetIntegerv(GL_GREEN_BITS, &gbits);
  glGetIntegerv(GL_BLUE_BITS, &bbits);
  glGetIntegerv(GL_ALPHA_BITS, &abits);
  glGetIntegerv(GL_STENCIL_BITS, &sbits);

#ifdef HIDE_KEWLERS
  if (abits==0) glueError("Failed to initialize 32bpp mode (framebuffer alpha channel is required)");
#else
  if (abits==0) glueError("Failed to initialize 32bpp mode, buy some decent hardware asshole!");
#endif


#if GLUE_USE_SHADOWS==1

#ifdef HIDE_KEWLERS
  if (sbits==0) glueError("Failed to initialize stencil buffer, which is required for shadows");
#else
  if (sbits==0) glueError("Failed to initialize stencil buffer, buy some decent hardware asshole!");
#endif

#endif // USE_SHADOWS

/*
  version=(char *)glGetString(GL_VERSION);

  #ifndef GL_CLAMP_TO_EDGE
  if (sscanf(version, "%d.%d", &major, &minor) == 2) {
    if (major*10+minor>=12) {
      GL_CLAMP_TO_EDGE = 0x812F;
    } else {
      GL_CLAMP_TO_EDGE = GL_CLAMP;
    }
  } else {
    GL_CLAMP_TO_EDGE = GL_CLAMP;
  }
  #endif
  */
  
  
}


glue_static void glueLoading(void) {
  int x;

#if GLUE_DEVEL_MODE==1  
  if (GetAsyncKeyState(VK_ESCAPE)) glueError("you esced");
#endif

  if (!glueLoadOK) {

    if (glueLoadingbarproc) { // show custom loading bar thingy
    
      glueLoadingbarproc((float)glueLoadsteps/GLUELOADSTEPS);
      
    } else { // show default horizontal bar
    
      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT);

      glueViewport();
 
      glColor4f(1, 1, 1, 1);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(-1, 1, -1, 1);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glDisable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glDisable(GL_FOG);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);

      glBegin(GL_QUADS);
      glVertex2f(-1, 0.05);
      glVertex2f(-1+(float)glueLoadsteps/GLUELOADSTEPS*2, 0.05);
      glVertex2f(-1+(float)glueLoadsteps/GLUELOADSTEPS*2, -0.05);
      glVertex2f(-1, -0.05);
      glEnd();
    }

    glueSwap();
    glueLoadsteps=glueLoadsteps+1;
  }
}



/**********************************************************************
  Initial stuff (engine & per frame)
**********************************************************************/


glue_static void glueBackground(float *col, int flags) {

  gluePush();
  glueSet2d(flags);
  glueDisabletexture();

  glColor4f(col[0], col[1], col[2], col[3]);

  glBegin(GL_POLYGON);
  glVertex2f(-1,  1);
  glVertex2f( 1,  1);
  glVertex2f( 1, -1);
  glVertex2f(-1, -1);
  glEnd();

  gluePop();
}


glue_static void glueClear(float *color) {
  static int frame=0;
/*
  if (glueLoadOK==1 && frame++<8) {
    glViewport(0, 0, glueXres, glueYres);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
  }
*/
  glueViewport();

  glClearColor(0, 0, 0, 0);
  glClearDepth(1.0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

#if GLUE_USE_SHADOWS==1
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
#endif

  if (color!=black) {

    gluePush();
    glueSetmatrices2d(0);

    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glColor4f(color[0], color[1], color[2], color[3]);

    glue2dquad();

  	gluePop();
  }
}


glue_static void gluePrepareflags(float *col) {
  float amb[]={0, 0, 0, 1.0};
  float dif[]={0.8, 0.8, 0.8, 1.0};
  float spec[]={0.0, 0.0, 0.0, 1.0};
//  static int frame=0;

  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_FOG);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDisable(GL_TEXTURE_GEN_Q);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  //glDisable(GL_TEXTURE_1D);
  //glDisable(GL_TEXTURE_2D);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);
  glDisable(GL_AUTO_NORMAL);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_ALWAYS, 0);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_DITHER);
  glDisable(GL_LINE_SMOOTH);
  //glEnable(GL_POLYGON_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_LINE_STIPPLE);
  glDisable(GL_LOGIC_OP);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);
  glDisable(GL_CLIP_PLANE3);
  glDisable(GL_CLIP_PLANE4);
  glDisable(GL_CLIP_PLANE5);
  glColor4f(1, 1, 1, 1);
  glDepthMask(1);
  glColorMask(1, 1, 1, 1);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, spec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
  //glDisable(GL_LIGHT_MODEL_LOCAL_VIEWER);
  //glDisable(GL_LIGHT_MODEL_TWO_SIDE);

 // if (GLUE_CUBEMAP_OK) {
 //   glDisable(GL_TEXTURE_CUBE_MAP_ARB);
 // }
  glueDisabletexture();

  if (glueMultisample==1) glEnable(GL_MULTISAMPLE_ARB);


  if (col!=0) glueClear(col);

  //glueFC=0;
  //glueVC=0;

  tinydebugline=0;
  tinyinfoline=0;
}


glue_static void gluePrepareflags2(float *col) {

  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  renderflags(0);
  glueDisabletexture();

  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);

  glColor4f(1, 1, 1, 1);
  glColorMask(1, 1, 1, 1);

  if (col!=0) glueClear(col);

  //glueFC=0;
  //glueVC=0;
  tinydebugline=0;
  tinyinfoline=0;
}



glue_static void glueSetmatrices2d(int flags) {

  glueViewport();

//  glMatrixMode(GL_TEXTURE);
//  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, 1.0, -1.0);
  //gluOrtho2D(-1, 1, 1, -1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

glue_static void glueSetmatrices3d(float perspective, Vertex cen, float znear, float zfar, float tilt, int flags) {

  glueViewport();

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glTranslatef(cen.x, cen.y, 0);
  gluPerspective(perspective, glueAspect/glueWidescreen, znear, zfar);
  glRotatef(tilt, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


glue_static Camera glueDefaultcamera(float dist) {
  Camera c;

  c.pos=new_v(0, 0, -dist);
  c.tgt=new_v(0, 0, 0);
  c.yvec=new_v(0, 1, 0);
  c.cen=new_v(0, 0, 0);
  c.znear=0.1;
  c.zfar=1000.0;
  c.per=90;
  c.tilt=0;

  return c;
}

glue_static Camera glueDefaultcamera2(float t, float dist) {
  Camera c;

  c.pos=new_v(sin(t*pi)*dist, 0, cos(t*pi)*dist);
  c.tgt=new_v(0, 0, 0);
  c.yvec=new_v(0, 1, 0);
  c.cen=new_v(0, 0, 0);
  c.znear=0.1;
  c.zfar=1000.0;
  c.per=90;
  c.tilt=0;

  return c;
}

glue_static void glueCamera(Camera *cam, int flags) {
/*
  if (cam->znear<=0 && cam->zfar<=0) {
    Vertex l=new_v(cam->tgt.x-cam->pos.x, cam->tgt.y-cam->pos.y, cam->tgt.z-cam->pos.z);
    float len=sqrt(l.x*l.x+l.y*l.y+l.z*l.z)*2.0;

    cam->znear=len*0.001;
    cam->zfar=len;
  }
*/
/*
  if (fabs(cam->per)<=0) {
    glueSetmatrices3d(90, flags);
  } else {
    glueSetmatrices3d(cam->per, flags);
  }
*/
  glueSetmatrices3d(cam->per, cam->cen, cam->znear, cam->zfar, cam->tilt, flags);
  //glueSetmatrices3d(float perspective, Vertex cen, float znear, float zfar, float tilt, int flags) {

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

}


glue_static void gluePrinterrors(void) {
  GLenum erreur;
  char *err=0;

  erreur=glGetError();
       if (erreur==GL_NO_ERROR)          err="GL_NO_ERROR";
  else if (erreur==GL_INVALID_ENUM)      err="GL_INVALID_ENUM";
  else if (erreur==GL_INVALID_VALUE)     err="GL_INVALID_VALUE";
  else if (erreur==GL_INVALID_OPERATION) err="GL_INVALID_OPERATION";
  else if (erreur==GL_STACK_OVERFLOW)    err="GL_STACK_OVERFLOW";
  else if (erreur==GL_STACK_UNDERFLOW)   err="GL_STACK_UNDERFLOW";
  else if (erreur==GL_OUT_OF_MEMORY)     err="GL_OUT_OF_MEMORY";
  //else if (erreur==GL_TABLE_TOO_LARGE)    err="GL_TABLE_TOO_LARGE";
  //else if (erreur==GL_INVALID_FRAMEBUFFER_OPERATION) err="GL_INVALID_FRAMEBUFFER_OPERATION";
  else sprintf(err, "UNKNOWN GL_ERROR #%i", erreur);

  if (erreur!=GL_NO_ERROR) glueErrorf("OpenGL error: %s", err);
//  if (erreur!=GL_NO_ERROR) glueError(err);
}




glue_static void gluePush(void) {

  glPushAttrib(
    GL_ACCUM_BUFFER_BIT|    // accum clear color
//    GL_COLOR_BUFFER_BIT|  // alpha_test/blend/dither/draw_buffer/logic_op
    GL_CURRENT_BIT|         // rgba/normal/texcoord/rasterpos
    GL_DEPTH_BUFFER_BIT|    // depth_test enable, func, value
    GL_ENABLE_BIT|          // alpha_test,auto_normal,blend,color_material,cull_face,depth_test,dither,fog,lightX,lighting,stipple/smooths,scissor_test,stencil_test,texture_Xd,texture_gen
//    GL_EVAL_BIT|
    GL_FOG_BIT|             // fog, color, density, end, start, mode
    GL_HINT_BIT|            // perspective_hint, smooth-flag, fog_hint
    GL_LIGHTING_BIT|        // kaikkien materiaalien ja valojen kaikki värit/sijainti, gl_shade_model
    //GL_LINE_BIT|          // line_smooth/stipple
    GL_LIST_BIT|            // list_base
    GL_PIXEL_MODE_BIT|      // X_BIAS, X_SCALE, X_OFFSET (r/g/b), zoom, index etc flags
    GL_POINT_BIT|           // point_smooth, point size
    GL_POLYGON_BIT|         // cullling, polygon_smooth/stipple
    //GL_POLYGON_STIPPLE_BIT|
    GL_SCISSOR_TEST|        // scissor test/box
    GL_STENCIL_BUFFER_BIT|  // stencil bit, mask, func fail/pass etc, buffer clear, writemask
    GL_TEXTURE_BIT|         // min/maxific bits for each texture, coordinates, wrapmodes, texture_gen_X, gltexgen equations
    //GL_TRANSFORM_BIT|     // clipping planes, matrix_mode, normalize
    GL_VIEWPORT_BIT);       // viewport, near, far

  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

}

glue_static void gluePop(void) {

  glPopAttrib();

  glMatrixMode(GL_TEXTURE);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


/**********************************************************************
  Other usefull stuph
**********************************************************************/



glue_static void glueFog(float *color, float min, float max) {
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_DENSITY, 1.0);
  glFogfv(GL_FOG_COLOR, color);
  glFogf(GL_FOG_START, min);
  glFogf(GL_FOG_END, max);
  //glHint(GL_FOG_HINT, GL_NICEST);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
}

//kimmo toi GL_FOG_HINT, GL_DONT_CARE vetää shitit ihan vituilleen radeonilla. alla 'toimiva' toteutus :)

glue_static void glueFog_toimiva(float *color, float min, float max) {
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_DENSITY, 1.0);
  glFogfv(GL_FOG_COLOR, color);
  glFogf(GL_FOG_START, min);
  glFogf(GL_FOG_END, max);
  glHint(GL_FOG_HINT, GL_NICEST);
}


glue_static void glueFog2(float *color, float power, float min, float max) {
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_EXP);
  glFogf(GL_FOG_DENSITY, power);
  glFogfv(GL_FOG_COLOR, color);
  glFogf(GL_FOG_START, min);
  glFogf(GL_FOG_END, max);
//  glHint(GL_FOG_HINT, GL_NICEST);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
}


glue_static void glueLight_hard(Vertex *dir, float *color) {
  float lpos[4];
  float amb[4]={0, 0, 0, 1};

  lpos[0]=dir->x;
  lpos[1]=dir->y;
  lpos[2]=dir->z;
  lpos[3]=0;

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
  glLightfv(GL_LIGHT0, GL_SPECULAR, color);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
}


glue_static void glueLight_hard_nospec(Vertex *dir, float *color) {
  float lpos[4];
  float amb[4]={0, 0, 0, 1};
  float emi[4]={0, 0, 0, 1};

  lpos[0]=dir->x;
  lpos[1]=dir->y;
  lpos[2]=dir->z;
  lpos[3]=0;

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
  glLightfv(GL_LIGHT0, GL_SPECULAR, color);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}


glue_static void glueLight_normal(Vertex *dir, float *color) {
  float lpos[4];
  float amb[]={color[0]*0.2, color[1]*0.2, color[2]*0.2, 1.0};

  lpos[0]=dir->x;
  lpos[1]=dir->y;
  lpos[2]=dir->z;
  lpos[3]=0;

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
  glLightfv(GL_LIGHT0, GL_SPECULAR, black);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0);
}


glue_static void glueLight_soft(Vertex *dir, float *color) {
  float lpos[4];
  float amb[]={color[0]*0.5, color[1]*0.5, color[2]*0.5, 1.0};

  lpos[0]=dir->x;
  lpos[1]=dir->y;
  lpos[2]=dir->z;
  lpos[3]=0;

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
  glLightfv(GL_LIGHT0, GL_SPECULAR, color);
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}


glue_static void glueLight_pos(Vertex *pos, float *color, float maxdist) {
  float lpos[4];

  lpos[0]=pos->x;
  lpos[1]=pos->y;
  lpos[2]=pos->z;
  lpos[3]=1.0;

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lpos);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
  glLightfv(GL_LIGHT0, GL_AMBIENT, black);
  glLightfv(GL_LIGHT0, GL_SPECULAR, color);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0/maxdist);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, black);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
}


glue_static void glueStrobo(float t, float br, float times, float fadespeed) {
  float br2;

  br2=clamp(1.0-fmod(t, 1.0/times)*times*fadespeed)*br;

  glueSetmatrices2d(0);

  glDisable(GL_FOG);
  glueDisabletexture();
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  glColor4f(br2, br2, br2, 1);

  glBegin(GL_POLYGON);
  glVertex2f(-1,  1);
  glVertex2f( 1,  1);
  glVertex2f( 1, -1);
  glVertex2f(-1, -1);
  glEnd();

}

glue_static float glueStrobof(float t, float br, float times, float fadespeed) {
  return clamp(1.0-fmod(t, 1.0/times)*times*fadespeed)*br;
}


glue_static void glueShowzbuffer(Texture *tex, float scale) {
  //float *imgf=malloc(sizeof(float)*eftexture_xsize*eftexture_ysize);
  unsigned char *img=malloc(eftexture_xsize*eftexture_ysize*4);
  int x;

  glPixelTransferf(GL_DEPTH_SCALE, scale);
  glPixelTransferf(GL_DEPTH_BIAS, -(scale-1));
//  glPixelTransferf(GL_DEPTH_SCALE, 1);
//  glPixelTransferf(GL_DEPTH_BIAS, 0);
  glReadPixels(0, 0, glueXres, glueYres, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, img);
//  glReadPixels(0, 0, effect_xsize, effect_ysize, GL_DEPTH_COMPONENT, GL_FLOAT, imgf);
/*
  for (x=0; x<effect_xsize*effect_ysize; x++) {
    unsigned char joo=pow(1.0-imgf[x], ramp)*255;
    img[x*4+0]=img[x*4+1]=img[x*4+2]=img[x*4+3]=joo;
  }
*/
  //for (x=effect_xsize*effect_ysize*4-1; x>=0; x--) img[x]=pow(1.0-img[x>>2]/255.0, ramp)*255;
  for (x=glueXres*glueYres*4-1; x>=0; x--) img[x]=img[x>>2];

  glueReloadtexture(tex, img);

  gluePrepareflags(black);
  glueOverlay(tex, 1.0, GLUE_NO_CULL|GLUE_NO_DEPTH);

  free(img);
  //free(imgf);
}

glue_static void glueShowalphabuffer(void) {

  glueSet2d(0);
  //glDisable(GL_TEXTURE_2D);
  glueDisabletexture();
  glColor4f(1, 1, 1, 1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_DST_ALPHA, GL_ZERO);

  glue2dquad();
}


glue_static void glueResetalpha(float c, int flags) {

  glueSet2d(flags);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glColor4f(1, 1, 1, c);

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);

  glBegin(GL_QUADS);
  glVertex2f(-1, -1);
  glVertex2f( 1, -1);
  glVertex2f( 1,  1);
  glVertex2f(-1,  1);
  glEnd();
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}


glue_static void glueColoroverlay(float *col, int flags) {

  glueSet2d(flags);
  glDisable(GL_TEXTURE_2D);
  glColor4f(col[0], col[1], col[2], col[3]);

  glBegin(GL_QUADS);
  glVertex2f(-1, -1);
  glVertex2f( 1, -1);
  glVertex2f( 1,  1);
  glVertex2f(-1,  1);
  glEnd();
}



glue_static void glue2dquad(void) {
  glBegin(GL_QUADS);

  glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 0); glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 0); glVertex2f(-1, -1);
  glMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 0); glMultiTexCoord2f(GL_TEXTURE1_ARB, 1, 0); glVertex2f( 1, -1);
  glMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 1); glMultiTexCoord2f(GL_TEXTURE1_ARB, 1, 1); glVertex2f( 1,  1);
  glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 1); glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 1); glVertex2f(-1,  1);

  glEnd();
}

glue_static void glue2dquad2(void) {
  glBegin(GL_QUADS);

  glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 1); glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 1); glVertex2f(-1, -1);
  glMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 1); glMultiTexCoord2f(GL_TEXTURE1_ARB, 1, 1); glVertex2f( 1, -1);
  glMultiTexCoord2f(GL_TEXTURE0_ARB, 1, 0); glMultiTexCoord2f(GL_TEXTURE1_ARB, 1, 0); glVertex2f( 1,  1);
  glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 0); glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 0); glVertex2f(-1,  1);
  
  glEnd();
}

glue_static void glueBackgroundcolor(float r, float g, float b, float a, int flags) {

  gluePush();
  glueSet2d(flags);
  glueDisabletexture();
  renderflags(flags);
  glColor4f(r, g, b, a);

  glBegin(GL_QUADS);
  glVertex2f(-1, -1);
  glVertex2f( 1, -1);
  glVertex2f( 1,  1);
  glVertex2f(-1,  1);
  glEnd();

  gluePop();
}


glue_static void glueWriteticktable(void) {
  int x;
  FILE *f;

  f=fopen("ticks.inc", "w");

  fprintf(f, "static int ticktable[]={\n");
  for (x=0; x<glueTicktablepos; x++) {
    fprintf(f, " 64*%i + %i,\n", glueTicktable[x]>>6, glueTicktable[x]%64);
  }
  fprintf(f, "};\n");

  fclose(f);
}



glue_static void glueCross(float len) {

  gluePush();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_FOG);

  glBegin(GL_LINES);

  glColor3f(1, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(len, 0, 0);

  glColor3f(0, 1, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, len, 0);

  glColor3f(0, 0, 1);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, len);

  glEnd();

  gluePop();
}

glue_static void glueShowlevels(float *levels, float *averages, float threshold, float scale) {
  int x;

  glueSetmatrices2d(0);
  glDisable(GL_FOG);
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_LINES);

  for (x=0; x<512; x++) {
    float av;

	  glColor4f(((x+64)>>6)&1, ((x+64)>>6)&2, ((x+64)>>6)&4, 1.0);

	  if (averages) av=averages[x];
    else av=1.0;

    glVertex2f(-1+x/256.0, -0.9);
    glVertex2f(-1+x/256.0, -0.9+clamp((levels[x]/av-threshold)*scale));

    if ((x&15)==0) {
      glColor4f(1, 1, 1, 1);
      glVertex2f(-1+x/256.0, -0.9);
      glVertex2f(-1+x/256.0, -1.0);
    }

    if ((x&3)==0) {
      glColor4f(1, 1, 1, 1);
      glVertex2f(-1+x/256.0, -0.95);
      glVertex2f(-1+x/256.0, -1.0);
    }
  }
  glEnd();
}

glue_static void glueXgrid(float scale) {
  int x, y;

  gluePush();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);

  glBegin(GL_LINES);

  for (y=0; y<32; y++) for (x=0; x<32; x++) {

    if (x==16 || y==16) glColor4f(1, 0, 0, 1);
    else glColor4f(1, 1, 1, 1);

    glVertex3f(0, (x-16)*scale, (y-16)*scale);
    glVertex3f(0, (x-16+1)*scale, (y-16)*scale);
    glVertex3f(0, (x-16)*scale, (y-16)*scale);
    glVertex3f(0, (x-16)*scale, (y-16+1)*scale);
  }

  glEnd();

  gluePop();
}

glue_static void glueYgrid(float scale) {
  int x, y;

  gluePush();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);
  glColor4f(1, 1, 1, 1);

  glBegin(GL_LINES);

  for (y=0; y<32; y++) for (x=0; x<32; x++) {

    if (x==16 || y==16) glColor4f(1, 0, 0, 1);
    else glColor4f(1, 1, 1, 1);

    glVertex3f((x-16)*scale,   0, (y-16)*scale);
    glVertex3f((x-16+1)*scale, 0, (y-16)*scale);
    glVertex3f((x-16)*scale,   0, (y-16)*scale);
    glVertex3f((x-16)*scale,   0, (y-16+1)*scale);
  }

  glEnd();

  gluePop();
}


#ifndef INTRO
#ifdef GLUE_DATAFILE_IN_USE

#include <datafile.h>

char *glueLoadfile(char *filename) {
  FILE *f;
  int length;
  char *data;
  int x;

  for (x=0; x<datafile_filecount; x++) {
    if (strcmp(filename, datafile_datatab[x].filename)==0) {
      f=fopen("datafile.dat", "rb");
      if (!f) glueError("error opening datafile.dat");
      fseek(f, datafile_datatab[x].start, SEEK_SET);
      data=(char *)malloc(datafile_datatab[x].len);
      fread(data, datafile_datatab[x].len, 1, f);
      fclose(f);
      return data;
    }
  }

  f=fopen(filename, "rb");
  if (!f) glueErrorf("error opening file: %s", filename);
  fseek(f, 0, SEEK_END);
  length=ftell(f);
  fseek(f, 0, SEEK_SET);
  data=(char *)malloc(length);
  fread(data, length, 1, f);
  fclose(f);

#if GLUE_DEVEL_MODE==1
  glueNoticef("loaded from disk: %s", filename);
#endif

  return data;
}

FILE *glueDatafile_pointer(char *filename) {
  FILE *f;
  int x;

  for (x=0; x<datafile_filecount; x++) {
    if (strcmp(filename, datafile_datatab[x].filename)==0) {
      f=fopen("datafile.dat", "rb");
      if (!f) glueError("error opening datafile.dat");
      fseek(f, datafile_datatab[x].start, SEEK_SET);
      return f;
    }
  }

  f=fopen(filename, "rb");
  if (!f) glueErrorf("error opening file: %s", filename);

#if GLUE_DEVEL_MODE==1
  glueNoticef("loaded from disk: %s", filename);
#endif

  return f;
}

int glueFilelength(char *filename) {
  FILE *f;
  int x;
  int length;

  for (x=0; x<datafile_filecount; x++) {
    if (strcmp(filename, datafile_datatab[x].filename)==0) {
      return datafile_datatab[x].len;
    }
  }

  f=fopen(filename, "rb");
  if (!f) glueErrorf("error opening file: %s", filename);
  fseek(f, 0, SEEK_END);
  length=ftell(f);
  fclose(f);

  return length;
}

#else // datafile ei käytössä

char *glueLoadfile(char *filename) {
  FILE *f;
  int length;
  char *data;
  int x;

  f=fopen(filename, "rb");
  if (!f) glueErrorf("error opening file: %s", filename);
  fseek(f, 0, SEEK_END);
  length=ftell(f);
  fseek(f, 0, SEEK_SET);
  data=(char *)malloc(length);
  fread(data, length, 1, f);
  fclose(f);

  return data;
}

FILE *glueDatafile_pointer(char *filename) {
  FILE *f;
  int x;

  f=fopen(filename, "rb");
  if (!f) glueErrorf("error opening file: %s", filename);

  return f;
}

int glueFilelength(char *filename) {
  FILE *f;
  int x;
  int length;

  f=fopen(filename, "rb");
  if (!f) glueErrorf("error opening file: %s", filename);
  fseek(f, 0, SEEK_END);
  length=ftell(f);
  fclose(f);

  return length;
}


#endif // DATAFILE_IN_USE
#endif // INTRO
