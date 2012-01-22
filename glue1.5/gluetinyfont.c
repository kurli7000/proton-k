
#ifndef FORGET_INCLUDES
#include "gluetinyfont.h"
#include "gluetxtr.h"
#include "glue.h"
#include "gluegen.h"
#include "string.h"
#include "fmodcrap.h"
#include <GL/gl.h>
#include <time.h>
#endif


unsigned char tinyfont_bitmap[128*128];
unsigned char texturedata_debug[128*128*4];
unsigned char texturedata_info[128*128*4];

unsigned long gluetinyfont[] = {
  /*0-15*/
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /*16-31*/
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  /*32-47*/
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

/*  48-63
 1 6  XX     4  X      c XX      c XX      8 X       e XXX     4  X      e XXX     4  X      4  X
 2 a X X     c XX      2   X     2   X     a X X     8 X       8 X       2   X     a X X     a X X
 3 a X X     4  X      4  X      4  X      e XXX     c XX      c XX      4  X      4  X      6  XX
 4 a X X     4  X      8 X       2   X     2   X     2   X     a X X     4  X      a X X     2   X
 5 c XX      4  X      e XXX     c XX      2   X     c XX      4  X      4  X      4  X      4  X                                           */
   0x6aaac00,0x4c44400,0x4248e00,0xc242c00,0x8ae2200,0xe8c2c00,0x48ca400,0xe244400,0x4a4a400,0x4a62400,0x0000000,0x0000000,0x0000000,0x0000000,0x0000000,0x0000000,

/*  64-79
 1 4  X      4  X      c XX      6  XX     c XX      e XXX     e XXX     6  XX     a X X     4  X      2   X     a X X     8 X       a X X     a X X     4  X
 2 a X X     a X X     a X X     8 X       a X X     8 X       8 X       8 X       a X X     4  X      2   X     a X X     8 X       e XXX     e XXX     a X X
 3 a X X     e XXX     c XX      8 X       a X X     c XX      c XX      a X X     e XXX     4  X      2   X     c XX      8 X       e XXX     e XXX     a X X
 4 8 X       a X X     a X X     8 X       a X X     8 X       8 X       a X X     a X X     4  X      2   X     a X X     8 X       a X X     e XXX     a X X
 5 4  X      a X X     c XX      6  XX     c XX      e XXX     8 X       6  XX     a X X     4  X      c XX      a X X     e XXX     a X X     a X X     4  X         */
   0x4aa8400,0x4aeaa00,0xcacac00,0x6888600,0xcaaac00,0xe8c8e00,0xe8c8800,0x68aa600,0xaaeaa00,0x4444400,0x2222c00,0xaacaa00,0x8888e00,0xaeeaa00,0xaeeea00,0x4aaa400,

/*  80-95
 1 c XX      4  X      c XX      e XXX     e XXX     a X X     a X X     a X X     a X X     a X X     e XXX
 2 a X X     a X X     a X X     8 X       4  X      a X X     a X X     a X X     a X X     a X X     2   X
 3 c XX      a X X     c XX      e XXX     4  X      a X X     a X X     e XXX     4  X      4  X      4  X
 4 8 X       a X X     a X X     2   X     4  X      a X X     4  X      e XXX     a X X     4  X      8 X
 5 8 X       6  XX     a X X     e XXX     4  X      4  X      4  X      a X X     a X X     4  X      e XXX   */
   0xcac8800,0x4aaa600,0xcacaa00,0xe8e2e00,0xe444400,0xaaaa400,0xaaa4400,0xaaeea00,0xaa4aa00,0xaa44400,0xe248e00,0x0000000,0x0000000,0x0000000,0x0000000,0x0000000,

/* 96-111
 1 0         4  X      8 X       0         2   X     0         2   X     0         8 X       4  X      4  X      8 X       4  X      0         0         0
 2 0         2   X     8 X       0         2   X     4  X      4  X      0         8 X       0         0         8 X       4  X      0         0         0
 3 0         6  XX     c XX      6  XX     6  XX     e XXX     6  XX     4  X      c XX      4  X      4  X      a X X     4  X      c XX      c XX      4  X
 4 0         a X X     a X X     8 X       a X X     8 X       4  X      a X X     a X X     4  X      4  X      c XX      4  X      e XXX     a X X     a X X
 5 0         6  XX     4  X      6  XX     4  X      6  XX     4  X      6  XX     a X X     4  X      4  X      a X X     2   X     a X X     a X X     4  X
 6 0         0         0         0         0         0         4  X      c XX      0         0         8 X       0         0         0         0         0               */
   0x0000000,0x426a600,0x88ca400,0x0068600,0x226a400,0x04e8600,0x2464440,0x004a6c0,0x88caa00,0x4044400,0x4044480,0x88aca00,0x4444200,0x00cea00,0x00caa00,0x004a400,

/* 112-127
 1 0         0         0         0         4  X      0         0         0         0         0         0
 2 0         0         0         0         6  XX     0         0         0         0         0         0
 3 4  X      4  X      6  XX     6  XX     4  X      a X X     a X X     a X X     a X X     a X X     c XX
 4 a X X     a X X     8 X       4  X      4  X      a X X     a X X     e XXX     4  X      4  X      4  X
 5 c XX      6  XX     8 X       c XX      2   X     6  XX     4  X      6  XX     a X X     4  X      6  XX
 6 8 X       2   X     0         0         0         0         0         0         0         0         0                                                */
   0x004ac80,0x004a620,0x0068800,0x0064c00,0x4644300,0x00aa600,0x00aa400,0x00ae600,0x00a4a00,0x00a4400,0x00c4600,0x0000000,0x0000000,0x0000000,0x0000000,0x0000000,

/* 128-255 */
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};




