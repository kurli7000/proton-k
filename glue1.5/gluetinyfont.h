
#ifndef GLUETINYFONT_H
#define GLUETINYFONT_H


#ifndef FORGET_INCLUDES
#include "glue.h"
#endif

// VARIABLES
glue_extern unsigned char tinyfont_bitmap[128*128];
glue_extern unsigned char texturedata_debug[128*128*4];
glue_extern unsigned char texturedata_info[128*128*4];
glue_extern unsigned long gluetinyfont[];

// PROTOTYPES
glue_static void gluemakefontbitmap(void);
glue_static void glueDebug_tiny(char *str);
glue_static void glueDebugf_tiny(char *fmt, ...);
glue_static void glueShowtinydebug(void);
glue_static void glueInfo_tiny(char *str);
glue_static void glueInfo(float t);
glue_static void init_tinyfont(void);

#endif // GLUETINYFONT_H
