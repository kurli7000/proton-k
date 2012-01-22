
#ifndef FORGET_INCLUDES

#include "gluepart.h"
#include "gluemem.h"
#include "gluegen.h"
#include "glueext.h"
#include "glueobj.h"
#include "glued.h"
#include "gluetxtr.h"
#include "gluesort.h"
#ifdef I_LOVE_BILL
#include "gluewin.h"
#endif
#include <GL/gl.h>

#endif

/**********************************************************************
  Particle stuff
**********************************************************************/





glue_static void particles_draw(float *v, float *t, float *c, unsigned int *i, int vc) {

  glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

  glue_disableallarrays();
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_FLOAT, 0, c);
  glClientActiveTexture(GL_TEXTURE0_ARB);
  glActiveTexture(GL_TEXTURE0_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, t); 
  
#ifdef I_LOVE_BILL
  GLUECALL(glDrawElements, (GL_QUADS, vc, GL_UNSIGNED_INT, i)); 
#else
  GLUECALL(glDrawRangeElements, (GL_QUADS, 0, vc-1, vc, GL_UNSIGNED_INT, i)); 
#endif 

  glPopClientAttrib();
}


glue_static void particles_draw_lod(float *v, float *t1, float *t2, float *c, unsigned int *i, int vc) {

  glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

  glue_disableallarrays();
    
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_FLOAT, 0, c);

  // tu1
  glClientActiveTexture(GL_TEXTURE1_ARB);
  glActiveTexture(GL_TEXTURE1_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, t2); 

  // tu0
  glClientActiveTexture(GL_TEXTURE0_ARB);
  glActiveTexture(GL_TEXTURE0_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, t1); 
 
#ifdef I_LOVE_BILL
  GLUECALL(glDrawElements, (GL_QUADS, vc, GL_UNSIGNED_INT, i)); 
#else
  GLUECALL(glDrawRangeElements, (GL_QUADS, 0, vc-1, vc, GL_UNSIGNED_INT, i)); 
#endif 

  glPopClientAttrib();
}



/********************************************************
*************   kakkosversio **************************
***************************************************/




#ifndef INTRO

glue_static Particle *glueLoadparticle(char *filename, char *filename_alpha, int xcnt, int ycnt, int flags) {
  Particle *p=malloc(sizeof(Particle));
  
  p->tex=glueLoadtexture(filename, filename_alpha, flags);
  p->xcnt=xcnt;
  p->ycnt=ycnt;
  
  return p;
}

#endif // INTRO


glue_static Particle *glueMakeparticle(unsigned char *data, int xres, int yres, int xcnt, int ycnt, int flags) {
  Particle *p=malloc(sizeof(Particle));
  
  p->tex=glueMaketexture(data, xres, yres, 0);
  p->xcnt=xcnt;
  p->ycnt=ycnt;
  
  return p;
}


glue_static void glueCalcparticlelods(Particle *p, Vertex *vert, float *lods, int vc, Camera *camera, float ramp) {      
  Vertex *cam=&camera->pos;
  Vertex *tgt=&camera->tgt;
  Vertex lookie=new_v(tgt->x-cam->x, tgt->y-cam->y, tgt->z-cam->z);
  float lookie_len=sqrt(lookie.x*lookie.x+lookie.y*lookie.y+lookie.z*lookie.z);
  int x;
  float raja;
  
  
  if (p->xcnt==1) raja=1.0;
  else raja=(float)(p->xcnt-1)/p->xcnt;
  
  
  lookie_len*=lookie_len;
  lookie.x/=lookie_len;
  lookie.y/=lookie_len;
  lookie.z/=lookie_len;
    
  for (x=0; x<vc; x++) {
    float dist;
    Vertex vpos=new_v(vert[x].x-cam->x, vert[x].y-cam->y, vert[x].z-cam->z);
    dist=dot(&lookie, &vpos);
    lods[x]=fabs(1./dist-1)/(ramp)*dist;  
    if (lods[x]>raja) lods[x]=raja;
    if (lods[x]<0) lods[x]=0;
  } 
}    



