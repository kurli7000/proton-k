
#ifndef GLUEDENSGRID_H
#define GLUEDENSGRID_H

#ifndef FORGET_INCLUDES
#include "glue.h"
#include "gluegen.h"
#include "gluewin.h"
#endif // FORGET INCLUDES


#define DG_gsize 32
#define DG_zshift 10
#define DG_yshift 5

/*
#define DG_gsize 64
#define DG_zshift 12
#define DG_yshift 6
*/

#define DG_maxfaces DG_gsize*DG_gsize*DG_gsize*5
#define DG_maxverts DG_maxfaces*3

#define gpos(x, y, z) (((z)<<DG_zshift)+((y)<<DG_yshift)+(x))

glue_extern int DG_normalize;
glue_extern int DG_calc_normals;

typedef struct {
  float f;
  int xp, yp, zp;
  int index, io;
} Densgrid;


glue_extern Densgrid dgrid[DG_gsize*DG_gsize*DG_gsize];
glue_extern Vertex Densgrid_vertices[DG_maxverts];
glue_extern Vertex Densgrid_normals[DG_maxverts];
glue_extern int Densgrid_indices[DG_maxfaces*3];
glue_extern int Densgrid_vpos, Densgrid_ipos;
glue_static void showgrid(float *color);
glue_static void glueDensgrid_to_mesh(float raja);


#endif

