
#ifndef GLUE_H
#define GLUE_H


/*
**  GLUE - Graphics Library Utility Environment V1.5
**  (c) Kewlers 2002-2010
**
**  ********
**
**  MinGW Developer Studio / Project Settings:
**
**  Extra compiling options:         -Wall -Wno-unused -Wno-parentheses
**  Optimization level:              3
**  Additional include directories:  Projektin dirikka
**  Libraries:                       opengl32,fmod,glu32,kernel32,user32,gdi32,jpeg.dll
**
**
*/


#ifndef glue_static
#define glue_static
#endif

#ifndef glue_extern
#define glue_extern extern
#endif

#ifndef FORGET_INCLUDES
#include "glue_setup.h"
#endif


#ifndef glue_widescreen
  #define glue_widescreen (4.0/3.0)
#endif

#ifndef GLUE_MIN_XRES
  #define GLUE_MIN_XRES 640
#endif

#ifndef GLUE_MIN_YRES
  #define GLUE_MIN_YRES 360
#endif

#ifndef GLUE_MAX_XRES
  #define GLUE_MAX_XRES 1280
#endif

#ifndef GLUE_MAX_YRES
  #define GLUE_MAX_YRES 1024
#endif

#ifndef GLUE_USE_ARBSHADOW
  #define GLUE_USE_ARBSHADOW 0
#endif

#ifndef GLUE_USE_3DTEXTURE
  #define GLUE_USE_3DTEXTURE 1
#endif

#ifndef GLUE_USE_CUBE
  #define GLUE_USE_CUBE 0
#endif

#ifndef GLUE_USE_MPEG2
  #define GLUE_USE_MPEG2 0
#endif

#ifndef GLUE_DEVEL_MODE
  #define GLUE_DEVEL_MODE 0
#endif

#ifndef glueName
  #define glueName "GLUE"
#endif

#ifndef glueName_fsaa
  #define glueName_fsaa "GLUE (FSAA)"
#endif

#ifndef GLUELOADSTEPS
#define GLUELOADSTEPS 50
#endif

#ifndef FORGET_INCLUDES

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include <GL\gl.h>
#include <GL\glu.h>

#endif // FORGET INCLUDES


// constants
#define pi 3.141582653589793

// texturing flags for glueBindtexture/glueLoadtexture/texflags
#define GLUE_NEAREST 1  //1
#define GLUE_CLAMP  2  //2
#define GLUE_ENVI   4  //3
#define GLUE_EYE    8  //4
#define GLUE_PLANE 16  //5
#define GLUE_NO_MIPMAP 32 //6
// only for multitexturing..
#define GLUE_BLEND_SIGNED 4194304 //22

// rendering flags for renderflags
#define GLUE_NO_CULL 64 //7
#define GLUE_INV_CULL 128 //8
#define GLUE_NO_DEPTH 256 //9
#define GLUE_CHECK_DEPTH 512 //10
#define GLUE_BLEND 1024 //11
#define GLUE_BLEND_SUBTRACT 2048 //12
#define GLUE_BLEND_MULTIPLY 4096 //13
#define GLUE_BLEND_ALPHA 8192 //14
#define GLUE_BLEND_ALPHAADD 32768  //16
//#define GLUE_BLEND_ALPHASATURATE 65536 //17
#define GLUE_BLEND_INV_ALPHA 16384 // 15
#define GLUE_BLEND_SATURATE 262144 //19
#define GLUE_ERASE_STENCIL 2097152 //22
#define GLUE_REFLECTION 134217728 //28
#define GLUE_TRILINEAR 67108864 //27
#define GLUE_BLEND_DST_ALPHA 536870912 // 30
#define GLUE_BLEND_INV_DST_ALPHA 1073741824 // 31
#define GLUE_BLEND_INV_DST 524288 //20
#define GLUE_BLEND_INVERT 268435456 // 29

// glueDrawobject
#define GLUE_FAST 1048576 //21

// object loading flags for glueLoadtexture
#define GLUE_FIX_WRAP 8388608 //23
#define GLUE_VCOLOR_NORMALS 16777216 //24
#define GLUE_TEXGEN_NORMALS 33554432 //25
#define GLUE_GEN_TANGENTS   67108864 //26

// general flags
//#define GLUE_CALC 65536  //17
//#define GLUE_DRAW 131072  //18


// flagmasks

// turhia, vois heivaa vittyy
//static int texloadmask=GLUE_NO_MIPMAP|GLUE_REFLECTION;//|GLUE_INVERSE;
//static int texturemask=GLUE_NO_MIPMAP|GLUE_REFLECTION|GLUE_NEAREST|GLUE_CLAMP|GLUE_ENVI|GLUE_EYE|GLUE_PLANE|GLUE_TRILINEAR;
//static int rendermask=GLUE_NO_CULL|GLUE_INV_CULL|GLUE_NO_DEPTH|GLUE_BLEND|GLUE_BLEND_SUBTRACT|GLUE_BLEND_MULTIPLY|GLUE_BLEND_ALPHA|GLUE_BLEND_SATURATE|GLUE_ERASE_STENCIL|GLUE_CHECK_DEPTH|GLUE_BLEND_DST_ALPHA|GLUE_BLEND_INV_DST_ALPHA|GLUE_BLEND_INV_DST|GLUE_BLEND_INVERT;
//static int objloadmask=GLUE_FIX_WRAP|GLUE_VCOLOR_NORMALS|GLUE_TEXGEN_NORMALS|GLUE_GEN_TANGENTS;