glue_static void glueCalcparticlelods_oneway(Particle *p, Vertex *vert, float *lods, int vc, Camera *camera, float ramp) {      
  Vertex *cam=&camera->pos;
  Vertex *tgt=&camera->tgt;
  Vertex lookie=new_v(tgt->x-cam->x, tgt->y-cam->y, tgt->z-cam->z);
  float lookie_len=sqrt(lookie.x*lookie.x+lookie.y*lookie.y+lookie.z*lookie.z);
  int x;
  float raja;
  
  
  if (p->xcnt==1) raja=1.0;
  else raja=(float)(p->xcnt-1)/p->xcnt;
  
  
  lookie_len*=lookie_len;
  lookie.x/=lookie_len;
  lookie.y/=lookie_len;
  lookie.z/=lookie_len;
    
  for (x=0; x<vc; x++) {
    float dist;
    Vertex vpos=new_v(vert[x].x-cam->x, vert[x].y-cam->y, vert[x].z-cam->z);

    dist=dot(&lookie, &vpos);
    if (dist<0) dist=0;
    lods[x]=fabs(1./dist-1)/(ramp)*dist;  
    if (lods[x]>raja) lods[x]=raja;
    if (lods[x]<0) lods[x]=0;
  } 
}    


glue_static void drawcoloredparticles2(Particle *p, Vertex *vert, Vertex *norm, float *rotobuf, float *sizebuf, float *brbuf, 
                           float *depths, int *inds, int indc, float *r, float *u, float *fadebuf, int *pnumbuf, 
                          float size, float br, int pflags, int flags) {
  int x;
  void *tmpptr = tmpmalloc(0);
  static int randtab[1024];
  float *vbuf, *tbuf1, *tbuf2, *cbuf;

  static float sintab[1024], costab[1024];
  static unsigned int *ibuf=0;
  static int maxindex=0;
  int vc2=0;
  int rotate=pflags&PART_ROTATE;
  
  ONCE (
    for (x=0; x<1024; x++) {
      randtab[x]=random();
      sintab[x]=sin(x/1024.0*pi*2.0-pi/4.0);
      costab[x]=cos(x/1024.0*pi*2.0-pi/4.0);
    }
  );
  vbuf=tmpmalloc(sizeof(float)*indc*4*3);
  tbuf1=tmpmalloc(sizeof(float)*indc*4*2);
  tbuf2=tmpmalloc(sizeof(float)*indc*4*2);
  cbuf=tmpmalloc(sizeof(float)*indc*4*4);
  
  
  // verteksit puhveliin
  {
    float *vbufp = vbuf;
    float *cbufp = cbuf;
    float *tbuf1p = tbuf1;
    float *tbuf2p = tbuf2;
    //float *texoffset = tmpmalloc(indc*sizeof *texoffset);
    //float *texkerroin = tmpmalloc(indc*sizeof *texkerroin);
    float *size2 = tmpmalloc(indc*sizeof *size2);
    float *br2 = tmpmalloc(indc*sizeof *br2*4);
    int *roto = tmpmalloc(indc*sizeof *roto);
      
  
    if (sizebuf) for (x=0; x<indc; x++) size2[x]=size*sizebuf[inds[x]]; 
    else for (x=0; x<indc; x++) size2[x]=size;
            
    if (brbuf) for (x=0; x<indc; x++) {
      br2[x*3+0]=brbuf[inds[x]*3+0]*br; 
      br2[x*3+1]=brbuf[inds[x]*3+1]*br; 
      br2[x*3+2]=brbuf[inds[x]*3+2]*br; 
    } else for (x=0; x<indc*3; x++) {
      br2[x]=br;
    }
  
    if (rotate) {
      if (rotobuf) for (x=0; x<indc; x++) roto[x]=rotobuf[inds[x]]*1023;
              else for (x=0; x<indc; x++) roto[x]=randtab[inds[x]&1023];
    } else for (x=0; x<indc; x++) roto[x]=0;

    // verteksit
    for (x=0; x<indc; x++) {
      int i = inds[x];
      Vertex *p = vert+i;
      float s = size2[x]*sintab[roto[x]&1023];
      float c = size2[x]*costab[roto[x]&1023];
      vbufp[ 0] = p->x+r[0]*s+u[0]*c; vbufp[ 1] = p->y+r[1]*s+u[1]*c; vbufp[ 2] = p->z+r[2]*s+u[2]*c;
      vbufp[ 3] = p->x+r[0]*c-u[0]*s; vbufp[ 4] = p->y+r[1]*c-u[1]*s; vbufp[ 5] = p->z+r[2]*c-u[2]*s;
      vbufp[ 6] = p->x-r[0]*s-u[0]*c; vbufp[ 7] = p->y-r[1]*s-u[1]*c; vbufp[ 8] = p->z-r[2]*s-u[2]*c;
      vbufp[ 9] = p->x-r[0]*c+u[0]*s; vbufp[10] = p->y-r[1]*c+u[1]*s; vbufp[11] = p->z-r[2]*c+u[2]*s;
      vbufp += 12;
    }

    // texturekoordinaatit ja värit    
    {
      float xk=1.0/p->xcnt;
      float yk=1.0/p->ycnt;
    
      for (x=0; x<indc; x++) {
        int i=inds[x];
        int fade1, fade2;
        float x1, y1, x2, fade;

        fade1=(int)(fadebuf[i]*p->xcnt)%p->xcnt;
        fade2=(fade1+1)%p->xcnt;
        fade=1.0-fmod(fadebuf[i]*p->xcnt, 1);
      
        cbufp[ 0] = br2[x*3+0]; cbufp[ 1] = br2[x*3+1]; cbufp[ 2] = br2[x*3+2]; cbufp[ 3] = fade;
        cbufp[ 4] = br2[x*3+0]; cbufp[ 5] = br2[x*3+1]; cbufp[ 6] = br2[x*3+2]; cbufp[ 7] = fade;
        cbufp[ 8] = br2[x*3+0]; cbufp[ 9] = br2[x*3+1]; cbufp[10] = br2[x*3+2]; cbufp[11] = fade;
        cbufp[12] = br2[x*3+0]; cbufp[13] = br2[x*3+1]; cbufp[14] = br2[x*3+2]; cbufp[15] = fade;
        cbufp += 16;    
        
        x1=xk*fade1; x2=xk*fade2;
        y1=yk*(pnumbuf[i]%p->ycnt);

        tbuf1p[0] = x1;    tbuf1p[1] = y1;
        tbuf1p[2] = x1+xk; tbuf1p[3] = y1;
        tbuf1p[4] = x1+xk; tbuf1p[5] = y1+yk;
        tbuf1p[6] = x1;    tbuf1p[7] = y1+yk;

        tbuf2p[0] = x2;    tbuf2p[1] = y1;
        tbuf2p[2] = x2+xk; tbuf2p[3] = y1;
        tbuf2p[4] = x2+xk; tbuf2p[5] = y1+yk;
        tbuf2p[6] = x2;    tbuf2p[7] = y1+yk;

        tbuf1p += 8; tbuf2p += 8;
      }
    }
      
    vc2 = indc;
  }

  // rakennetaan uusi indeksibufferi jos faceja enemmän kuin ennen
  if (vc2*4>maxindex) {
    if (ibuf) free(ibuf);
    ibuf=malloc(sizeof(int)*vc2*4);
    for (x=0; x<vc2*4; x++) ibuf[x]=x; 
    maxindex=vc2*4;
  }

  particles_draw_lod(vbuf, tbuf1, tbuf2, cbuf, ibuf, vc2*4);

  glueFC+=vc2*2;
  glueVC+=vc2*4;
  tmpfree(tmpptr);
}



