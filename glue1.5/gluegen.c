
#ifndef FORGET_INCLUDES
#include "gluegen.h"
#include "gluewin.h"
#endif

/**********************************************************************
  Generic (non opengl/etc-related) stuff
**********************************************************************/

int glue_seed;


glue_static float floorn(float a, float f) {
  return a-fmod(a, f);
}

//float clamp(float a) { return a<0?0:a>1?1:a; }

glue_static Vertex rot_x(Vertex s, float a) { Vertex d; d.x=s.x; rot2d(d.y, d.z, s.y, s.z, sin(a), cos(a)); return d; }
glue_static Vertex rot_y(Vertex s, float a) { Vertex d; d.y=s.y; rot2d(d.z, d.x, s.z, s.x, sin(a), cos(a)); return d; }
glue_static Vertex rot_z(Vertex s, float a) { Vertex d; d.z=s.z; rot2d(d.x, d.y, s.x, s.y, sin(a), cos(a)); return d; }

//int r0255(int a) { return a<0?0:a>255?255:a; }

glue_static int random() { return RANDOM(); }
glue_static float ranf() { return RANF(); } 
glue_static int ranb()   { return RANB(); }

                        
glue_static float smth(float a) {
  if (a<0) return 0;
  if (a>1) return 1;
  return 1-(cos(a*pi)*0.5+0.5);
}

glue_static float fastsmth(float a) {
  if (a<0) return 0;
  if (a>1) return 1;
  return 1-(fastcos(a*pi)*0.5+0.5);
}

glue_static float smth2(float a, float power) {
  float b;

  if (a>=0.5) b=pow((a-0.5)*2, power)*0.5+0.5;
  else b=pow(a*2, 1.0/power)*0.5;

  return b;
}

glue_static float ffmod(float a, float b) {
  float c=a;
  
  if (b<=0) glueError("you so saki saki");
  while (c<0) c+=b;
  return fmod(c, b);
}

glue_static float bojojoing(float a, float k) {
  float aa=clamp(a);
  float ker=pow(1.0-aa, 2.0);

  return smth(pow(aa, 0.5))+(sin(aa*k-pi*0.5)+1)*ker*0.25;

}

glue_static float smthstart(float a) {

  if (a<0) return 0;
  else if (a<0.5) return a*a; 
  else return (a-0.5)+0.25;
}



glue_static float saw(float x) {
  float joo;
  joo=fmod(fabs(x), 2);
  if (joo>1) joo=1-(joo-1);
  return (joo-0.5)*2.0;
}

glue_static float saw2(float x) {
  float joo;
  joo=fmod(x, 1);
  if (joo>0.5) joo=0.5-(joo-0.5);
  return joo*2;
}

glue_static float buzz(float a) {
  a=fmod(fmod(a, 16)+16, 16);
  switch ((int)a) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: default: return 0;
    case 7: return a-7;
    case 8: case 9: case 10: case 11: case 12: case 13: case 14: return 1;
    case 15: return 16-a;
  }
}

glue_static float gpow(float a, float b) {
  return a<0?-pow(-a, b):fabs(a)<=0?0:pow(a, b);
}


glue_static float fastsinx(float a) {
  static float sintab[2048];
  int x;
  int a1=a*2048;
  float f=a*2048-a1;

  ONCE (
    for (x=0; x<2048; x++) sintab[x]=sin(x/2048.0*pi*2.0);
  );

  return sintab[a1&2047]*(1.0-f)+sintab[a1+1&2047]*f;
}



#ifdef INTRO
static Vertex new_v(float x, float y, float z) {
  Vertex v;
  v.x=x; v.y=y; v.z=z;
  return v;
}
#endif



glue_static int texreso(int a) {
  int x;
  int joo=0;
  
  for (x=0; x<16; x++) {
    int p1=pow(2, x), p2=pow(2, x+1);

    if (a>p1 && a<=p2) joo=p2;
  }

  return joo;
}

