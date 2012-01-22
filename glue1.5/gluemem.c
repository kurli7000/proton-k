
#ifndef FORGET_INCLUDES
#include "glue.h"
#include "gluemem.h"
#ifdef I_LOVE_BILL
#include "gluewin.h"
#else
#include "gluelin.h"
#endif
#endif // FORGET_INCLUDES

/**********************************************************************
  Memory stuff
**********************************************************************/

char tmpmbuf[32*1048576];
char *tmpmptr=tmpmbuf;

/* allocate temporary memory. works like a stack. upon tmpfreeing everything else tmpmalloced after it gets freed as well */
glue_static void *tmpmalloc(size_t bytes) {
  tmpmptr = alignup(tmpmptr, 16);
  if ((size_t)(tmpmbuf+sizeof(tmpmbuf)-tmpmptr) < bytes) { glueError("out of tempmalloc"); return 0; }
    else return (tmpmptr += bytes)-bytes;
}

glue_static void tmpfree(void *p) {
  if ((char*)p>tmpmptr) glueError("tmpfree error");
  tmpmptr = p;
}