glue_static void glueDrawparticles2(Particle *p, Vertex *vert, Vertex *norm, float *rotobuf, float *sizebuf, 
                        float *colbuf, int vc, Camera *camera, float *fadebuf, int *pnumbuf, 
                        float size, float br, int pflags, int flags) {
  int x;
  void *tmpptr = tmpmalloc(0);
  Vertex *cam=&camera->pos;
  Vertex *tgt=&camera->tgt;
  Vertex lookie=new_v(tgt->x-cam->x, tgt->y-cam->y, tgt->z-cam->z);
  float lookie_len=sqrt(lookie.x*lookie.x+lookie.y*lookie.y+lookie.z*lookie.z);
  Vertex ncam=new_v(cam->x, cam->y, cam->z);
  int cull=pflags&PART_CULL, sort=pflags&PART_SORT, nobehindcheck=pflags&PART_NOBEHIND;
  int inv_sort=pflags&PART_INVSORT;
  float r[3], rl;
  float u[3], ul;
  float m[16];
  Vertex *vlooks=0;
  float invk;

  if (fadebuf==0 || pnumbuf==0) glueError("give me fadebuf and pnumbuf dipshit!");
  
  if (inv_sort) invk=-1.0, nobehindcheck=1, sort=1;
  else invk=1.0;
  
  normalize(&ncam);
  
  lookie_len*=lookie_len;
  lookie.x/=lookie_len;
  lookie.y/=lookie_len;
  lookie.z/=lookie_len;

  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  r[0]=m[0]; r[1]=m[4]; r[2]=m[8]; rl=1./sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]); r[0]*=rl; r[1]*=rl; r[2]*=rl;
  u[0]=m[1]; u[1]=m[5]; u[2]=m[9]; ul=1./sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]); u[0]*=ul; u[1]*=ul; u[2]*=ul;
    
  // calc distances from camera
  if (!cull) {
    for (x=0; x<vc; x++) {
      Vertex vpos=new_v(vert[x].x-cam->x, vert[x].y-cam->y, vert[x].z-cam->z);
      radix_buffer1[x].f=dot(&lookie, &vpos)*invk;
      radix_buffer1[x].i=x;
    } 
  } else {
    vlooks=tmpmalloc(sizeof(Vertex)*vc);
    for (x=0; x<vc; x++) {
      Vertex vpos=new_v(vert[x].x-cam->x, vert[x].y-cam->y, vert[x].z-cam->z);
      
      vlooks[x]=vpos;
      normalize(&vlooks[x]);
      radix_buffer1[x].f=dot(&lookie, &vpos);
      radix_buffer1[x].i=x;
    } 
    
  }
  if (sort) GLUECALL(glueRadixsort, (vc));
  
  // verteksit puhveliin
  {
    int *inds = tmpmalloc(vc*sizeof *inds);
    float *depths = tmpmalloc(vc*sizeof *depths);
    int indc = 0;
    
    if (!cull) {
      if (nobehindcheck) {
        for (x=0; x<vc; x++) inds[indc] = radix_buffer1[(vc-1)-x].i, depths[indc] = radix_buffer1[(vc-1)-x].f, indc++;
      } else {
        for (x=0; x<vc; x++) if (radix_buffer1[(vc-1)-x].f>0) inds[indc] = radix_buffer1[(vc-1)-x].i, depths[indc] = radix_buffer1[(vc-1)-x].f, indc++;
      }
    } else for (x=0; x<vc; x++) {
      int i = radix_buffer1[(vc-1)-x].i;
      Vertex *n = norm + i;
      if (n->x*vlooks[i].x+n->y*vlooks[i].y+n->z*vlooks[i].z<=0.25 && radix_buffer1[(vc-1)-x].f>0) inds[indc] = i, depths[indc] = radix_buffer1[(vc-1)-x].f, indc++;
    }
    
        
    
    // draw particles in several steps
    {
      int blk, blksz=1024;

      gluePush();
      if (flags&GLUE_CHECK_DEPTH) renderflags(flags|GLUE_NO_CULL);
      else renderflags(flags|GLUE_NO_CULL);
      glDisable(GL_LIGHTING);   
  
      glueBindtexture_lod_colors(p->tex, flags);  
      
      for (blk=0; blk<indc; blk+=blksz) 
      drawcoloredparticles2(p, vert, norm, rotobuf, sizebuf, colbuf, depths+blk, inds+blk, indc-blk<blksz?indc-blk:blksz, r, u, fadebuf, pnumbuf, size, br, pflags, flags);
      
      gluePop();
      
    }
  }
  tmpfree(tmpptr);
  if (cull) tmpfree(vlooks);
}






