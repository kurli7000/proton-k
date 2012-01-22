
#ifndef GLUEOBJ_H
#define GLUEOBJ_H

#ifndef FORGET_INCLUDES
#include "glue.h"
#endif

// VARIABLES


// PROTOTYPES
glue_static void glue_disableallarrays(void);
glue_static void buildlist(int list, Vertex *v, Vertex *n, Vertex *t, Vertex *c, int *ind, int vc, int fc);
glue_static Vertex *glueTexgen_normals(Vertex *norm, int vc);
glue_static Vertex *glueVertexcolor_normals(Vertex *norm, int vc);
glue_static Object *glueLoadobject(int *indices, Vertex *vertices, Vertex *texcoords, int *neighbours, Vertex *colors, Edge *edges, int *orig_indices, 
                       int *vertex_links, int vc, int fc, int ec, int linkcnt, int orig_vc, int flgs);
glue_static Object *glueLoadobject_flat(int *indices, Vertex *vertices, Vertex *texcoords, int *neighbours, Vertex *colors, Edge *edges, int *orig_indices,
                       int *vertex_links, int vc, int fc, int ec, int linkcnt, int orig_vc, int flgs);                       
glue_static void renderflags(int flgs);
glue_static void glueDrawobject(Object *obj, int flags);
glue_static void glueDrawobject_fast(Object *obj);
glue_static void glueDrawlists2(Vertex *v, Vertex *n, Vertex *t, float *c, int *i, int vc, int fc, int flags);
glue_static void glueDrawlists(Vertex *v, Vertex *t, int *i, int vc, int fc, int flags);
glue_static void glueShownormals(Object *obj, float len_vn, float len_fn, int flags);

glue_static Object *glueCone(float length, float radius, int slices, int flags);
glue_static Object *glueCube(float r, int flags);
glue_static Object *glueSphere(int xslices, int yslices, int flags);
glue_static Object *glueSphere_wrap(int xslices, int yslices, int flags);
glue_static Object *glueSemisphere(int xslices, int yslices, int flags);
glue_static Object *gluePlane(int xsize, int ysize, Vertex scale, int flags);
glue_static Object *glueCylinder(int xslices, int yslices, int flags);
glue_static void calc_objnormals(Object *obj);
glue_static void calc_objtangents(Object *obj);
glue_static void buildlist2(int list, Vertex *v, Vertex *n, Vertex *t, Vertex *c, int *ind, int vc, int fc, Color *tangents);


#endif // GLUEOBJ_H

