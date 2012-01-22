
#ifndef GLUEGEN_H
#define GLUEGEN_H

#ifndef FORGET_INCLUDES
#include "glue.h"
#endif

#define pow2(a) (a<0?-a*a:a*a)
#define ONCE(x) do { static int da_onceflag=0; if (da_onceflag==0) { x; da_onceflag=1; } } while (0)

#define rand DONT USE RAND YOU FUCKING MORON!
#define rot2d(dx, dy, sx, sy, s, c) ( (dx)=(sx)*(c)-(sy)*(s), (dy)=(sy)*(c)+(sx)*(s) )

#define r0(x) (x<0?0:x)
#define JMAGIC 69069
#define KMAGIC 0x1
#define RANDOM() ((unsigned)(glue_seed = (glue_seed*JMAGIC+KMAGIC) & 0xffffffff)>>17)
#define RANF()   ((unsigned)(glue_seed = (glue_seed*JMAGIC+KMAGIC) & 0xffffffff)*(1./4292967296.))
#define RANB()   ((unsigned)(glue_seed = (glue_seed*JMAGIC+KMAGIC) & 0xffffffff)>>24)

#define vp_len(a, b) sqrt((a->x-b->x)*(a->x-b->x)+(a->y-b->y)*(a->y-b->y)+(a->z-b->z)*(a->z-b->z))
#define v_len(a, b) sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z))
#define v_len2(a) sqrt((a.x)*(a.x)+(a.y)*(a.y)+(a.z)*(a.z))

#ifndef INTRO
#define new_v(x, y, z) (Vertex){(x), (y), (z)}
#else
static Vertex new_v(float x, float y, float z);
#endif

#define clamp(x) ((x)<0?0:(x)>1?1:(x))
#define r0255(x) ((x)<0?0:(x)>255?255:(x))
#define clamp2(x, a, b) ((x)<(a)?(a):(x)>(b)?(b):(x))

#define cross(a, b) (new_v((a)->y*(b)->z-(a)->z*(b)->y, (a)->z*(b)->x-(a)->x*(b)->z, (a)->x*(b)->y-(a)->y*(b)->x))
#define dot(a, b) ((a)->x*(b)->x+(a)->y*(b)->y+(a)->z*(b)->z)
#define normalize(d) { float r=1.0/sqrt(dot((d), (d))); (d)->x*=r; (d)->y*=r; (d)->z*=r; }

#define square(x) (fmod(x, 1)<0.5?1:-1)

// VARIABLES
glue_extern int glue_seed;


// PROTOTYPES

glue_static float floorn(float a, float f);
//float clamp(float a);
glue_static Vertex rot_x(Vertex s, float a);
glue_static Vertex rot_y(Vertex s, float a);
glue_static Vertex rot_z(Vertex s, float a);
//int r0255(int a);
glue_static int random();
glue_static float ranf();
glue_static int ranb();

glue_static float smth(float a);
glue_static float fastsmth(float a);
glue_static float smth2(float a, float power);
glue_static float bojojoing(float a, float k);
glue_static float ffmod(float a, float b);
glue_static float smthstart(float a);
glue_static float saw(float x);
glue_static float saw2(float x);
glue_static float buzz(float a);
glue_static float gpow(float a, float b);
glue_static float fastsinx(float a);
#define fastsin(a) fastsinx((a)*0.15915494309189533619760628554095)
#define fastcos(a) fastsinx((a)*0.15915494309189533619760628554095+0.25)
glue_static int texreso(int a);

Vertex v_fade(Vertex *v1, Vertex *v2, float fade);
Vertex v_avg(Vertex *v1, Vertex *v2);
Vertex v_scale(Vertex *v1, float scale);
Vertex v_add(Vertex *v1, Vertex *v2);
Vertex v_sub(Vertex *v1, Vertex *v2);
Vertex v_mult(Vertex *v1, Vertex *v2);




/*
Vertex cross(Vertex *a, Vertex *b);
float dot(Vertex *a, Vertex *b); 
Vertex vp_cross(Vertex *a, Vertex *b);
float vp_dot(Vertex *a, Vertex *b);
float v_dot(Vertex a, Vertex b);
Vertex v_cross(Vertex a, Vertex b);
void normalize(Vertex *d);
*/
glue_static void calc_fnorm(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *n);
//int lmt(int i, int m) { return i<0?0:i>m?m:i; }
glue_static int lmt(int i, int m);
glue_static Vertex glueSpline(float t, Splinept *pts, int ptc);
glue_static Vertex glueSpline_loop(float t, Splinept *pts, int ptc);
glue_static Vertex gluePoint2d(Vertex *in);
glue_static Vertex glueMultmatrix(Vertex *in);
glue_static void glueTargetmatrix(Vertex *dir);
glue_static void glueMultmatrix_arr(Vertex *in, Vertex *out, int cnt);
glue_static void glueMultmatrix_arr2(Vertex *in, Vertex *out, int cnt, float *m);
glue_static void invertmatrix(float *m);

#endif // GLUEGEN_H
