#ifndef LODDER_H
#define LODDER_H

#include "..\glue1.5\glue.h"
#include "gluemesh.h"
#include "..\glue1.5\gluewin.h"
#include "..\glue1.5\gluegen.h"
#include "..\glue1.5\gluemem.h"
#include "..\glue1.5\gluetxtr.h"
#include "..\glue1.5\glueext.h"
#include "..\glue1.5\glueglsl.h"

//#define DEBUG_LODDER 
#define MAX_LOD_REDUCTION 6
#define MAX_GROUPS 32
#define NPAR -1

typedef struct {
  Vertex pos;
  int parent;
} Mvert;

void meshlodder(Mesh *mesh);
Vertex *calc_lodder_normals(Vertex *vertices, int vc, int *indices, int fc);
int mergevertices(Mvert *data, int vc, int *indices, int fc);
Vertex *unpack_mvbuffer(Mvert *data, int vc);
void render_lodder_sphere(int group, Vertex spherepos, Vertex spherepos2, float diam, float powr, float push, float t);
void calc_fnorm_nn(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *n);

#endif
