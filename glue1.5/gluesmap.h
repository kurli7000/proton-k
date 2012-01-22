#ifndef GLUESMAP_H
#define GLUESMAP_H

#include "glue.h"
#include "gluetxtr.h"
#include "glueovrl.h"

typedef struct {
  int texnum;
  int xres, yres;
  float lviewmat[16];
  float lprojmat[16];
} Shadowmap;


glue_static void glueShadowmap_render(skene *efekti, Camera *light, float aspect, float time1, float time2, float *knobs, int *switches, Shadowmap *dest);
glue_static Shadowmap *glueShadowmap(int xres, int yres);
glue_static void glueBindtexture_shadow(Shadowmap *sm, float ambient); 
glue_static void glueBindtexture_shadowprojector(Shadowmap *sm, Texture *tex);
glue_static void glueShowshadowmap(Shadowmap *sm);




#endif
