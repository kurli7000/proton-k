
#ifndef GLUEPART_H
#define GLUEPART_H

#ifndef FORGET_INCLUDES
#include "glue.h"
#endif


#define PART_SORT 2
#define PART_NOBEHIND 16
#define PART_CULL 1
#define PART_INVSORT 4
#define PART_ROTATE 8

typedef struct {
  float x, y, z, len;
} Vertex2;

typedef struct {
  int kunt;
  float koko;
} juttuh;

// PROTOTYPES

glue_static void particles_draw(float *v, float *t, float *c, unsigned int *i, int vc);
glue_static void particles_draw_lod(float *v, float *t1, float *t2, float *c, unsigned int *i, int vc);
glue_static float dot2(Vertex2 *v1, Vertex2 *v2);
glue_static void normalize2(Vertex2 *v);
glue_static Vertex2 v_sub2(Vertex *v1, Vertex *v2);
glue_static void ps_dontrecurse(Vertex2 *rdir, int rcnt, Vertex2 *cdir, int ccnt, float *shd, float koko, float kerroin);
static void ps_xrecurse(Vertex2 *rdir, int rcnt, Vertex2 *cdir, int ccnt, float *shd, float koko, int depth, float kerroin);
glue_static float *calc_particleshadows(Vertex *receive, int rcnt, Vertex *cast, int ccnt, Vertex *light, float koko, float br);
glue_static float *calc_particlesizes(Vertex *v, Vertex *n, int *vc, int *ind, int fc, int ybercount, float ybersize, int add_center);
glue_static Particle *glueLoadparticle(char *filename, char *filename_alpha, int xcnt, int ycnt, int flags);
glue_static Particle *glueMakeparticle(unsigned char *data, int xres, int yres, int xcnt, int ycnt, int flags);
glue_static void glueDrawparticles2(Particle *p, Vertex *vert, Vertex *norm, float *rotobuf, float *sizebuf, 
                        float *colbuf, int vc, Camera *camera, float *fadebuf, int *pnumbuf, 
                        float size, float br, int pflags, int flags);
glue_static void glueDrawparticles3(Texture *tex, Vertex *vert, Vertex *norm, float *rotobuf, float *sizebuf, 
                        float *colbuf, int vc, Camera *camera, float size, float br, int pflags, int flags);
glue_static void glueCalcparticlelods(Particle *p, Vertex *vert, float *lods, int vc, Camera *camera, float ramp);


#endif

