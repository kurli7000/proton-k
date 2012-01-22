
#ifndef GLUETXTR_H
#define GLUETXTR_H

#ifndef FORGET_INCLUDES
#include "glue.h"
#include <GL/gl.h>
#endif

#define glueTexcoord(a, b) glMultiTexCoord2f(GL_TEXTURE0_ARB, a, b), glMultiTexCoord2f(GL_TEXTURE1_ARB, a, b), glMultiTexCoord2f(GL_TEXTURE2_ARB, a, b), glMultiTexCoord2f(GL_TEXTURE3_ARB, a, b)

/*
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE GL_CLAMP
#endif
*/
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif


// VARIABLES
glue_extern float particle_tiletab[80];
glue_extern float gluePlaneequ_s[4];
glue_extern float gluePlaneequ_t[4];


// PROTOTYPES
#define glueBindtextureN(a, b, c, d, e) do_tu(a, b, c, d, e)
glue_static void texflags(int flgs);
glue_static void glueTexscale(int texnum, Vertex scale);
glue_static void glueTextranslate(int texnum, Vertex trans);
glue_static void glueTexrotate(int texnum, float a, Vertex rot);
glue_static void mipmap(unsigned char *map, int xres, int yres);
glue_static void glueBuildmipmaps(int xres, int yres, unsigned char *map);
glue_static void glueBuildmipmaps_fast(int xres, int yres, unsigned char *map);
glue_static void glueBindtexture(Texture *texture, int flags);
glue_static void glueBindtexture_lod_colors(Texture *tex, int flags);
glue_static void glueBindtexture_mt(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags);
glue_static void glueBindtexture_mta(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags, int alphablendflags);
glue_static void do_tu(int activetexture, Texture *tex, int flags, int blendflags, int alphablendflags);
glue_static void glueBindtexture_mt4(
  Texture *tex1, int flags1,
  Texture *tex2, int flags2,
  Texture *tex3, int flags3,
  Texture *tex4, int flags4,
  int blendflags12,
  int blendflags23,
  int blendflags34);
glue_static void glueBindtexture_mt4a(
  Texture *tex1, int flags1,
  Texture *tex2, int flags2,
  Texture *tex3, int flags3,
  Texture *tex4, int flags4,
  int blendflags12, int alphablendflags12,
  int blendflags23, int alphablendflags23,
  int blendflags34, int alphablendflags34);
#ifndef INTRO
unsigned char *load_jpeg(char *filename, int *xres, int *yres);
Texture *glueLoadtexture(char *filename, char *filename_alpha, int flgs);
void blur(unsigned char *src, unsigned char *dst, float rad, int xres, int yres);
#endif // INTRO
glue_static Texture *glueNoisetexture(int xres, int yres, int min, int max, int flgs);
glue_static Texture *glueNoisetexture2(int xres, int yres, int flgs);
glue_static Texture *glueNoisetexture3(int xres, int yres, int flgs);
glue_static Texture *glueNoisetexture3d(int xres, int yres, int zres, int flgs);
glue_static Texture *glueLoad3dtexture(char *filemask, char *alphafilemask, int numstart, int numend, int flgs);
glue_static Texture *glueColortexture(int xres, int yres, int r, int g, int b, int a, int flgs);
glue_static void glueReloadtexture(Texture *tex, unsigned char *data);
glue_static Texture *glueMaketexture(unsigned char *map, int xres, int yres, int flgs);
glue_static Texture *glueTexture(int xres, int yres, int flgs);
glue_static void glueDisabletexture(void);

#endif // GLUETXTR_H
