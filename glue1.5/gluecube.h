
#ifndef GLUECUBE_H
#define GLUECUBE_H

#include "glue.h"
#include "glueext.h"
#include "gluefbo.h"

glue_static Framebuffer *glueCubemap(int res);
glue_static void glueRendercubemap(Framebuffer *fbo, skene *s, float t, Vertex pos, Vertex tgt, float zmin, float zmax, int maxface);



#endif


