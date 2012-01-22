#ifndef GLUEMPEG_H
#define GLUEMPEG_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "glue.h"
#include "gluewin.h"
#include "gluemem.h"
#include "gluetxtr.h"
#include "libmpeg2\mpeg2.h"
#include "libmpeg2\mpeg2convert.h"

#define MPEG_BUFFER_SIZE 4096

typedef struct {
  mpeg2dec_t *decoder;
  const mpeg2_info_t *info;
  mpeg2_state_t state;
  int framenum;
  unsigned char *data;
  int stream_from_disk;
  FILE *f;
  size_t datasize;
  size_t whereami;
  float prevtime;
  int end;
} Mpeg2;


Mpeg2 *glueMpeg2_open(char *filename, int stream_from_disk);
//void glueMpeg2_get(Mpeg2 *mpg, Texture *dst, int sync_hannel, float fps, int border, int skip_frames);
void glueMpeg2_get(Mpeg2 *mpg, Texture *dst, int sync_channel, float fps, int border, int skip_frames, int reset_on_error);
void glueMpeg2_reset(Mpeg2 *mpg); // resets all variables, video starts from the first frame
void glueMpeg2_close(Mpeg2 *mpg);


#endif