glue_static void gluemakefontbitmap(void) {
  int x, y, ch;

  for (ch=0; ch<256; ch++) for (y=0; y<8; y++) for (x=0; x<4; x++)
    tinyfont_bitmap[(ch&15)*8+((ch>>4)&15)*8*128+y*128+x] = (gluetinyfont[ch]<<(y*4+x)) & 0x80000000 ? 255 : 0;

}



glue_static void glueDebug_tiny(char *str) {
  int x, y, ch;

  if (tinydebugline==0) memset(texturedata_debug, 0, 128*128*4);

  for (ch=0; str[ch]!=0; ch++) {
    for (y=0; y<8; y++) for (x=0; x<4; x++) {
      unsigned char c;

      c=tinyfont_bitmap[(str[ch]&15)*8+(str[ch]>>4&15)*8*128+y*128+x];

      texturedata_debug[(tinydebugline*9+y)*512+(x+ch*5+2)*4+0]=c;
      texturedata_debug[(tinydebugline*9+y)*512+(x+ch*5+2)*4+1]=c;
      texturedata_debug[(tinydebugline*9+y)*512+(x+ch*5+2)*4+2]=c;
      texturedata_debug[(tinydebugline*9+y)*512+(x+ch*5+2)*4+3]=c;
    }
  }

  tinydebugline++;
}


glue_static void glueDebugf_tiny(char *fmt, ...) {
  static char buf[8000];
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  glueDebug_tiny(buf);
  va_end(ap);
}

