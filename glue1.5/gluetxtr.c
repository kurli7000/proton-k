
#ifndef FORGET_INCLUDES

#include "gluetxtr.h"
#include "glueext.h"
#include "gluemem.h"
#ifdef I_LOVE_BILL
#include "gluewin.h"
#else
#include "gluelin.h"
#endif
#include "gluegen.h"

#ifndef INTRO
#include "libjpeg/jpeglib.h"
#endif

#endif // FORGET_INCLUDES


float gluePlaneequ_s[4]={1, 0, 0, 0};
float gluePlaneequ_t[4]={0, 0, 1, 0};

/**********************************************************************
  Texture related stuff
**********************************************************************/



glue_static void texflags(int flgs) {
  int flags=flgs;//&texturemask;


  if ((flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else {
    if (flags&GLUE_NEAREST) {
      if (flags&GLUE_NO_MIPMAP) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      } else if (flags&GLUE_TRILINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
      } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
      if (flags&GLUE_NO_MIPMAP) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      } else if (flags&GLUE_TRILINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      } else {
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // clamp/repeat
    if (flags&GLUE_CLAMP) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
  }

  // texcoord gen
  if (flags&GLUE_ENVI) {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  } else if (flags&GLUE_PLANE) {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, gluePlaneequ_s);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, gluePlaneequ_t);
  } else if (flags&GLUE_EYE) {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  } else if (flags&GLUE_REFLECTION) {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
  } else {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
  }
}



glue_static void glueTexscale(int texnum, Vertex scale) {
  int mode;

  glGetIntegerv(GL_MATRIX_MODE, &mode);
  glMatrixMode(GL_TEXTURE);

  if (texnum==3) glActiveTexture(GL_TEXTURE3_ARB);
  else if (texnum==2) glActiveTexture(GL_TEXTURE2_ARB);
  else if (texnum==1) glActiveTexture(GL_TEXTURE1_ARB);
  else glActiveTexture(GL_TEXTURE0_ARB);

  glScalef(scale.x, scale.y, scale.z);

  glActiveTexture(GL_TEXTURE0_ARB);
  glMatrixMode(mode);
}



glue_static void glueTextranslate(int texnum, Vertex trans) {
  int mode;

  glGetIntegerv(GL_MATRIX_MODE, &mode);
  glMatrixMode(GL_TEXTURE);

  if (texnum==3) glActiveTexture(GL_TEXTURE3_ARB);
  else if (texnum==2) glActiveTexture(GL_TEXTURE2_ARB);
  else if (texnum==1) glActiveTexture(GL_TEXTURE1_ARB);
  else glActiveTexture(GL_TEXTURE0_ARB);

  glTranslatef(trans.x, trans.y, trans.z);

  glActiveTexture(GL_TEXTURE0_ARB);
  glMatrixMode(mode);
}


glue_static void glueTexrotate(int texnum, float a, Vertex rot) {
  int mode;

  glGetIntegerv(GL_MATRIX_MODE, &mode);
  glMatrixMode(GL_TEXTURE);

  if (texnum==3) glActiveTexture(GL_TEXTURE3_ARB);
  else if (texnum==2) glActiveTexture(GL_TEXTURE2_ARB);
  else if (texnum==1) glActiveTexture(GL_TEXTURE1_ARB);
  else glActiveTexture(GL_TEXTURE0_ARB);

  glRotatef(a, rot.x, rot.y, rot.z);

  glActiveTexture(GL_TEXTURE0_ARB);
  glMatrixMode(mode);
}



