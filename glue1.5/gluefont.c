
#include "gluefont.h"
#include "gluetxtr.h"
#include "gluegen.h"
#include "glueovrl.h"
#ifdef I_LOVE_BILL
#include "gluewin.h"
#else
#include "gluelih.h"
#endif
#include <gl/gl.h>

Font *glueLoadfont(char *filename, char *filename_alpha, unsigned char *indextab, int flags) {
  Font *f=malloc(sizeof(Font));
  int x, y, xx, yy;
  unsigned char *tmpdata;
  int xres, yres;
  int xstart, xend;
  int threshold=8;

  f->tex=glueLoadtexture(filename, filename_alpha, 0);
  tmpdata=load_jpeg(filename, &xres, &yres);

  for (x=0; x<256; x++) f->postab[x]=new_v(0, 0, 0), f->widthtab[x]=0;

  for (y=0; y<8; y++) for (x=0; x<8; x++) {
    int i=indextab[y*8+x];

    f->postab[i]=new_v(x/8.0, y/8.0, 0);

    // etit‰‰n vasen reuna
    xstart=-1;
    for (xx=0; xx<xres/8 && xstart==-1; xx++) for (yy=0; yy<yres/8; yy++) {
      int u, v;

      u=xx+x*xres/8;
      v=yy+y*yres/8;

      if (tmpdata[(v*xres+u)*3]>threshold) {
        xstart=xx;
        continue;
      }
    }

    // etit‰‰n oikee reuna
    xend=-1;
    for (xx=xres/8-1; xx>=0 && xend==-1; xx--) for (yy=0; yy<yres/8; yy++) {
      int u, v;

      u=xx+x*xres/8;
      v=yy+y*yres/8;

      if (tmpdata[(v*xres+u)*3]>threshold) {
        xend=xx;
        continue;
      }
    }

    if (xstart==-1 || xend==-1 || xstart==xend) glueErrorf("les erreurs du not finding le start/end of font no %i, start=%i, end=%i", i, xstart, xend);
    f->widthtab[i]=(float)(xend-xstart)/(xres/8.0);
  }

  free(tmpdata);

  return f;
}



void gluePrintzzzbla(Font *f, float spacing, int center, float *colbuf, Vertex *offsets, char *str) {
  int ch;

  if (center) {
    float lev;

    lev=0;
    for (ch=0; str[ch]!=0; ch++) {
      if (str[ch]==32) {
        lev+=0.5;
      } else {
        lev+=(f->widthtab[str[ch]&255]+0.05)*spacing;
      }
    }

    glTranslatef(-lev*center, 0, 0);
  }

  for (ch=0; str[ch]!=0; ch++) {
    float x1, y1, x2, y2;

    if (str[ch]==32) {
      glTranslatef(1, 0, 0);
    } else {
      float w=f->widthtab[str[ch]&255]+0.05;

      x1=f->postab[str[ch]&255].x;
      y1=f->postab[str[ch]&255].y;
      x2=x1+1/8.0;
      y2=y1+1/8.0;

      if (colbuf) glColor4f(colbuf[ch*4+0], colbuf[ch*4+1], colbuf[ch*4+2], colbuf[ch*4+3]);

      if (ch==0) glTranslatef(1.0, 0, 0);
      glPushMatrix();
      if (offsets) glTranslatef(offsets[ch].x, offsets[ch].y, 0);
      glTranslatef(-(1.0-w), 0, 0);
      glBegin(GL_QUADS);
      glTexCoord2f(x1, y1); glVertex2f(-1,  1);
      glTexCoord2f(x2, y1); glVertex2f( 1,  1);
      glTexCoord2f(x2, y2); glVertex2f( 1, -1);
      glTexCoord2f(x1, y2); glVertex2f(-1, -1);
      glEnd();
      glPopMatrix();
      glTranslatef(w*spacing*2.0, 0, 0);
    }
  }

  glColor4f(1, 1, 1, 1);
}


void gluePrintf(Font *f, float xpos, float ypos, int swapxy, float rotate, float scale, float spacing, int center, float *colbuf, Vertex *offsets, int flags, char *fmt, ...) {
  static char buf[8000];
  va_list ap;
  Texture *tex=f->tex;
  float bla=(float)tex->yres/(float)tex->xres;

  gluePush();

  glueSet2d(flags|GLUE_NO_CULL|GLUE_NO_DEPTH);
  glRotatef(rotate, 0, 0, 1);
  glTranslatef(xpos, ypos, 0);
  glScalef(scale, scale*bla, 1);

  glScalef(1.0/glue_widescreen, 1, 1);

  if (swapxy) {
    glRotatef(90, 0, 0, 1);
  }

  glueBindtexture(f->tex, flags);
  //glColor4f(1, 1, 1, 1);

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  gluePrintzzzbla(f, spacing, center, colbuf, offsets, buf);
  va_end(ap);

  gluePop();
}

void gluePrintf2(Font *f, float xpos, float ypos, int swapxy, float rotate, float scalex, float scaley, float spacing, int center, float *colbuf, int flags, char *fmt, ...) {
  static char buf[8000];
  va_list ap;
  Texture *tex=f->tex;
  float bla=(float)tex->yres/(float)tex->xres;

  gluePush();

  glueSet2d(flags|GLUE_NO_CULL|GLUE_NO_DEPTH);
//  glueSet2d(flags|GLUE_NO_CULL);
  glRotatef(rotate, 0, 0, 1);
  glTranslatef(xpos, ypos, 0);
  glScalef(scalex, scaley*bla, 1);

  glScalef(1.0/glue_widescreen, 1, 1);

  if (swapxy) {
    glRotatef(90, 0, 0, 1);
  }

  glueBindtexture(f->tex, flags);

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  gluePrintzzzbla(f, spacing, center, colbuf, 0, buf);
  va_end(ap);

  gluePop();
}

float gluePrint_length(Font *f, float scale, float spacing, char *fmt, ...) {
  static char buf[8000];
  va_list ap;
  float lev;
  int ch;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  va_end(ap);

  lev=0;
  for (ch=0; buf[ch]!=0; ch++) {
    if (buf[ch]==32) {
      lev+=0.5;
    } else {
      lev+=(f->widthtab[buf[ch]&255]+0.05)*spacing;
    }
  }

  return lev*scale*2;
}