Vertex v_fade(Vertex *v1, Vertex *v2, float fade) {
  Vertex v;
  
  v.x=v1->x*(1-fade)+v2->x*fade;
  v.y=v1->y*(1-fade)+v2->y*fade;
  v.z=v1->z*(1-fade)+v2->z*fade;
  
  return v;
}


Vertex v_avg(Vertex *v1, Vertex *v2) {
  Vertex plaa;

  plaa.x=(v1->x+v2->x)*0.5;
  plaa.y=(v1->y+v2->y)*0.5;
  plaa.z=(v1->z+v2->z)*0.5;

  return plaa;
}


Vertex v_scale(Vertex *v1, float scale) {
  Vertex plaa;

  plaa.x=v1->x*scale;
  plaa.y=v1->y*scale;
  plaa.z=v1->z*scale;

  return plaa;
}

Vertex v_add(Vertex *v1, Vertex *v2) {
  Vertex plaa;

  plaa.x=v1->x+v2->x;
  plaa.y=v1->y+v2->y;
  plaa.z=v1->z+v2->z;

  return plaa;
}

Vertex v_sub(Vertex *v1, Vertex *v2) {
  Vertex plaa;

  plaa.x=v1->x-v2->x;
  plaa.y=v1->y-v2->y;
  plaa.z=v1->z-v2->z;

  return plaa;
}

Vertex v_mult(Vertex *v1, Vertex *v2) {
  Vertex plaa;

  plaa.x=v1->x*v2->x;
  plaa.y=v1->y*v2->y;
  plaa.z=v1->z*v2->z;

  return plaa;
}





/**********************************************************************
  Math stuff
**********************************************************************/

/*
Vertex cross(Vertex *a, Vertex *b) {
  Vertex v;
  v.x=a->y*b->z-a->z*b->y;
  v.y=a->z*b->x-a->x*b->z;
  v.z=a->x*b->y-a->y*b->x;
  return v;
}

float dot(Vertex *a, Vertex *b) {
  return a->x*b->x+a->y*b->y+a->z*b->z;
}

Vertex vp_cross(Vertex *a, Vertex *b) {
  Vertex v;
  v.x=a->y*b->z-a->z*b->y;
  v.y=a->z*b->x-a->x*b->z;
  v.z=a->x*b->y-a->y*b->x;
  return v;
}

float vp_dot(Vertex *a, Vertex *b) {
  return a->x*b->x+a->y*b->y+a->z*b->z;
}


float v_dot(Vertex a, Vertex b) {
  return a.x*b.x+a.y*b.y+a.z*b.z;
}

Vertex v_cross(Vertex a, Vertex b) {
  Vertex v;
  v.x=a.y*b.z-a.z*b.y;
  v.y=a.z*b.x-a.x*b.z;
  v.z=a.x*b.y-a.y*b.x;
  return v;
}


void normalize(Vertex *d) {
  float r=1.0/sqrt(dot(d, d));
  d->x*=r; d->y*=r; d->z*=r;
}
*/

glue_static void calc_fnorm(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *n) {
  n->x=(v1->y-v3->y)*(v2->z-v3->z)-(v1->z-v3->z)*(v2->y-v3->y);
  n->y=(v1->z-v3->z)*(v2->x-v3->x)-(v1->x-v3->x)*(v2->z-v3->z);
  n->z=(v1->x-v3->x)*(v2->y-v3->y)-(v1->y-v3->y)*(v2->x-v3->x);
  normalize(n);
}

int lmt(int i, int m) { return i<0?0:i>m?m:i; }