glue_static void mipmap(unsigned char *map, int xres, int yres) {
  int x, y;

  for (y=0; y<yres; y++) for (x=0; x<xres; x++) {
    map[y*xres*4+x*4+0]=(map[(y*2+0)*xres*8+(x*2+0)*4+0] + map[(y*2+0)*xres*8+(x*2+1)*4+0] + map[(y*2+1)*xres*8+(x*2+0)*4+0] + map[(y*2+1)*xres*8+(x*2+1)*4+0])*0.25;
    map[y*xres*4+x*4+1]=(map[(y*2+0)*xres*8+(x*2+0)*4+1] + map[(y*2+0)*xres*8+(x*2+1)*4+1] + map[(y*2+1)*xres*8+(x*2+0)*4+1] + map[(y*2+1)*xres*8+(x*2+1)*4+1])*0.25;
    map[y*xres*4+x*4+2]=(map[(y*2+0)*xres*8+(x*2+0)*4+2] + map[(y*2+0)*xres*8+(x*2+1)*4+2] + map[(y*2+1)*xres*8+(x*2+0)*4+2] + map[(y*2+1)*xres*8+(x*2+1)*4+2])*0.25;
    map[y*xres*4+x*4+3]=(map[(y*2+0)*xres*8+(x*2+0)*4+3] + map[(y*2+0)*xres*8+(x*2+1)*4+3] + map[(y*2+1)*xres*8+(x*2+0)*4+3] + map[(y*2+1)*xres*8+(x*2+1)*4+3])*0.25;
  }

  for (x=xres*yres*4; x<xres*yres*4*4; x++) map[x]=0;
}



/* converts row of char type data to floats with possible gamma correction (sqr) and exaggeration of highest intensities (glow) */
static void char2flt(float *dst, unsigned char *src, int stride, int len, int sqr, int glow) {
  float *dmax = dst+len, a;
  if (sqr) {
    if (glow) for (; dst<dmax; src+=stride, dst++) { a = *src*(1./255.0); if (a>.85) a += (a-.85)*(a-.85)*20; *dst = a*a; }
         else for (; dst<dmax; src+=stride, dst++) { a = *src*(1./255.0); *dst = a*a; }
  } else {
    if (glow) for (; dst<dmax; src+=stride, dst++) { a = *src*(1./255.0); if (a>.85) a += (a-.85)*(a-.85)*20; *dst = a; }
         else for (; dst<dmax; src+=stride, dst++) { a = *src*(1./255.0); *dst = a; }
  }
}
/* does the inverse */
static void flt2char(unsigned char *dst, int stride, float *src, int len, int sqr) {
  float *smax = src+len;
  int a;
  if (sqr) for (; src<smax; src++, dst+=stride) a = sqrt(*src)*255.9, *dst = a<0 ? 0 : a>255 ? 255 : a;
      else for (; src<smax; src++, dst+=stride) a = *src*255.9, *dst = a<0 ? 0 : a>255 ? 255 : a;
}
/* dst = dst + convolve(src0, src1) */
static void conv_acc(float *dst, float *src0, int cnt0, float *src1, int cnt1) {
  int i, j;
  if (cnt1==0) return;
  for (i=0; i<cnt0; i++) for (j=0; j<cnt1; j++) dst[i+j] += src0[i]*src1[j];
}
/* dst = dst + src*k */
static void fbuf_lacc(float *dst, float *src, int cnt, float k) {
  int i;
  for (i=0; i<cnt; i++) dst[i] += src[i]*k;
}
static void normalize_kernel(float *b, int cnt) {
  int i;
  float s=0;

  for (i=0; i<cnt; i++) s += b[i];
  s = 1./s;
  for (i=0; i<cnt; i++) b[i] *= s;;
}

static float sinc(float x) { x=fabs(x); return x<.0001? 1 : sin(x)/x; }