glue_static void glueShowtinydebug(void) {
  float u=256.0/glueXres;
  float v=256.0/(glueYres*glueWidescreen);

/*
  for (y=0; y<128; y++) for (x=0; x<128; x++) {
    texturedata[y*128*4+x*4+0]=tinyfont_bitmap[y*128+x];
    texturedata[y*128*4+x*4+1]=tinyfont_bitmap[y*128+x];
    texturedata[y*128*4+x*4+2]=tinyfont_bitmap[y*128+x];
    texturedata[y*128*4+x*4+3]=tinyfont_bitmap[y*128+x];
  }
*/

  glueReloadtexture(debugtexture, texturedata_debug);

  glueSetmatrices2d(0);
  glueBindtexture(debugtexture, GLUE_NEAREST);

  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glColor4f(1, 1, 1, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

  glBegin(GL_POLYGON);
  glTexCoord2f(0, 1); glVertex2f(-1,   -1);
  glTexCoord2f(1, 1); glVertex2f(-1+u, -1);
  glTexCoord2f(1, 0); glVertex2f(-1+u, -1+v);
  glTexCoord2f(0, 0); glVertex2f(-1,   -1+v);
  glEnd();

}




glue_static void glueInfo_tiny(char *str) {
  int x, y, ch;

  if (tinyinfoline==0) memset(texturedata_info, 0, 128*128*4);

  for (ch=0; str[ch]!=0; ch++) {
    for (y=0; y<8; y++) for (x=0; x<4; x++) {
      unsigned char c;

      c=tinyfont_bitmap[(str[ch]&15)*8+(str[ch]>>4&15)*8*128+y*128+x];

      texturedata_info[(tinyinfoline*9+y)*512+(x+ch*5)*4+0]=c;
      texturedata_info[(tinyinfoline*9+y)*512+(x+ch*5)*4+1]=c;
      texturedata_info[(tinyinfoline*9+y)*512+(x+ch*5)*4+2]=c;
      texturedata_info[(tinyinfoline*9+y)*512+(x+ch*5)*4+3]=c;
    }
  }

  tinyinfoline++;
}


glue_static void glueInfo(float t) {
  float u=256.0/glueXres;
  float v=256.0/(glueYres);//*glueWidescreen);
  int zbits, rbits, gbits, bbits, sbits, abits;
  char *version;
  int minor, major;
  static double fps=0;//, cumfps=0, avgfps=0;
  static double msec=0, pmsec=0;
  static int frames=0;
  char *stringi=malloc(50);


  glFinish();
  

  version=(char *)glGetString(GL_VERSION);
  sscanf(version, "%d.%d", &major, &minor);

  glGetIntegerv(GL_DEPTH_BITS, &zbits);
  glGetIntegerv(GL_RED_BITS, &rbits);
  glGetIntegerv(GL_GREEN_BITS, &gbits);
  glGetIntegerv(GL_BLUE_BITS, &bbits);
  glGetIntegerv(GL_ALPHA_BITS, &abits);
  glGetIntegerv(GL_STENCIL_BITS, &sbits);

/*
  if (frames>0) {
    fps=1000.0/(float)(msec-pmsec);
    cumfps+=fps;
    avgfps=cumfps/frames;
  }
  */
  
  ONCE ( 
    pmsec=clock()/(float)CLOCKS_PER_SEC*1000.0;
  );
  
  msec=clock()/(float)CLOCKS_PER_SEC*1000.0;

  if (msec>pmsec+1000) {
    pmsec=msec;
    fps=frames;
    frames=0;
  }

  frames++;
  //pmsec=msec;

  sprintf(stringi, "time: %5.2f  row:%i", t, (int)floor(fmod(t, 1)*64));
  glueInfo_tiny(stringi);
  sprintf(stringi, "fps: %6.4f", fps);//, avgfps);
  glueInfo_tiny(stringi);
  sprintf(stringi, "vc:%i pc:%i", glueVC, glueFC);
  glueInfo_tiny(stringi);
  sprintf(stringi, "bpp:%i  alpha:%i", rbits+gbits+bbits, abits);
  glueInfo_tiny(stringi);
  sprintf(stringi, "zbuffer:%i  stencil:%i", zbits, sbits);
  glueInfo_tiny(stringi);
/*
  int diff=get_timerdifference();
  static int tdmin=1000000, tdmax=0;
  static float tdavg=23.25;
  if (diff<tdmin) tdmin=diff;
  if (diff>tdmax) tdmax=diff;
  tdavg=tdavg*0.99+diff*0.01;
  sprintf(stringi, "diff:%i min:%i max:%i", diff, tdmin, tdmax);
  glueInfo_tiny(stringi); 
  sprintf(stringi, "diff avg:%5.2f", tdavg);
  glueInfo_tiny(stringi); 
*/

  free(stringi);


  glueReloadtexture(infotexture, texturedata_info);

  glueSetmatrices2d(0);
  glViewport(0, 0, glueXres, glueYres);
  glueBindtexture(infotexture, GLUE_NEAREST);

  glDisable(GL_TEXTURE_1D);
  glDisable(GL_LIGHTING);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glColor4f(1, 1, 1, 1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

  glBegin(GL_POLYGON);
  glTexCoord2f(0, 1); glVertex2f(-1.0+0.01,   1.0-0.02-v);
  glTexCoord2f(1, 1); glVertex2f(-1.0+0.01+u, 1.0-0.02-v);
  glTexCoord2f(1, 0); glVertex2f(-1.0+0.01+u, 1.0-0.02);
  glTexCoord2f(0, 0); glVertex2f(-1.0+0.01,   1.0-0.02);
  glEnd();

}



glue_static void init_tinyfont(void) {
  gluemakefontbitmap();
  debugtexture=glueColortexture(128, 128, 0, 0, 0, 0, 0);
  infotexture=glueColortexture(128, 128, 0, 0, 0, 0, 0);
}






