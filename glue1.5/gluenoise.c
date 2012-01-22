#include "gluenoise.h"


static int nbuf[1024];
static int inoise(int x, int y, int z) {
  return nbuf[nbuf[nbuf[z&1023]+y&1023]+x&1023];
}



glue_static void precalc_gluenoise(void) {
  int i, x, y;
  static int kerran=0;
  

  
  if (kerran>0) return;
  kerran++;
  
  for (x=0; x<1024; x++) {
    nbuf[x]=x;
  }

  for (i=0; i<1024; i++) {
    x=random()&1023;
    y=nbuf[i];
    nbuf[i]=nbuf[x];
    nbuf[x]=y;
  }
  
}




glue_static int glueNoise(int x, int y, int z) {
  int xx,yy,zz,x2,y2,z2;

  precalc_gluenoise();

  xx=x&1023;
  yy=y&1023;
  zz=z&1023;
  xx=xx*xx*(1536-xx)>>19;
  yy=yy*yy*(1536-yy)>>19;
  zz=zz*zz*(1536-zz)>>19;
  x2=1024-xx;
  y2=1024-yy;
  z2=1024-zz;
  x>>=10;
  y>>=10;
  z>>=10;

  return ((inoise(x, y  , z  )*x2+inoise(x+1, y  , z  )*xx)*y2
         +(inoise(x, y+1, z  )*x2+inoise(x+1, y+1, z  )*xx)*yy>>15)*z2
        +((inoise(x, y  , z+1)*x2+inoise(x+1, y  , z+1)*xx)*y2
         +(inoise(x, y+1, z+1)*x2+inoise(x+1, y+1, z+1)*xx)*yy>>15)*zz>>15;
}

static int inoiseloop(int x, int y, int z, int loopandx, int loopandy, int loopandz) {
  return nbuf[nbuf[nbuf[z&loopandz]+y&loopandy]+x&loopandx];
}


glue_static int glueNoise_loop(int x, int y, int z, int loopandx, int loopandy, int loopandz) {
  int xx,yy,zz,x2,y2,z2;

  precalc_gluenoise();

  xx=x&1023;
  yy=y&1023;
  zz=z&1023;
  xx=xx*xx*(1536-xx)>>19;
  yy=yy*yy*(1536-yy)>>19;
  zz=zz*zz*(1536-zz)>>19;
  x2=1024-xx;
  y2=1024-yy;
  z2=1024-zz;
  x>>=10;
  y>>=10;
  z>>=10;

  return ((inoiseloop(x, y  , z  , loopandx, loopandy, loopandz)*x2+inoiseloop(x+1, y  , z  , loopandx, loopandy, loopandz)*xx)*y2
         +(inoiseloop(x, y+1, z  , loopandx, loopandy, loopandz)*x2+inoiseloop(x+1, y+1, z  , loopandx, loopandy, loopandz)*xx)*yy>>15)*z2
        +((inoiseloop(x, y  , z+1, loopandx, loopandy, loopandz)*x2+inoiseloop(x+1, y  , z+1, loopandx, loopandy, loopandz)*xx)*y2
         +(inoiseloop(x, y+1, z+1, loopandx, loopandy, loopandz)*x2+inoiseloop(x+1, y+1, z+1, loopandx, loopandy, loopandz)*xx)*yy>>15)*zz>>15;
}


glue_static int glueNoise_sharp(int x, int y, int z) {
  int xx,yy,zz,x2,y2,z2;

  precalc_gluenoise();

  xx=x&1023;
  yy=y&1023;
  zz=z&1023;
  x2=1024-xx;
  y2=1024-yy;
  z2=1024-zz;
  x>>=10;
  y>>=10;
  z>>=10;

  return ((inoise(x, y  , z  )*x2+inoise(x+1, y  , z  )*xx)*y2
         +(inoise(x, y+1, z  )*x2+inoise(x+1, y+1, z  )*xx)*yy>>15)*z2
        +((inoise(x, y  , z+1)*x2+inoise(x+1, y  , z+1)*xx)*y2
         +(inoise(x, y+1, z+1)*x2+inoise(x+1, y+1, z+1)*xx)*yy>>15)*zz>>15;
}