glue_static void glueBuildmipmaps(int xres, int yres, unsigned char *map) {
  int level = 0, i, x, y, c;
  void *tmpptr = tmpmalloc(0);
  float *buf, *buf2;
  static float kernel[16];// = { 0,0,0,0,0,0,-.1, .6,.6, -.1,0,0,0,0,0,0 };
  float blurcnt = 2.2;

  blurcnt=pi/blurcnt;
  for (i=0; i<16; i++) kernel[i] = sinc((i-7.5)*blurcnt)*cos((i-7.5)*(pi/16.0))*cos((i-7.5)*(pi/16.0));
  normalize_kernel(kernel, 16);

  buf = tmpmalloc(xres*yres*sizeof *buf);
  buf2 = tmpmalloc((xres+16)*(yres+16)*sizeof *buf);
  glTexImage2D(GL_TEXTURE_2D, level++, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  do {
    int oyres=yres, oxres=xres;
    for (c=0; c<4; c++) {
      yres=oyres; xres=oxres;
#if 1
      for (y=0; y<yres; y++) char2flt(buf+y*xres, map+y*xres*4+c, 4, xres, c<3, 0);
      if (yres>1) {
        for (x=0; x<(yres+16)*xres; x++) buf2[x] = 0.0;
        for (i=0; i<16; i++) for (y=0; y<yres; y++) fbuf_lacc(buf2+(y+i)*xres, buf+y*xres, xres, kernel[i]);
        yres >>= 1;
        for (y=0; y<yres; y++) for (x=0; x<xres; x++) buf[y*xres+x] = buf2[(y*2+8)*xres+(x)];
      }
      if (xres>1) {
        for (x=0; x<yres*(xres+16); x++) buf2[x] = 0.0;
        for (y=0; y<yres; y++) conv_acc(buf2+y*(xres+16), buf+y*xres, xres, kernel, 16);
        xres >>= 1;
        for (y=0; y<yres; y++) for (x=0; x<xres; x++) buf[y*xres+x] = buf2[y*(xres*2+16)+(x*2+8)];
      }
      for (y=0; y<yres; y++) flt2char(map+y*xres*4+c, 4, buf+y*xres, xres, c<3);
#else

/*    for (y=0; y<yres; y++) for (x=0; x<xres; x++) {
        map[(y*xres+x)*4+c]=
          (map[((y*2+0)*xres*2+(x*2+0))*4+c]+
           map[((y*2+0)*xres*2+(x*2+1))*4+c]+
           map[((y*2+1)*xres*2+(x*2+1))*4+c]+
           map[((y*2+1)*xres*2+(x*2+0))*4+c]+2)>>2;
      }*/
      if (xres>1 && yres>1) {
        xres>>=1; yres>>=1;
        for (y=0; y<yres; y++) for (x=0; x<xres; x++) {
          map[(y*xres+x)*4+c]=
            (map[((y*2+0)*xres*2+(x*2+0))*4+c]+
             map[((y*2+0)*xres*2+(x*2+1))*4+c]+
             map[((y*2+1)*xres*2+(x*2+1))*4+c]+
             map[((y*2+1)*xres*2+(x*2+0))*4+c]+2)>>2;
        }
      } else xres=xres+1>>1, yres=yres+1>>1;
#endif
    }
    glTexImage2D(GL_TEXTURE_2D, level++, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } while (xres>1 || yres>1);
  tmpfree(tmpptr);
  //gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, map);
}


glue_static void glueBuildmipmaps_fast(int xres, int yres, unsigned char *map) {
  int c, x, y, level=0;

  glTexImage2D(GL_TEXTURE_2D, level++, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);

  do {
    if (xres>1 && yres>1) {
      xres>>=1; yres>>=1;

      for (c=0; c<4; c++) for (y=0; y<yres; y++) for (x=0; x<xres; x++) {
        map[(y*xres+x)*4+c]=
          (map[((y*2+0)*xres*2+(x*2+0))*4+c]+
           map[((y*2+0)*xres*2+(x*2+1))*4+c]+
           map[((y*2+1)*xres*2+(x*2+1))*4+c]+
           map[((y*2+1)*xres*2+(x*2+0))*4+c]+2)>>2;
      }
    } else xres=(xres+1)>>1, yres=(yres+1)>>1;

    glTexImage2D(GL_TEXTURE_2D, level++, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } while (xres>1 || yres>1);
}


glue_static void glueBindtexture(Texture *texture, int flags) {

  if (!texture) glueError("error: NULL texture");

  glActiveTexture(GL_TEXTURE3_ARB);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE2_ARB);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0_ARB);

  if ((texture->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, texture->texnum);
  } else {
    if (GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->texnum);
  }

  texflags(texture->flags|flags);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(texture->scale.x, texture->scale.y, texture->scale.z);
  glMatrixMode(GL_MODELVIEW);

}



glue_static void glueBindtexture_lod_colors(Texture *tex, int flags) {

  // tu0
  glActiveTexture(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_1D);
  if (GLUE_CUBEMAP_OK) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex->texnum);
  texflags(tex->flags|flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  // tu1
  glActiveTexture(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_1D);
  if (GLUE_CUBEMAP_OK) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tex->texnum);
  texflags(tex->flags|flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_INTERPOLATE_ARB);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, GL_SRC_ALPHA);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  // tu2

  glActiveTexture(GL_TEXTURE2_ARB);
  glDisable(GL_TEXTURE_1D);
  if (GLUE_CUBEMAP_OK) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, glueDummytex->texnum);
  texflags(glueDummytex->flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  // tu3
  glActiveTexture(GL_TEXTURE3_ARB);
  glDisable(GL_TEXTURE_1D);
  if (GLUE_CUBEMAP_OK) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  glDisable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE0_ARB);
}



