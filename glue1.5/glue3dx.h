
#ifndef GLUE3DX_H
#define GLUE3DX_H

#include "glue.h"

typedef struct {
  Vertex pos, x, y, z;
  float fov;
  Vertex rvelocity, tvelocity;
  float rspeed, tspeed;
} Camera3dx;


#ifdef GLUE_USE_3DX
void get3dxdata(Vertex *translate, Vertex *rotate);
void init3dx(void);
void uninit3dx(void);
Camera3dx *glueCamera3dx(
  Vertex initpos, 
  Vertex inittgt, 
  float initfov,
  Vertex initrvelocity, 
  Vertex inittvelocity, 
  float rspeed, 
  float tspeed,
  unsigned char *name);
void glue3dxcam_exact(Camera3dx *c3dx);
void glue3dxcam_smooth(Camera3dx *c3dx, float change);
void glueWrite3dxfile(void);
#endif
void glueC3dx2cam(Camera3dx *c3dx, float t, Splinept *postab, Splinept *xtab, Splinept *ytab, Splinept *ztab, int pcnt);


#endif
