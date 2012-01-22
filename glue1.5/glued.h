
#ifndef GLUED_H
#define GLUED_H

#include <time.h>
#include <malloc.h>


#ifndef FORGET_INCLUDES
#include "glue.h"
#endif

#define glueParentContext(c) ((c)->parent)
#define glueIsRootContext(c) ((c)==0)
#define GLUECALL(fun, par) do { GlueContext da_ctx=glueStartStuff(#fun); { fun par; } glueStuffDone(da_ctx); } while (0)
#define GLUESTART(wtf) do { GlueContext da_ctx=glueStartStuff(wtf)
#define GLUEDONE()  glueStuffDone(da_ctx); } while (0)

typedef clock_t GlueInterval;
typedef struct glueContext *GlueContext;
struct glueContext { char *desc; struct glueContext *parent; double tick; };

// VARIABLES

glue_extern GlueContext glueCurrentContext;


// PROTOTYPES
glue_static GlueInterval glueStartInterval(void);
glue_static float glueStopInterval(GlueInterval start);
glue_static GlueContext glueStartStuff(char *action);
glue_static void glueStuffDone(GlueContext c);

#endif // GLUED_H

