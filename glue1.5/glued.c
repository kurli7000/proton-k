

#ifndef FORGET_INCLUDES
#include "glued.h"
#endif

GlueContext glueCurrentContext = 0;

glue_static GlueInterval glueStartInterval(void) {
  return clock();
}

glue_static float glueStopInterval(GlueInterval start) {
  return (clock_t)(clock()-start) / (float)CLOCKS_PER_SEC;
}



/* start doing some action. */
glue_static GlueContext glueStartStuff(char *action) {
  struct glueContext *a = malloc(sizeof *a);
  a->desc = action;
  a->parent = glueCurrentContext;
  a->tick = glueStartInterval();
  glueCurrentContext = a;
  return a;
}

/* called when given action finished */
glue_static void glueStuffDone(GlueContext c) {
/*  glueAssert(c!=0); glueAssert(glueCurrentContext==c);*/
  glueCurrentContext = c->parent;
  //fprintf(stderr, "%s in %.4fs\n", c->desc, glueStopInterval(c->tick));
  free(c);
}








