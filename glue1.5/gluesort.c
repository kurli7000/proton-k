
#ifndef FORGET_INCLUDES

#include "gluesort.h"
#include "glue.h"
#ifdef I_LOVE_BILL
#include "gluewin.h"
#else
#include "gluelin.h"
#endif

#endif  //FORGET_INCLUDES

#include <assert.h>

Sortthing radix_buffer1[262144];
Sortthing radix_buffer2[262144];


glue_static void glueRadixsort(unsigned int count) {
  unsigned *u;
  static unsigned int offsets[1024];
  unsigned int i, sum, t;

  if (count>262144) glueError("dumbass");
  assert(sizeof(float) == sizeof(unsigned));

  for (i = 0; i<1024; i++) offsets[i] = 0;

  for (i = 0; i<count; i++) {
    u = (unsigned *)&radix_buffer1[i].f;
    offsets[DIGIT(u, 0)    ]++;
    offsets[DIGIT(u, 1)+256]++;
    offsets[DIGIT(u, 2)+512]++;
    offsets[DIGIT(u, 3)+768]++;
  }

  sum = 0;
  for (i=0; i<1024; i++) {
    t = offsets[i];
    offsets[i] = sum;
    sum = (i&255) == 255 ? 0 : sum + t;
  }

  for (i=0; i<count; i++) u = (unsigned *)&radix_buffer1[i].f, radix_buffer2[offsets[DIGIT(u, 0)    ]++] = radix_buffer1[i];
  for (i=0; i<count; i++) u = (unsigned *)&radix_buffer2[i].f, radix_buffer1[offsets[DIGIT(u, 1)+256]++] = radix_buffer2[i];
  for (i=0; i<count; i++) u = (unsigned *)&radix_buffer1[i].f, radix_buffer2[offsets[DIGIT(u, 2)+512]++] = radix_buffer1[i];
  for (i=0; i<count; i++) u = (unsigned *)&radix_buffer2[i].f, radix_buffer1[offsets[DIGIT(u, 3)+768]++] = radix_buffer2[i];
}


#undef FLTCONV 
#undef DIGIT 



  

