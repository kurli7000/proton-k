#ifndef GLUESORT_H
#define GLUESORT_H



#ifndef FORGET_INCLUDES
#include "glue.h"
#endif

#define FLTCONV(x) ((x) ^ (0U-((x)>>31)>>1) ^ 0x80000000)
#define DIGIT(v, p) ((FLTCONV(v[0])>>(p)*8)&255)



typedef struct {
  float f;
  unsigned int i;
} Sortthing;

// VARIABLES
glue_extern Sortthing radix_buffer1[262144];
glue_extern Sortthing radix_buffer2[262144];

// PROTOTYPES

glue_static void glueRadixsort(unsigned int count);

//#undef FLTCONV 
//#undef DIGIT 


#endif
  

