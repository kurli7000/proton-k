
#ifndef FORGET_INCLUDES

#include "glueovrl.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glueext.h"
#include "gluegen.h"
#include "gluetxtr.h"
#include "gluemem.h"
#include "glueobj.h"

#endif




glue_static void glueSet2d(int flags) {

  renderflags(flags|GLUE_NO_DEPTH|GLUE_NO_CULL);

  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glDisable(GL_TEXTURE_GEN_Q);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glShadeModel(GL_SMOOTH);
  //glDisable(GL_NORMALIZE);
  glDisable(GL_AUTO_NORMAL);

  if (GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }

  glueSetmatrices2d(flags);
}


glue_static void glueShowimage(Texture *texture, float xpos, float ypos, float zpos, float xsize, float ysize, float rot, float br, int flags) {

  gluePush();
//  glPushAttrib(GL_ALL_ATTRIB_BITS);

  renderflags(flags);

  glueBindtexture(texture, flags);

//  glueSetmatrices2d(flags);
  glueSet2d(flags);

  if (rot!=0) glScalef(1, 1.0/(glueWidescreen/glueAspect), 1);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(texture->scale.x, texture->scale.y, 1);
  glTranslatef(xpos, ypos, 0);
  glTranslatef(0.5, 0.5, 0);
  glScalef(xsize/zpos, ysize/zpos, 1);
  glRotatef(rot, 0, 0, 1);
  glTranslatef(-0.5, -0.5, 0);
  glMatrixMode(GL_MODELVIEW);

  if (flags&GLUE_BLEND_ALPHA) glColor4f(1.0, 1.0, 1.0, br);
  else glColor4f(br, br, br, br);

  glue2dquad();
  // glPopAttrib();

  gluePop();
}