glue_static void drawcoloredparticles3(Texture *tex, Vertex *vert, Vertex *norm, float *rotobuf, float *sizebuf, float *brbuf, 
                           float *depths, int *inds, int indc, float *r, float *u, float size, float br, int pflags, int flags) {
  int x;
  void *tmpptr = tmpmalloc(0);
  static int randtab[1024];
  float *vbuf, *tbuf1, *cbuf;

  static float sintab[1024], costab[1024];
  static unsigned int *ibuf=0;
  static int maxindex=0;
  int vc2=0;
  int rotate=pflags&PART_ROTATE;
  
  ONCE (
    for (x=0; x<1024; x++) {
      randtab[x]=random();
      sintab[x]=sin(x/1024.0*pi*2.0-pi/4.0);
      costab[x]=cos(x/1024.0*pi*2.0-pi/4.0);
    }
  );
  vbuf=tmpmalloc(sizeof(float)*indc*4*3);
  tbuf1=tmpmalloc(sizeof(float)*indc*4*2);
  cbuf=tmpmalloc(sizeof(float)*indc*4*4);
  
  
  // verteksit puhveliin
  {
    float *vbufp = vbuf;
    float *cbufp = cbuf;
    float *tbuf1p = tbuf1;
    float *size2 = tmpmalloc(indc*sizeof *size2);
    float *br2 = tmpmalloc(indc*sizeof *br2*4);
    int *roto = tmpmalloc(indc*sizeof *roto);
      
  
    if (sizebuf) for (x=0; x<indc; x++) size2[x]=size*sizebuf[inds[x]]; 
    else for (x=0; x<indc; x++) size2[x]=size;
            
    if (brbuf) for (x=0; x<indc; x++) {
      br2[x*4+0]=brbuf[inds[x]*4+0]*br; 
      br2[x*4+1]=brbuf[inds[x]*4+1]*br; 
      br2[x*4+2]=brbuf[inds[x]*4+2]*br; 
      br2[x*4+3]=brbuf[inds[x]*4+3]*br; 
    } else for (x=0; x<indc*4; x++) {
      br2[x]=br;
    }
  
    if (rotate) {
      if (rotobuf) for (x=0; x<indc; x++) roto[x]=rotobuf[inds[x]]*1023;
              else for (x=0; x<indc; x++) roto[x]=randtab[inds[x]&1023];
    } else for (x=0; x<indc; x++) roto[x]=0;

    // verteksit
    for (x=0; x<indc; x++) {
      int i = inds[x];
      Vertex *p = vert+i;
      float s = size2[x]*sintab[roto[x]&1023];
      float c = size2[x]*costab[roto[x]&1023];
      vbufp[ 0] = p->x+r[0]*s+u[0]*c; vbufp[ 1] = p->y+r[1]*s+u[1]*c; vbufp[ 2] = p->z+r[2]*s+u[2]*c;
      vbufp[ 3] = p->x+r[0]*c-u[0]*s; vbufp[ 4] = p->y+r[1]*c-u[1]*s; vbufp[ 5] = p->z+r[2]*c-u[2]*s;
      vbufp[ 6] = p->x-r[0]*s-u[0]*c; vbufp[ 7] = p->y-r[1]*s-u[1]*c; vbufp[ 8] = p->z-r[2]*s-u[2]*c;
      vbufp[ 9] = p->x-r[0]*c+u[0]*s; vbufp[10] = p->y-r[1]*c+u[1]*s; vbufp[11] = p->z-r[2]*c+u[2]*s;
      vbufp += 12;
    }

    // texturekoordinaatit ja värit    
    {    
      for (x=0; x<indc; x++) {
     
        cbufp[ 0] = br2[x*4+0]; cbufp[ 1] = br2[x*4+1]; cbufp[ 2] = br2[x*4+2]; cbufp[ 3] = br2[x*4+3];
        cbufp[ 4] = br2[x*4+0]; cbufp[ 5] = br2[x*4+1]; cbufp[ 6] = br2[x*4+2]; cbufp[ 7] = br2[x*4+3];
        cbufp[ 8] = br2[x*4+0]; cbufp[ 9] = br2[x*4+1]; cbufp[10] = br2[x*4+2]; cbufp[11] = br2[x*4+3];
        cbufp[12] = br2[x*4+0]; cbufp[13] = br2[x*4+1]; cbufp[14] = br2[x*4+2]; cbufp[15] = br2[x*4+3];
        cbufp += 16;    
        
        tbuf1p[0] = 0; tbuf1p[1] = 0;
        tbuf1p[2] = 1; tbuf1p[3] = 0;
        tbuf1p[4] = 1; tbuf1p[5] = 1;
        tbuf1p[6] = 0; tbuf1p[7] = 1;
        tbuf1p += 8; 
      }
    }
      
    vc2 = indc;
  }

  // rakennetaan uusi indeksibufferi jos faceja enemmän kuin ennen
  if (vc2*4>maxindex) {
    if (ibuf) free(ibuf);
    ibuf=malloc(sizeof(int)*vc2*4);
    for (x=0; x<vc2*4; x++) ibuf[x]=x; 
    maxindex=vc2*4;
  }
  
  particles_draw(vbuf, tbuf1, cbuf, ibuf, vc2*4);

  glueFC+=vc2*2;
  glueVC+=vc2*4;
  tmpfree(tmpptr);
}




