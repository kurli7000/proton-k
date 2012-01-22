
#ifndef GLUEMESH_H
#define GLUEMESH_H

//#include "glue.h"
#include "..\glue1.5\glueobj.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <io.h>

typedef struct {
  int numgroups;
  Object **groups;
} Mesh;

/*
typedef struct {
  unsigned int num;
  int vc, fc;
  Vertex *vertices;
  Vertex *normals;
  Vertex *texcoords;
  Color *tangents;
  int *indices;
  int *orig_indices;
  int *vertex_links;
  int linkcnt;
} Group;
*/

#endif // GLUEMESH_H