glue_static void glueBindtexture_mt(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags) {


  if (!tex1) glueError("error: NULL texture");
  if (!tex2) glueError("error: NULL texture");

  glActiveTexture(GL_TEXTURE3_ARB);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE2_ARB);
  glDisable(GL_TEXTURE_2D);

  // tu0
  glActiveTexture(GL_TEXTURE0_ARB);

  if ((tex1->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex1->texnum);
  } else {
    if (GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex1->texnum);
  }

  texflags(flags1|tex1->flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);


  if (blendflags&GLUE_BLEND_ALPHA) { // crossfade
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);

    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);
  } else { // väri vaikuttaa ekaan unittiin
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);

    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);
  }

  // tu1
  glActiveTexture(GL_TEXTURE1_ARB);

  if ((tex2->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex2->texnum);
  } else {
    if (GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex2->texnum);
  }

  texflags(flags2|tex2->flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);

  if (blendflags&GLUE_BLEND) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_ADD);
  } else if (blendflags&GLUE_BLEND_MULTIPLY) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);
  } else if (blendflags&GLUE_BLEND_SIGNED) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD_SIGNED_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_ADD_SIGNED_ARB);
  } else if (blendflags&GLUE_BLEND_ALPHA) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_INTERPOLATE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, GL_SRC_ALPHA);
  } else {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
  }

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(tex2->scale.x, tex2->scale.y, tex2->scale.z);
  glMatrixMode(GL_MODELVIEW);

  glActiveTexture(GL_TEXTURE0_ARB);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(tex1->scale.x, tex1->scale.y, tex1->scale.z);
  glMatrixMode(GL_MODELVIEW);
}



glue_static void glueBindtexture_mta(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags, int alphablendflags) {

  if (!tex1) glueError("error: NULL texture");
  if (!tex2) glueError("error: NULL texture");

  glActiveTexture(GL_TEXTURE3_ARB);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE2_ARB);
  glDisable(GL_TEXTURE_2D);

  // tu0
  glActiveTexture(GL_TEXTURE0_ARB);

  if ((tex1->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex1->texnum);
  } else {
    if (GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex1->texnum);
  }

  texflags(flags1|tex1->flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);

  // tu1
  glActiveTexture(GL_TEXTURE1_ARB);

  if ((tex2->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex2->texnum);
  } else {
    if (GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex2->texnum);
  }

  texflags(flags2|tex2->flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);

  if (blendflags&GLUE_BLEND) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD);
  } else if (blendflags&GLUE_BLEND_MULTIPLY) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
  } else if (blendflags&GLUE_BLEND_SIGNED) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD_SIGNED_ARB);
  } else if (blendflags&GLUE_BLEND_ALPHA) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_INTERPOLATE_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, GL_SRC_ALPHA);
  } else {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
  }

  if (alphablendflags&GLUE_BLEND) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_ADD);
  } else if (alphablendflags&GLUE_BLEND_MULTIPLY) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);
  } else if (alphablendflags&GLUE_BLEND_SIGNED) {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_ADD_SIGNED_ARB);
  } else {
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
  }

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(tex2->scale.x, tex2->scale.y, tex2->scale.z);
  glMatrixMode(GL_MODELVIEW);

  glActiveTexture(GL_TEXTURE0_ARB);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(tex1->scale.x, tex1->scale.y, tex1->scale.z);
  glMatrixMode(GL_MODELVIEW);
}






glue_static void do_tu(int activetexture, Texture *tex, int flags, int blendflags, int alphablendflags) {

  glActiveTexture(activetexture);

  if (tex==0) {

    glDisable(GL_TEXTURE_2D);
    if (GLUE_CUBEMAP_OK) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glDisable(GL_TEXTURE_1D);

  } else {

    if ((tex->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_CUBE_MAP_ARB);
      glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex->texnum);
    } else {
      if (GLUE_CUBEMAP_OK) {
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
      }
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex->texnum);
    }

    texflags(flags|tex->flags);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);

    if (blendflags&GLUE_BLEND) {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD);
    } else if (blendflags&GLUE_BLEND_MULTIPLY) {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
    } else if (blendflags&GLUE_BLEND_SIGNED) {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD_SIGNED_ARB);
    } else if (blendflags&GLUE_BLEND_ALPHA) {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_INTERPOLATE_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, GL_SRC_ALPHA);
    } else {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
    }

    if (alphablendflags&GLUE_BLEND) {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_ADD);
    } else if (alphablendflags&GLUE_BLEND_MULTIPLY) {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);
    } else if (alphablendflags&GLUE_BLEND_SIGNED) {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_ADD_SIGNED_ARB);
    } else {
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
    }


    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(tex->scale.x, tex->scale.y, tex->scale.z);
    glMatrixMode(GL_MODELVIEW);
  }
}