glue_static void glueDrawparticles3(Texture *tex, Vertex *vert, Vertex *norm, float *rotobuf, float *sizebuf, 
                        float *colbuf, int vc, Camera *camera, float size, float br, int pflags, int flags) {
  int x;
  void *tmpptr = tmpmalloc(0);
  Vertex *cam=&camera->pos;
  Vertex *tgt=&camera->tgt;
  Vertex lookie=new_v(tgt->x-cam->x, tgt->y-cam->y, tgt->z-cam->z);
  float lookie_len=sqrt(lookie.x*lookie.x+lookie.y*lookie.y+lookie.z*lookie.z);
  Vertex ncam=new_v(cam->x, cam->y, cam->z);
  int cull=pflags&PART_CULL, sort=pflags&PART_SORT, nobehindcheck=pflags&PART_NOBEHIND;
  int inv_sort=pflags&PART_INVSORT;
  float r[3], rl;
  float u[3], ul;
  float m[16];
  Vertex *vlooks=0;
  float invk;
  
  if (inv_sort) invk=-1.0, nobehindcheck=1, sort=1;
  else invk=1.0;
  
  normalize(&ncam);
  
  lookie_len*=lookie_len;
  lookie.x/=lookie_len;
  lookie.y/=lookie_len;
  lookie.z/=lookie_len;

  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  r[0]=m[0]; r[1]=m[4]; r[2]=m[8]; rl=1./sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]); r[0]*=rl; r[1]*=rl; r[2]*=rl;
  u[0]=m[1]; u[1]=m[5]; u[2]=m[9]; ul=1./sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]); u[0]*=ul; u[1]*=ul; u[2]*=ul;
    
  // calc distances from camera
  if (!cull) {
    for (x=0; x<vc; x++) {
      Vertex vpos=new_v(vert[x].x-cam->x, vert[x].y-cam->y, vert[x].z-cam->z);
      radix_buffer1[x].f=dot(&lookie, &vpos)*invk;
      radix_buffer1[x].i=x;
    } 
  } else {
    vlooks=tmpmalloc(sizeof(Vertex)*vc);
    for (x=0; x<vc; x++) {
      Vertex vpos=new_v(vert[x].x-cam->x, vert[x].y-cam->y, vert[x].z-cam->z);
      
      vlooks[x]=vpos;
      normalize(&vlooks[x]);
      radix_buffer1[x].f=dot(&lookie, &vpos);
      radix_buffer1[x].i=x;
    } 
    
  }
  if (sort) GLUECALL(glueRadixsort, (vc));
  
  // verteksit puhveliin
  {
    int *inds = tmpmalloc(vc*sizeof *inds);
    float *depths = tmpmalloc(vc*sizeof *depths);
    int indc = 0;
    
    if (!cull) {
      if (nobehindcheck) {
        for (x=0; x<vc; x++) inds[indc] = radix_buffer1[(vc-1)-x].i, depths[indc] = radix_buffer1[(vc-1)-x].f, indc++;
      } else {
        for (x=0; x<vc; x++) if (radix_buffer1[(vc-1)-x].f>0) inds[indc] = radix_buffer1[(vc-1)-x].i, depths[indc] = radix_buffer1[(vc-1)-x].f, indc++;
      }
    } else for (x=0; x<vc; x++) {
      int i = radix_buffer1[(vc-1)-x].i;
      Vertex *n = norm + i;
      if (n->x*vlooks[i].x+n->y*vlooks[i].y+n->z*vlooks[i].z<=0.25 && radix_buffer1[(vc-1)-x].f>0) inds[indc] = i, depths[indc] = radix_buffer1[(vc-1)-x].f, indc++;
    }
    
    // draw particles in several steps
    {
      int blk, blksz=1024;
      
      gluePush();
      renderflags(flags|GLUE_NO_CULL);
      glDisable(GL_LIGHTING); 

      glueBindtexture(tex, flags);
      
      for (blk=0; blk<indc; blk+=blksz) 
      drawcoloredparticles3(tex, vert, norm, rotobuf, sizebuf, colbuf, depths+blk, inds+blk, indc-blk<blksz?indc-blk:blksz, r, u, size, br, pflags, flags);
      
      gluePop();
    }
  }
  tmpfree(tmpptr);
  if (cull) tmpfree(vlooks);
}




