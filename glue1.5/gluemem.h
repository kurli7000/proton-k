
#ifndef GLUEMEM_H
#define GLUEMEM_H

#include <malloc.h>

/* returns p aligned up for element size x (power of 2) */
#define alignup(p, x) ((char*)p+(0u-(unsigned)p & (unsigned)(x-1)))

glue_extern char tmpmbuf[32*1048576];
glue_extern char *tmpmptr;



glue_static void *tmpmalloc(size_t bytes);
glue_static void tmpfree(void *p);

#endif // GLUEMEM_H