glue_static void glueBindtexture_mt4(
  Texture *tex1, int flags1,
  Texture *tex2, int flags2,
  Texture *tex3, int flags3,
  Texture *tex4, int flags4,
  int blendflags12,
  int blendflags23,
  int blendflags34) {

  if (!tex1) glueError("error: NULL texture");

  // tu0
  glActiveTexture(GL_TEXTURE0_ARB);

  if ((tex1->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex1->texnum);
  } else {
    if (GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex1->texnum);
  }

  texflags(flags1|tex1->flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  // glColor vaikuttaa ekaan textureunittiin
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);


  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(tex1->scale.x, tex1->scale.y, tex1->scale.z);
  glMatrixMode(GL_MODELVIEW);


  do_tu(GL_TEXTURE1_ARB, tex2, flags2, blendflags12, blendflags12);
  do_tu(GL_TEXTURE2_ARB, tex3, flags3, blendflags23, blendflags23);
  do_tu(GL_TEXTURE3_ARB, tex4, flags4, blendflags34, blendflags34);

  glActiveTexture(GL_TEXTURE0_ARB);
}



glue_static void glueBindtexture_mt4a(
  Texture *tex1, int flags1,
  Texture *tex2, int flags2,
  Texture *tex3, int flags3,
  Texture *tex4, int flags4,
  int blendflags12, int alphablendflags12,
  int blendflags23, int alphablendflags23,
  int blendflags34, int alphablendflags34) {

  if (!tex1) glueError("error: NULL texture");

  // tu0
  glActiveTexture(GL_TEXTURE0_ARB);

  if ((tex1->flags&GLUE_REFLECTION) && GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, tex1->texnum);
  } else {
    if (GLUE_CUBEMAP_OK) {
      glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex1->texnum);
  }

  texflags(flags1|tex1->flags);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);

  // glColor vaikuttaa ekaan textureunittiin
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);

  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
  glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_PRIMARY_COLOR_ARB);
  glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);


  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(tex1->scale.x, tex1->scale.y, tex1->scale.z);
  glMatrixMode(GL_MODELVIEW);

  do_tu(GL_TEXTURE1_ARB, tex2, flags2, blendflags12, alphablendflags12);
  do_tu(GL_TEXTURE2_ARB, tex3, flags3, blendflags23, alphablendflags23);
  do_tu(GL_TEXTURE3_ARB, tex4, flags4, blendflags34, alphablendflags34);

  glActiveTexture(GL_TEXTURE0_ARB);
}



#ifndef INTRO
unsigned char *load_jpeg(char *filename, int *xres, int *yres) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * infile;
  JSAMPARRAY buffer;
  int row_stride;
  int x;
  unsigned char *resu, *p;
/*
  if ((infile = fopen(filename, "rb")) == NULL) {
//    fprintf(stderr, "Can't open %s\n", filename);
//    return 0;
    char *erreur=malloc(1024);

    sprintf(erreur, "%s not found", filename);
    glueError(erreur);
    free(erreur);
  }
*/
  infile = glueDatafile_pointer(filename);

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, 1);
  jpeg_start_decompress(&cinfo);
  row_stride = cinfo.output_width * cinfo.output_components;
  buffer = (*cinfo.mem->alloc_sarray)
  ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
  p=resu=malloc(cinfo.output_height*cinfo.output_width*3);
  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    for (x=0; (unsigned)x<(unsigned)cinfo.output_width*3; x++) *p++=buffer[0][x];
    //put_scanline_someplace(buffer[0], row_stride);
  }
  xres[0]=cinfo.output_width;
  yres[0]=cinfo.output_height;

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
  return resu;
}