// structs

typedef struct {
  float x, y, z;//, k;
} Vertex;

typedef struct {
  float r, g, b, a;
} Color;

typedef struct {
  unsigned char r, g, b, a;
} Uccolor;

typedef struct {
  Vertex pos;
  Vertex tgt;
  Vertex yvec;
  Vertex cen;
  float znear, zfar;
  float per;
  float tilt;
} Camera;
/*
typedef struct {
  float a, b, c, d;
} Plane;
*/

typedef struct {
  int v[3];
  Vertex norm;
  void *nf[3];
//  Plane p;
  Vertex cen;
  int visible;
} Face;

typedef struct {
  int v[2];
  int f[2];
  float sharpness;
} Edge;

typedef struct {
  unsigned int texnum;
  Vertex scale;
  int xres, yres, zres;
  int flags;
  void *cubetex;
  int texcnt;
} Texture;


typedef struct {
  int tc;
  Texture *tex;
} Texarray;

typedef struct {
  unsigned int objnum;
  int vc, fc, ec;
  Face *faces;
  Vertex *vertices;
  Vertex *normals;
  Vertex *texcoords;
  Vertex *colors;
  Color *tangents;
  Edge *edges;
  int *indices;
  Vertex center;
  int shadowlist;
  float size;
  //int flags;
  int *orig_indices;
  int *vertex_links;
  int linkcnt;
} Object;

typedef struct _Splinept {
  float t;
  Vertex p;
} Splinept;

typedef struct {
  Texture *tex;
  float widthtab[256];
  Vertex postab[256];
} Font;

typedef struct {
  float x, y;
} Grid;

typedef struct {
  Texture *tex;
  int xcnt, ycnt;
} Particle;

typedef Uccolor (colorfunc) (Uccolor in);
typedef void (skene) (float time, float time2, float *knobs, int *switches, int sceneflags);


// VARIABLES
glue_extern int eftexture_xsize;
glue_extern int eftexture_ysize;
glue_extern int glueFC;
glue_extern int glueVC;
glue_extern int glueXres;
glue_extern int glueYres;
glue_extern int glueFullscreen;
glue_extern int glueLoop;
glue_extern int glueMultisample;
glue_extern float glueAspect;
glue_extern float glueWidescreen;
glue_extern int glueSbits;
glue_extern int glueShowinformation;
glue_extern int glueShowdebug;
glue_extern int glueLoadOK;
glue_extern Texture *debugtexture;
glue_extern Texture *infotexture;
glue_extern int glueLoadsteps;
glue_extern Vertex glueMousepos;
glue_extern int glueMousekey;
glue_extern Vertex glueGolden;
glue_extern float glueGamma;
glue_extern int tinydebugline;
glue_extern int tinyinfoline;
glue_extern int glueTicktable[1024];
glue_extern int glueTicktablepos;
glue_extern float black[];
glue_extern float white[];
glue_extern float gray[];

glue_extern Texture *glueDummytex;

typedef void (glueLoadproc) (float pct);  
glue_extern glueLoadproc *glueLoadingbarproc;

// debug malloc
#define malloc(a) failmalloc(a)
void *failmalloc(int z);


// PROTOTYPES
glue_static void testit(void);  // RENAME
glue_static void glueLoading(void);
glue_static void glueBackground(float *col, int flags);
glue_static void glueClear(float *color);
glue_static void gluePrepareflags(float *col);
glue_static void gluePrepareflags2(float *col);
glue_static void glueSetmatrices2d(int flags);
glue_static void glueSetmatrices3d(float perspective, Vertex cen, float znear, float zfar, float tilt, int flags);
glue_static Camera glueDefaultcamera(float dist);
glue_static Camera glueDefaultcamera2(float t, float dist);
glue_static void glueCamera(Camera *cam, int flags);
glue_static void gluePrinterrors(void);
glue_static void gluePush(void);
glue_static void gluePop(void);
glue_static void glueFog(float *color, float min, float max);
glue_static void glueFog_toimiva(float *color, float min, float max);
glue_static void glueFog2(float *color, float power, float min, float max);
glue_static void glueLight_hard(Vertex *dir, float *color);
glue_static void glueLight_hard_nospec(Vertex *dir, float *color);
glue_static void glueLight_normal(Vertex *dir, float *color);
glue_static void glueLight_soft(Vertex *dir, float *color);
glue_static void glueLight_pos(Vertex *pos, float *color, float maxdist);
glue_static void glueStrobo(float t, float br, float times, float fadespeed);
glue_static float glueStrobof(float t, float br, float times, float fadespeed);
glue_static void glueShowzbuffer(Texture *tex, float scale);
glue_static void glueShowalphabuffer(void);
glue_static void glueResetalpha(float c, int flags);
glue_static void glueColoroverlay(float *col, int flags);
glue_static void glue2dquad(void);
glue_static void glue2dquad2(void);
glue_static void glueBackgroundcolor(float r, float g, float b, float a, int flags);
glue_static void glueWriteticktable(void);
glue_static void glueCross(float len);
glue_static void glueShowlevels(float *levels, float *averages, float threshold, float scale);
glue_static void glueXgrid(float scale);
glue_static void glueYgrid(float scale);
#ifndef INTRO
char *glueLoadfile(char *filename);
FILE *glueDatafile_pointer(char *filename);
int glueFilelength(char *filename);
#endif //INTRO

#endif //GLUE_H