glue_static void glueOverlay(Texture *texture, float br, int flags) {
  float plaa=0.5;

  gluePush();

  glueSet2d(flags);
  glueBindtexture(texture, flags|GLUE_CLAMP);
  glColor4f(br, br, br, br);

  glBegin(GL_POLYGON);
  glTexCoord2f(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glTexCoord2f(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}

glue_static void glueOverlay_mt(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags, float br, Vertex scale, int flags) {
  float plaa=0.1;

  gluePush();

  glueSet2d(flags);
  glueBindtexture_mt(tex1, flags1, tex2, flags2, blendflags);
  glColor4f(br, br, br, br);

  glScalef(scale.x, scale.y, scale.z);

  glBegin(GL_POLYGON);
  glueTexcoord(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glueTexcoord(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glueTexcoord(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glueTexcoord(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}


glue_static void glueOverlay_mt2(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags, float br, Vertex scale1, Vertex scale2, int flags) {
  float plaa=0.1;

  gluePush();

  glueSet2d(flags);
  glueBindtexture_mt(tex1, flags1, tex2, flags2, blendflags);
  glColor4f(br, br, br, br);

  //glScalef(scale.x, scale.y, scale.z);
  glueTextranslate(0, new_v(0.5, 0.5, 0));
  glueTexscale(0, scale1);
  glueTextranslate(0, new_v(-0.5, -0.5, 0));

  glueTextranslate(1, new_v(0.5, 0.5, 0));
  glueTexscale(1, scale2);
  glueTextranslate(1, new_v(-0.5, -0.5, 0));

  glBegin(GL_POLYGON);
  glueTexcoord(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glueTexcoord(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glueTexcoord(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glueTexcoord(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}



glue_static void glueOverlay_alphacheck(Texture *texture, float raja, float br, int inv, int flags) {
  float plaa=0.5;

  gluePush();

  glueSet2d(flags);
  glueBindtexture(texture, flags|GLUE_CLAMP);
  glColor4f(br, br, br, 1);

  glEnable(GL_ALPHA_TEST);
  if (inv) glAlphaFunc(GL_GREATER, raja);
  else glAlphaFunc(GL_LEQUAL, raja);

  glBegin(GL_POLYGON);
  glTexCoord2f(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glTexCoord2f(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  glAlphaFunc(GL_ALWAYS, 0);

  gluePop();
}


glue_static void glueOverlay_scale(Texture *texture, float br, Vertex scale, int flags) {
  float plaa=0.5;

  gluePush();

  glueSet2d(flags);
  glueBindtexture(texture, flags|GLUE_CLAMP);
  glColor4f(br, br, br, br);
  glScalef(scale.x, scale.y, scale.z);

  glBegin(GL_POLYGON);
  glTexCoord2f(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glTexCoord2f(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}

glue_static void glueOverlay_scaletrans(Texture *texture, float br, Vertex scale, Vertex trans, int flags) {
  float plaa=0.0;

  gluePush();

  glueSet2d(flags);
  //glueBindtexture(texture, flags|GLUE_CLAMP);
  glueBindtexture(texture, flags);
  glColor4f(br, br, br, br);
  glTranslatef(trans.x, trans.y, trans.z);
  glScalef(scale.x, scale.y, scale.z);

  glBegin(GL_POLYGON);
  glTexCoord2f(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glTexCoord2f(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}

glue_static void glueOverlay_scaletranscol(Texture *texture, Vertex scale, Vertex trans, int flags) {
  float plaa=0.0;

  gluePush();

  glueSet2d(flags);
  //glueBindtexture(texture, flags|GLUE_CLAMP);
  glueBindtexture(texture, flags);
  glTranslatef(trans.x, trans.y, trans.z);
  glScalef(scale.x, scale.y, scale.z);

  glBegin(GL_POLYGON);
  glTexCoord2f(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glTexCoord2f(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}

glue_static void glueOverlay_scaletranscolrot(Texture *texture, Vertex scale, Vertex trans, float rotfo,  Vertex rot, int flags) {
  float plaa=0.0;

  gluePush();

  glueSet2d(flags);
  //glueBindtexture(texture, flags|GLUE_CLAMP);
  glueBindtexture(texture, flags);
  glRotatef(rotfo, rot.x, rot.y, rot.z);
  glTranslatef(trans.x, trans.y, trans.z);
  glScalef(scale.x, scale.y, scale.z);

  glBegin(GL_POLYGON);
  glTexCoord2f(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glTexCoord2f(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}

glue_static void glueOverlay_translate(Texture *texture, float br, Vertex trans, int flags) {
  float plaa=0.5;

  gluePush();

  glueSet2d(flags);
  glueBindtexture(texture, flags);
  glMatrixMode(GL_TEXTURE);
  glTranslatef(trans.x, trans.y, trans.z);
  glMatrixMode(GL_MODELVIEW);

  glColor4f(br, br, br, br);

  glBegin(GL_POLYGON);
  glTexCoord2f(0-plaa*0.5, 0-plaa*0.5); glVertex2f(-1-plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 0-plaa*0.5); glVertex2f( 1+plaa, -1-plaa);
  glTexCoord2f(1+plaa*0.5, 1+plaa*0.5); glVertex2f( 1+plaa,  1+plaa);
  glTexCoord2f(0-plaa*0.5, 1+plaa*0.5); glVertex2f(-1-plaa,  1+plaa);
  glEnd();

  gluePop();
}



glue_static void glueOverlay_zoom(Texture *texture, float br, float zoom, int flags) {

  gluePush();

  glueSet2d(flags);
  glueBindtexture(texture, flags);

  glColor4f(br, br, br, br);

  glBegin(GL_POLYGON);
  glTexCoord2f(0+zoom, 1-zoom); glVertex2f(-1, -1);
  glTexCoord2f(1-zoom, 1-zoom); glVertex2f( 1, -1);
  glTexCoord2f(1-zoom, 0+zoom); glVertex2f( 1,  1);
  glTexCoord2f(0+zoom, 0+zoom); glVertex2f(-1,  1);
  glEnd();

  gluePop();
}


glue_static void glueCrossfade(Texture *tex1, Texture *tex2, float pos, float ramp, int flags) {
  float br1, br2;

  br1=pow(1.0-pos, ramp);
  br2=pow(pos, ramp);

  gluePush();

  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_FOG);

  glColor4f(br1, br1, br1, 1);

  glueSetmatrices2d(flags);
  glueBindtexture(tex1, flags);

  glue2dquad();

  glueBindtexture(tex2, flags);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glColor4f(br2, br2, br2, 1.0);

  glue2dquad();

  gluePop();
}