Texture *glueLoadtexture(char *filename, char *filename_alpha, int flgs) {
  int flags=flgs;//&texloadmask;
  unsigned char *tmap;
  unsigned char *amap;
  unsigned char *map;
  int x, y;
  int pic_xres, pic_yres;
  int tex_xres, tex_yres;
  Texture *tex=malloc(sizeof(Texture));


  if (!filename && !filename_alpha) glueError("stoopid");

  if (filename_alpha) amap=load_jpeg(filename_alpha, &pic_xres, &pic_yres);
  else amap=0;

  if (!filename) {
    tmap=malloc(pic_xres*pic_yres*4);
    memset(tmap, 255, pic_xres*pic_yres*4);
  } else {
    tmap=load_jpeg(filename, &pic_xres, &pic_yres);
  }

 // tex_xres=texreso(pic_xres);
 // tex_yres=texreso(pic_yres);
  tex_xres=pic_xres;
  tex_yres=pic_yres;

  map=malloc(tex_xres*tex_yres*4);
  memset(map, 255, tex_xres*tex_yres*4);

  for (y=0; y<pic_yres; y++) for (x=0; x<pic_xres; x++) {
    unsigned char r, g, b, a;

    r=tmap[y*pic_xres*3+x*3+0];
    g=tmap[y*pic_xres*3+x*3+1];
    b=tmap[y*pic_xres*3+x*3+2];
    if (amap!=0) a=amap[y*pic_xres*3+x*3+0];
    else a=255;

    map[y*tex_xres*4+x*4+0]=r;
    map[y*tex_xres*4+x*4+1]=g;
    map[y*tex_xres*4+x*4+2]=b;
    map[y*tex_xres*4+x*4+3]=a;

  }

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=tex_xres;
  tex->yres=tex_yres;
  tex->scale.x=((float)pic_xres/(float)tex_xres);
  tex->scale.y=((float)pic_yres/(float)tex_yres);
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_2D, tex->texnum);
  if (flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_xres, tex_yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, tex_xres, tex_yres, GL_RGBA, GL_UNSIGNED_BYTE, map);
    //glueBuildmipmaps(tex_xres, tex_yres, map);
  }

  free(tmap);
  if (filename_alpha!=0) free(amap);
  free(map);

  glueLoading();

  return tex;
}



void blur(unsigned char *src, unsigned char *dst, float rad, int xres, int yres) {
  int x, y;

  float r=xres*rad;

  int ir=(int)r;
  int tabkoko=(ir+2)*2;
  float *brtab=malloc(sizeof(float)*tabkoko*tabkoko);
//  static int foo = 0;


  for (y=0; y<tabkoko; y++) for (x=0; x<tabkoko; x++) {
    float u=x-(ir+2)+.23;
    float v=y-(ir+2)+.17;
    /*float d=sqrt(u*u+v*v);*/
    float d0=(u*sin(2.50)+v*cos(2.50));
    float d1=(u*sin(3.76)+v*cos(3.76));
    float d2=(u*sin(5.02)+v*cos(5.02));
    float d3=(u*sin(6.28)+v*cos(6.28));
    float d4=(u*sin(7.54)+v*cos(7.54));
    float d = d0>d1?d0:d1; d=d>d2?d:d2; d=d>d3?d:d3; d=d>d4?d:d4; d*=1.1;
    brtab[y*tabkoko+x]=(r+d*3)*clamp((r-d)*2);
  }
  {
    float brsum=0;
    for (x=0; x<tabkoko*tabkoko; x++) brsum+=brtab[x];
    brsum=1./brsum;
    for (x=0; x<tabkoko*tabkoko; x++) brtab[x]*=brsum;
  }

  {
    float *tmprow = malloc((xres+ir*2+4)*sizeof *tmprow);
    float *srctmp = malloc((xres+ir*2+4)*sizeof *srctmp);
    for (y=0; y<yres; y++) {
      int v, c;

      for (c=0; c<4; c++) {
        for (x=0; x<xres+ir*2+4; x++) tmprow[x]=0;
        for (v=-ir-2; v<ir+2; v++) if (y+v>=0 && y+v<yres) {
          /*for (x=0; x<xres; x++) {
            float a = src[(y+v)*xres*4+x*4+c]*(1./255.);
            if (c<3 && a>.85) a+=(a-.85)*(a-.85)*20;
            a*=a;
            srctmp[x] = a;
          }*/
          char2flt(srctmp, src+(y+v)*xres*4+c, 4, xres, c<3, c<3);
          conv_acc(tmprow, srctmp, xres, brtab+(v+ir+2)*tabkoko, ir*2+4);
        }
        flt2char(dst+y*xres*4+c, 4, tmprow+ir+2, xres, c<3);
/*        if (c<3) for (x=0; x<xres; x++) dst[y*xres*4+x*4+c]=r0255(sqrt(tmprow[x+ir+2])*255.0);
            else for (x=0; x<xres; x++) dst[y*xres*4+x*4+c]=r0255(tmprow[x+ir+2]*255.0);*/
      }
    }
    free(tmprow);
    free(srctmp);
  }
  free(brtab);
}