//////////////////////////////////////////////


glue_static float dot2(Vertex2 *v1, Vertex2 *v2) {
  return v1->x*v2->x+
         v1->y*v2->y+
         v1->z*v2->z;
}

glue_static void normalize2(Vertex2 *v) {
  v->len=sqrt(dot2(v, v));
  v->x/=v->len;
  v->y/=v->len;
  v->z/=v->len;
}

glue_static Vertex2 v_sub2(Vertex *v1, Vertex *v2) {
  Vertex2 v;

  v.x=v1->x-v2->x;
  v.y=v1->y-v2->y;
  v.z=v1->z-v2->z;

  normalize2(&v);

  return v;
}


glue_static void ps_dontrecurse(Vertex2 *rdir, int rcnt, Vertex2 *cdir, int ccnt, float *shd, float koko, float kerroin) {
  int i, j;
  for (j=0; j<ccnt; j++) for (i=0; i<rcnt; i++) {
    Vertex2 *r=rdir+i, *c=cdir+j;
    float d = r->x*c->x + r->y*c->y + r->z*c->z;
    if (d*(c->len*c->len+koko*koko) > c->len*c->len && r->len>c->len+koko) shd[i]*=kerroin;
  }
}
/*
void ps_stupidrecurse(Vertex2 *rdir, int rcnt, Vertex2 *cdir, int ccnt, float *shd, float koko) {
  if (rcnt<10 && ccnt<10) { ps_dontrecurse(rdir, rcnt, cdir, ccnt, shd, koko); return; }
  if (rcnt>ccnt) {
    int spl=rcnt>>1;
    ps_stupidrecurse(rdir    , spl     , cdir, ccnt, shd    , koko);
    ps_stupidrecurse(rdir+spl, rcnt-spl, cdir, ccnt, shd+spl, koko);
  } else {
    int spl=ccnt>>1;
    ps_stupidrecurse(rdir, rcnt, cdir    , spl     , shd, koko);
    ps_stupidrecurse(rdir, rcnt, cdir+spl, ccnt-spl, shd, koko);
  }
}*/


