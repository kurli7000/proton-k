
#ifndef GLUEOVRL_H
#define GLUEOVRL_H

#ifndef FORGET_INCLUDES
#include "glue.h"
#endif

// VARIABLES


// PROTOTYPES

glue_static void glueSet2d(int flags);
glue_static void glueShowimage(Texture *texture, float xpos, float ypos, float zpos, float xsize, float ysize, float rot, float br, int flags);
glue_static void glueOverlay(Texture *texture, float br, int flags);
glue_static void glueOverlay_mt(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags, float br, Vertex scale, int flags);
glue_static void glueOverlay_mt2(Texture *tex1, int flags1, Texture *tex2, int flags2, int blendflags, float br, Vertex scale1, Vertex scale2, int flags);
glue_static void glueOverlay_alphacheck(Texture *texture, float raja, float br, int inv, int flags);
glue_static void glueOverlay_scale(Texture *texture, float br, Vertex scale, int flags);
glue_static void glueOverlay_scaletrans(Texture *texture, float br, Vertex scale, Vertex trans, int flags);
glue_static void glueOverlay_scaletranscol(Texture *texture, Vertex scale, Vertex trans, int flags);
glue_static void glueOverlay_scaletranscolrot(Texture *texture, Vertex scale, Vertex trans, float rotfo,  Vertex rot,int flags);
glue_static void glueOverlay_translate(Texture *texture, float br, Vertex trans, int flags);
glue_static void glueOverlay_zoom(Texture *texture, float br, float zoom, int flags);
glue_static void glueCrossfade(Texture *tex1, Texture *tex2, float pos, float ramp, int flags);

#endif // GLUEOVRL_H