#endif // INTRO



glue_static Texture *glueNoisetexture(int xres, int yres, int min, int max, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));
  unsigned char *map=malloc(xres*yres*4);
  int x;

  for (x=0; x<xres*yres; x++) {
    int c=(random()%(max-min))+min;
    map[x*4+0]=c;
    map[x*4+1]=c;
    map[x*4+2]=c;
    map[x*4+3]=255;
  }

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_2D, tex->texnum);

  if (flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, map);
//    glueBuildmipmaps(xres, yres, map);
  }

  glueLoading();

  free(map);
  return tex;
}

glue_static Texture *glueNoisetexture2(int xres, int yres, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));
  unsigned char *map=malloc(xres*yres*4);
  int x;

  for (x=0; x<xres*yres; x++) {
    int c=random()&255;

    map[x*4+0]=c;
    map[x*4+1]=c;
    map[x*4+2]=c;
    map[x*4+3]=ranf()*255;
  }

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_2D, tex->texnum);

  if (flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
    glueBuildmipmaps(xres, yres, map);
  }

  glueLoading();

  free(map);
  return tex;
}


glue_static Texture *glueNoisetexture3(int xres, int yres, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));
  unsigned char *map=malloc(xres*yres*4);
  int x;

  for (x=0; x<xres*yres; x++) {
    int c=random()&255;

    map[x*4+0]=ranf()*255;
    map[x*4+1]=ranf()*255;
    map[x*4+2]=ranf()*255;
    map[x*4+3]=ranf()*255;
  }

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_2D, tex->texnum);

  if (flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
    glueBuildmipmaps(xres, yres, map);
  }

  glueLoading();

  free(map);
  return tex;
}


#if GLUE_USE_3DTEXTURE==1
glue_static Texture *glueNoisetexture3d(int xres, int yres, int zres, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));
  unsigned char *map=malloc(xres*yres*zres*4);
  int x;

  for (x=0; x<xres*yres*zres; x++) {
    int c=random()&255;

    map[x*4+0]=random()&255;
    map[x*4+1]=random()&255;
    map[x*4+2]=random()&255;
    map[x*4+3]=random()&255;
  }

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_3D, tex->texnum);

  if (flags&GLUE_NO_MIPMAP) {
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, xres, yres, zres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
    glueBuildmipmaps(xres, yres, map); // apua lol, ei toimi
  }

  glueLoading();

  free(map);
  return tex;
}