glue_static Vertex glueSpline(float t, Splinept *pts, int ptc) {
  int i;
  Splinept *p0, *p1, *p2, *p3;
  float k0, k1, k2, k3;
  Vertex v;//=malloc(sizeof(Vertex));

  t *= ptc;
/*  ptc--;
//  t=fmod(t, 1)*(float)ptc;

  for (i=0; i<ptc && pts[i].t<t; i++) ;*/
  i = floor(t);
  t -= i;
  i %= ptc;

  p0=pts+lmt(i-2, ptc);
  p1=pts+lmt(i-1, ptc);
  p2=pts+lmt(i  , ptc);
  p3=pts+lmt(i+1, ptc);

/*  t=p2==p1?0:(t-p1->t)/(p2->t-p1->t);*/

/*  if (t>1) t=1; if (t<0) t=0;*/
  k2=t*t*(3-2*t); k1=1-k2; k0=0; k3=0;
  k3=-t*t*(1-t)*.5*(1+(p3==p2)); k1-=k3;
  k0=-t*(1-t)*(1-t)*.5*(1+(p0==p1)); k2-=k0;

  v.x=p0->p.x*k0+p1->p.x*k1+p2->p.x*k2+p3->p.x*k3;
  v.y=p0->p.y*k0+p1->p.y*k1+p2->p.y*k2+p3->p.y*k3;
  v.z=p0->p.z*k0+p1->p.z*k1+p2->p.z*k2+p3->p.z*k3;

  return v;
}

glue_static int lmt2(int i, int m) {
  return (i+m)%m;
}

glue_static Vertex glueSpline_loop(float t, Splinept *pts, int ptc) {
  int i;
  Splinept *p0, *p1, *p2, *p3;
  float k0, k1, k2, k3;
  Vertex v;//=malloc(sizeof(Vertex));

  t *= ptc;
/*  ptc--;
//  t=fmod(t, 1)*(float)ptc;

  for (i=0; i<ptc && pts[i].t<t; i++) ;*/
  i = floor(t);
  t -= i;
  i %= ptc;

  p0=pts+lmt2(i-2, ptc);
  p1=pts+lmt2(i-1, ptc);
  p2=pts+lmt2(i  , ptc);
  p3=pts+lmt2(i+1, ptc);

/*  t=p2==p1?0:(t-p1->t)/(p2->t-p1->t);*/

/*  if (t>1) t=1; if (t<0) t=0;*/
  k2=t*t*(3-2*t); k1=1-k2; k0=0; k3=0;
  k3=-t*t*(1-t)*.5*(1+(p3==p2)); k1-=k3;
  k0=-t*(1-t)*(1-t)*.5*(1+(p0==p1)); k2-=k0;

  v.x=p0->p.x*k0+p1->p.x*k1+p2->p.x*k2+p3->p.x*k3;
  v.y=p0->p.y*k0+p1->p.y*k1+p2->p.y*k2+p3->p.y*k3;
  v.z=p0->p.z*k0+p1->p.z*k1+p2->p.z*k2+p3->p.z*k3;

  return v;
}

/********************* matrix stuff **************************/


glue_static Vertex gluePoint2d(Vertex *in) {
  float foo[16], foo2[16], foo3[16];
  Vertex out;
  int i, j;

  glGetFloatv(GL_PROJECTION_MATRIX, foo);
  glGetFloatv(GL_MODELVIEW_MATRIX, foo2);

  for (i=0; i<4; i++) for (j=0; j<4; j++)
    foo3[i*4+j]=foo[0*4+j]*foo2[i*4+0]+
                foo[1*4+j]*foo2[i*4+1]+
                foo[2*4+j]*foo2[i*4+2]+
                foo[3*4+j]*foo2[i*4+3];

  out.x=in->x*foo3[0]+in->y*foo3[4]+in->z*foo3[8]+foo3[12];
  out.y=in->x*foo3[1]+in->y*foo3[5]+in->z*foo3[9]+foo3[13];
  out.z=in->x*foo3[3]+in->y*foo3[7]+in->z*foo3[11]+foo3[15];
  out.x/=out.z;
  out.y/=out.z;

  return out;
}