/*static void split()*/

static void ps_xrecurse(Vertex2 *rdir, int rcnt, Vertex2 *cdir, int ccnt, float *shd, float koko, int depth, float kerroin) {
  float px, py, pz, l;
  void *tmpptr=tmpmalloc(0);
  Vertex2 *cdir0, *cdir1, *rdir0, *rdir1;
  float *shd0, *shd1;
  int ccnt0, ccnt1, rcnt0, rcnt1, i;
  if (!depth || rcnt<10) { ps_dontrecurse(rdir, rcnt, cdir, ccnt, shd, koko, kerroin); return; }
  cdir0 = tmpmalloc(ccnt*sizeof *cdir0); ccnt0=0;
  cdir1 = tmpmalloc(ccnt*sizeof *cdir1); ccnt1=0;
  rdir0 = tmpmalloc(rcnt*sizeof *rdir0); rcnt0=0;
  rdir1 = tmpmalloc(rcnt*sizeof *rdir1); rcnt1=0;
  shd0 = tmpmalloc(rcnt*sizeof *shd0);
  shd1 = tmpmalloc(rcnt*sizeof *shd1);
  px=0; py=0; pz=0;
  for (i=0; i<rcnt; i++) {
    Vertex2 *r=rdir+i; px+=r->x; py+=r->y; pz+=r->z;
  }
  if (depth&1) {
    pz = px; px = py; py = -pz; pz = 0;
  } else {
    px = py; py = pz; pz = -px; px = 0;
  }
  l=1./sqrt(px*px+py*py+pz*pz); px*=l; py*=l; pz*=l;
  for (i=0; i<ccnt; i++) {
    Vertex2 *c=cdir+i;
     if ((c->x*px+c->y*py+c->z*pz)*c->len>=-koko) cdir0[ccnt0++] = cdir[i];
     if ((c->x*px+c->y*py+c->z*pz)*c->len<= koko) cdir1[ccnt1++] = cdir[i];
  }
  for (i=0; i<rcnt; i++) {
    Vertex2 *c=rdir+i;
    if (c->x*px+c->y*py+c->z*pz>=0) rdir0[rcnt0] = rdir[i], shd0[rcnt0]=shd[i], rcnt0++;
                               else rdir1[rcnt1] = rdir[i], shd1[rcnt1]=shd[i], rcnt1++;
  }
  ps_xrecurse(rdir0, rcnt0, cdir0, ccnt0, shd0, koko, depth-1, kerroin);
  ps_xrecurse(rdir1, rcnt1, cdir1, ccnt1, shd1, koko, depth-1, kerroin);
  rcnt0=0; rcnt1=0;
  for (i=0; i<rcnt; i++) {
    Vertex2 *c=rdir+i;
    if (c->x*px+c->y*py+c->z*pz>=0) shd[i]=shd0[rcnt0], rcnt0++;
                               else shd[i]=shd1[rcnt1], rcnt1++;
  }
  tmpfree(tmpptr);
}



