#ifndef GLUENOISE_H
#define GLUENOISE_H

#include "glue.h"
#include "gluegen.h"

glue_static int glueNoise(int x, int y, int z);
glue_static int glueNoise_loop(int x, int y, int z, int loopandx, int loopandy, int loopandz);
glue_static int glueNoise_sharp(int x, int y, int z);


#endif