glue_static Texture *glueLoad3dtexture(char *filemask, char *alphafilemask, int numstart, int numend, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));
  unsigned char *map;//=malloc(xres*yres*zres*4);
  int x, i;
  int numtex=numend-numstart+1;
  char *buffer;
  char *abuffer;
  int xres, yres, xrestemp, yrestemp;
  char filename[256];
  
  sprintf(filename, filemask, numstart);
  buffer=load_jpeg(filename, &xres, &yres);
  sprintf(filename, alphafilemask, numstart);
  abuffer=load_jpeg(filename, &xrestemp, &yrestemp);
  if (xrestemp!= xres || yrestemp!=yres) glueError("error: 3dtexture resolution mismatch with alpha fuckhead");
  map=malloc(xres*yres*numtex*4);
  
  memset(map, 255, xres*yres*numtex*4);
  //memcpy(map, buffer, xres*yres*4);
  for (i=0; i<xres*yres; i++) {
  	map[i*4+0]=buffer[i*3+0];
  	map[i*4+1]=buffer[i*3+1];
  	map[i*4+2]=buffer[i*3+2];
  	map[i*4+3]=abuffer[i*3+0];
  }
  free(buffer);
  free(abuffer);
  
  for (x=numstart+1; x<=numend; x++) {
  	int num=x-numstart;
  	char *ptr=map+num*(xres*yres*4);
  	
  	sprintf(filename, filemask, x);
  	buffer=load_jpeg(filename, &xrestemp, &yrestemp);
  	if (xrestemp!= xres || yrestemp!=yres) glueError("error: 3dtexture resolution mismatch");
  	sprintf(filename, alphafilemask, x);
  	abuffer=load_jpeg(filename, &xrestemp, &yrestemp);
  	if (xrestemp!= xres || yrestemp!=yres) glueError("error: 3dtexture resolution mismatch (alpha)");
  	//memcpy(map+num*(xres*yres*3), buffer, xres*yres*3);
    for (i=0; i<xres*yres; i++) {
  	  ptr[i*4+0]=buffer[i*3+0];
  	  ptr[i*4+1]=buffer[i*3+1];
     	ptr[i*4+2]=buffer[i*3+2];
    	ptr[i*4+3]=abuffer[i*3+0];
    }
  	free(buffer);
    free(abuffer);
  }
 
  
  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->zres=numtex;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_3D, tex->texnum);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, xres, yres, numtex, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);

  glueLoading();

  free(map);
  return tex;
}


#endif

glue_static Texture *glueColortexture(int xres, int yres, int r, int g, int b, int a, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));
  unsigned char *map=malloc(xres*yres*4);
  int x;

  for (x=0; x<xres*yres; x++) {
    map[x*4+0]=r;
    map[x*4+1]=g;
    map[x*4+2]=b;
    map[x*4+3]=a;
  }

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_2D, tex->texnum);

  if (flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
//    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, map);
    glueBuildmipmaps(xres, yres, map);
  }

  glueLoading();

  free(map);
  return tex;
}


glue_static void glueReloadtexture(Texture *tex, unsigned char *data) {

  glueBindtexture(tex, 0);

  if (tex->flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->xres, tex->yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  } else {
//    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, tex->xres, tex->yres, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glueBuildmipmaps(tex->xres, tex->yres, data);
    glueBuildmipmaps_fast(tex->xres, tex->yres, data);
  }
}

glue_static Texture *glueMaketexture(unsigned char *map, int xres, int yres, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_2D, tex->texnum);

  if (flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
//    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, map);
    glueBuildmipmaps(xres, yres, map);
  }


  glueLoading();

  return tex;
}



glue_static Texture *glueTexture(int xres, int yres, int flgs) {
  int flags=flgs;//&texloadmask;
  Texture *tex=malloc(sizeof(Texture));
  unsigned char *map=malloc(xres*yres*4);
  int x;

  for (x=0; x<xres*yres; x++) {
    map[x*4+0]=0;
    map[x*4+1]=0;
    map[x*4+2]=0;
    map[x*4+3]=0;
  }

  glGenTextures(1, &tex->texnum);
  tex->flags=flags;
  tex->xres=xres;
  tex->yres=yres;
  tex->scale.x=1.0;
  tex->scale.y=1.0;
  tex->scale.z=1.0;

  glBindTexture(GL_TEXTURE_2D, tex->texnum);

  if (flags&GLUE_NO_MIPMAP) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xres, yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
  } else {
//    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, map);
    glueBuildmipmaps(xres, yres, map);
  }

  glueLoading();

  free(map);
  return tex;
}



glue_static void glueDisabletexture(void) {

  glActiveTexture(GL_TEXTURE3_ARB);
  if (GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }
#if GLUE_USE_3DTEXTURE==1
  glDisable(GL_TEXTURE_3D);
#endif
  glDisable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE2_ARB);
  if (GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }
#if GLUE_USE_3DTEXTURE==1
  glDisable(GL_TEXTURE_3D);
#endif
  glDisable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE1_ARB);
  if (GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }
#if GLUE_USE_3DTEXTURE==1
  glDisable(GL_TEXTURE_3D);
#endif
  glDisable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE0_ARB);
  if (GLUE_CUBEMAP_OK) {
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
  }
#if GLUE_USE_3DTEXTURE==1
  glDisable(GL_TEXTURE_3D);
#endif
  glDisable(GL_TEXTURE_2D);
}





