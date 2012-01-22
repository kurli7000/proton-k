#ifndef GLUEFONT_H
#define GLUEFONT_H

#include "glue.h"

Font *glueLoadfont(char *filename, char *filename_alpha, unsigned char *indextab, int flags);
void gluePrintf(Font *f, float xpos, float ypos, int swapxy, float rotate, float scale, float spacing, int center, float *colbuf, Vertex *offsets, int flags, char *fmt, ...);
void gluePrintf2(Font *f, float xpos, float ypos, int swapxy, float rotate, float scalex, float scaley, float spacing, int center, float *colbuf, int flags, char *fmt, ...);
float gluePrint_length(Font *f, float scale, float spacing, char *fmt, ...);

#endif