glue_static Vertex glueMultmatrix(Vertex *in) {
  float m[16];
  Vertex out;
  float v1[4], v2[4];

  v1[0]=in->x;
  v1[1]=in->y;
  v1[2]=in->z;
  v1[3]=0.0;

  glGetFloatv(GL_MODELVIEW_MATRIX, m);

  v2[0]=m[0]*v1[0]+m[4]*v1[1]+m[ 8]*v1[2]+m[12]*v1[3];
  v2[1]=m[1]*v1[0]+m[5]*v1[1]+m[ 9]*v1[2]+m[13]*v1[3];
  v2[2]=m[2]*v1[0]+m[6]*v1[1]+m[10]*v1[2]+m[14]*v1[3];
  v2[3]=m[3]*v1[0]+m[7]*v1[1]+m[11]*v1[2]+m[15]*v1[3];

  out.x=v2[0];
  out.y=v2[1];
  out.z=v2[2];

  return out;
}



glue_static void glueTargetmatrix(Vertex *dir) {
  float new_m[16];
  Vertex yyy, kaa, koo, yvek;

  yvek.x=0; yvek.y=1; yvek.z=0;// yvek.k=0;
  koo.x=dir->x; koo.y=dir->y; koo.z=dir->z;// koo.k=0;
  normalize(&koo);
  yyy=cross(&koo, &yvek);
  normalize(&yyy);
  kaa=cross(&yyy, &koo);
  new_m[0]=yyy.x; new_m[1]=yyy.y; new_m[2]=yyy.z;  new_m[3]=0.0;  // eka=risti(kolmas, yvec)
  new_m[4]=kaa.x; new_m[5]=kaa.y; new_m[6]=kaa.z;  new_m[7]=0.0;  // toka=risti(eka, kolmas)
  new_m[8]=koo.x; new_m[9]=koo.y; new_m[10]=koo.z; new_m[11]=0.0; // kolmas=suuntavektori normalisoituna
  new_m[12]=0;    new_m[13]=0;    new_m[14]=0;     new_m[15]=1.0;   // neljäs=siirtovektori

  glMultMatrixf(new_m);
}



glue_static void glueMultmatrix_arr(Vertex *in, Vertex *out, int cnt) {
  float m[16];
  int x;
  
  glGetFloatv(GL_MODELVIEW_MATRIX, m);

  for (x=0; x<cnt; x++) {
    out[x].x = m[0]*in[x].x + m[4]*in[x].y + m[ 8]*in[x].z + m[12];
    out[x].y = m[1]*in[x].x + m[5]*in[x].y + m[ 9]*in[x].z + m[13];
    out[x].z = m[2]*in[x].x + m[6]*in[x].y + m[10]*in[x].z + m[14];
  }
}

glue_static void glueMultmatrix_arr2(Vertex *in, Vertex *out, int cnt, float *m) {
  int x;
  
  for (x=0; x<cnt; x++) {
    out[x].x = m[0]*in[x].x + m[4]*in[x].y + m[ 8]*in[x].z + m[12];
    out[x].y = m[1]*in[x].x + m[5]*in[x].y + m[ 9]*in[x].z + m[13];
    out[x].z = m[2]*in[x].x + m[6]*in[x].y + m[10]*in[x].z + m[14];
  }
}


glue_static void invertmatrix(float *m) {
  float tmp[16], t, k;
  int level, i, pivot, r;
  
  for (i = 0; i<16; i++) tmp[i] = m[i], m[i] = i%5==0;
  for (level = 0; level<4; level++) {
    pivot = level;
    for (r = level+1; r<4; r++) if
               (fabs(tmp[r*4+level])>fabs(tmp[pivot*4+level])) pivot = r;
    k = 1./tmp[pivot*4+level];
    for (i = 0; i<4; i++) {
      t = tmp[pivot*4+i]*k; tmp[pivot*4+i] = tmp[level*4+i]; tmp[level*4+i] = t;
      t = m[pivot*4+i]*k; m[pivot*4+i] = m[level*4+i]; m[level*4+i] = t;
    }
    for (r = 0; r<4; r++) if (r!=level) {
      k = -tmp[r*4+level];
      for (i = 0; i<4; i++) {
        tmp[r*4+i] += tmp[level*4+i]*k;
        m[r*4+i] += m[level*4+i]*k;
      }
    }
  }
}