glue_static float *calc_particleshadows(Vertex *receive, int rcnt, Vertex *cast, int ccnt, Vertex *light, float koko, float br) {
  float *shd;
  void *tmpptr=tmpmalloc(0);
  int i;
  Vertex2 *rdir=tmpmalloc(sizeof(Vertex2)*rcnt);
  Vertex2 *cdir=tmpmalloc(sizeof(Vertex2)*ccnt);
  shd=malloc(sizeof(float)*rcnt*3);

  for (i=0; i<rcnt; i++) {
    shd[i]=1.0;
    rdir[i]=v_sub2(light, receive+i);
  }
  for (i=0; i<ccnt; i++) {
    cdir[i]=v_sub2(light, cast+i);
  }
  
  GLUECALL(ps_xrecurse, (rdir, rcnt, cdir, ccnt, shd, koko, 20, br));

  for (i=rcnt-1; i>=0; i--) {
    shd[i*3+0]=clamp(shd[i]);
    shd[i*3+1]=clamp(shd[i]);
    shd[i*3+2]=clamp(shd[i]);
  }
  tmpfree(tmpptr);
  return shd;
}


glue_static float *calc_particlesizes(Vertex *v, Vertex *n, int *vc, int *ind, int fc, int ybercount, float ybersize, int add_center) {
  int vc2=add_center?vc[0]+fc:vc[0];
//  int fstart=vc[0];
  juttuh *vsizes=tmpmalloc(sizeof(juttuh)*vc2);
  float *sizes=malloc(sizeof(float)*(vc2+ybercount));
  int x;

  for (x=0; x<vc2; x++) {
    vsizes[x].kunt=0;
    vsizes[x].koko=0;
  }

  for (x=0; x<fc; x++) {
    int v1, v2, v3;
    float plen;

    v1=ind[x*3+0]; v2=ind[x*3+1]; v3=ind[x*3+2];
    plen=(v_len(v[v1], v[v2])+v_len(v[v2], v[v3])+v_len(v[v3], v[v1]))/4.0;

    if (add_center) {
      Vertex ppos=new_v((v[v1].x+v[v2].x+v[v3].x)/3, (v[v1].y+v[v2].y+v[v3].y)/3, (v[v1].z+v[v2].z+v[v3].z)/3);
      Vertex pnorm;
      //Vertex pnorm=new_v(n[v1].x+n[v2].x+n[v3].x, n[v1].y+n[v2].y+n[v3].y, n[v1].z+n[v2].z+n[v3].z);
      //normalize(&pnorm);

      calc_fnorm(&v[v1], &v[v2], &v[v3], &pnorm);

      v[vc[0]+x]=new_v(ppos.x-pnorm.x*0.3, ppos.y-pnorm.y*0.3, ppos.z-pnorm.z*0.3);
      //v[vc[0]+x]=ppos;
      n[vc[0]+x]=pnorm;
      vsizes[vc[0]+x].koko=plen;
       vsizes[vc[0]+x].kunt=1;
    }

    vsizes[v1].kunt+=1; vsizes[v1].koko+=plen;
    vsizes[v2].kunt+=1; vsizes[v2].koko+=plen;
    vsizes[v3].kunt+=1; vsizes[v3].koko+=plen;
  }

  if (add_center) vc[0]+=fc;

  for (x=0; x<vc2; x++) sizes[x]=vsizes[x].koko/vsizes[x].kunt;
  for (x=vc2; x<vc2+ybercount; x++) sizes[x]=ybersize;

  tmpfree(vsizes);

  return sizes;
}




