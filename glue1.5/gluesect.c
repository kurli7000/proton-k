#include "gluesect.h"

void normaliseeraa(float *d) {
  float r=1.0/sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
  d[0]*=r; d[1]*=r; d[2]*=r;
}

void calcnormalzednormal(float *n, float *v1, float *v2, float *v3) {
  n[0]=(v1[1]-v3[1])*(v2[2]-v3[2])-(v1[2]-v3[2])*(v2[1]-v3[1]);
  n[1]=(v1[2]-v3[2])*(v2[0]-v3[0])-(v1[0]-v3[0])*(v2[2]-v3[2]);
  n[2]=(v1[0]-v3[0])*(v2[1]-v3[1])-(v1[1]-v3[1])*(v2[0]-v3[0]);
  normaliseeraa(n);
}


static float uglydet3(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2) { 
  /*return x0*y1*z2 + y0*z1*x2 + z0*x1*y2 - x0*z1*y2 - y0*x1*z2 - z0*y1*x2; */ 
  return x0*(y1*z2-z1*y2) + y0*(z1*x2-x1*z2) + z0*(x1*y2-y1*x2); 
} 

static float uglyvol(float *v0, float *v1, float *v2, float *v3) { 
  return uglydet3(v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2], 
                  v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2], 
                  v3[0]-v0[0], v3[1]-v0[1], v3[2]-v0[2]); 
} 



int inhtersekt(float *e0, float *e1, float *f0, float *f1, float *f2,
              float *lpiste, float *dir) {
  float top = uglyvol(e0, f0, f1, f2); 
  float bot = uglyvol(e1, f2, f1, f0); /* nää saa nopsemminkin normaalin kautta..*/ 
  float rot0 = uglyvol(e0, e1, f0, f1); 
  float rot1 = uglyvol(e0, e1, f1, f2); 
  float rot2 = uglyvol(e0, e1, f2, f0); 
  int leikkaa = top>0 && bot>0 && rot0>0 && rot1>0 && rot2>0; 
  //int rval=0;

  if (leikkaa) { 
    float k = top*1./(top+bot); 
    float N[3];

    dir[0]=e1[0]-e0[0]; 
    dir[1]=e1[1]-e0[1]; 
    dir[2]=e1[2]-e0[2]; 

    lpiste[0] = e0[0] + dir[0]*k; 
    lpiste[1] = e0[1] + dir[1]*k; 
    lpiste[2] = e0[2] + dir[2]*k; 

    calcnormalzednormal(N, f0, f1, f2); 
    k=2*(N[0]*dir[0]+N[1]*dir[1]+N[2]*dir[2]); 

    dir[0]-=N[0]*k; 
    dir[1]-=N[1]*k; 
    dir[2]-=N[2]*k; 

    normaliseeraa(dir);
    //rval=1;
  }


  return leikkaa;
}


int glueIntersektion(Vertex *ev1, Vertex *ev2, Vertex *fv1, Vertex *fv2, Vertex *fv3, 
                     Vertex *intersekt, Vertex *reflection) {
  float e0[3], e1[3];
  float f0[3], f1[3], f2[3];
  float i[3], r[3];
  int rval=0;
  
  e0[0]=ev1->x; e0[1]=ev1->y; e0[2]=ev1->z;
  e1[0]=ev2->x; e1[1]=ev2->y; e1[2]=ev2->z;
  f0[0]=fv1->x; f0[1]=fv1->y; f0[2]=fv1->z;
  f1[0]=fv2->x; f1[1]=fv2->y; f1[2]=fv2->z;
  f2[0]=fv3->x; f2[1]=fv3->y; f2[2]=fv3->z;

  rval=inhtersekt(e0, e1, f0, f1, f2, i, r);
  
  intersekt->x=i[0]; intersekt->y=i[1]; intersekt->z=i[2];
  reflection->x=r[0]; reflection->y=r[1]; reflection->z=r[2];
 
  return rval;
}




  


int findface(Vertex *e1, Vertex *e2, Object *obj, Vertex *isect, Vertex *refl) {
  int face=-1;
  float dist=100000.0;
  int x;

  for (x=0; x<obj->fc; x++) {
    int v1, v2, v3;
    Vertex i, r;

    v1=obj->faces[x].v[0];
    v2=obj->faces[x].v[1];
    v3=obj->faces[x].v[2];

    if (glueIntersektion(e1, e2, &obj->vertices[v3], &obj->vertices[v2], &obj->vertices[v1], &i, &r)) {
      float newdist=sqrt((e1->x-i.x)*(e1->x-i.x) + (e1->y-i.y)*(e1->y-i.y) + (e1->z-i.z)*(e1->z-i.z));
      if (newdist<dist) {
        face=x;
        dist=newdist;
        isect[0]=i;
        refl[0]=r;
      }
    }
  }  

  return face;
}


/*
void glueLaserbeam(Vertex *v, Vertex *dir, Object *obj, float length, int iterations, float color[4], int flags) {
  int x, f;
  Vertex lsrpos, lsrdir;

  lsrpos.x=v->x; lsrpos.y=v->y; lsrpos.z=v->z;
  lsrdir.x=dir->x; lsrdir.y=dir->y; lsrdir.z=dir->z;

  renderflags(flags);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glLineWidth(2.0);

  for (x=0; x<iterations; x++) {
    int v1, v2, v3;
    Vertex e1, e2;
    int f;
    int joo;

    e1.x=lsrpos.x; e1.y=lsrpos.y; e1.z=lsrpos.z;
    e2.x=lsrpos.x+lsrdir.x*length; e2.y=lsrpos.y+lsrdir.y*length; e2.z=lsrpos.z+lsrdir.z*length;

    f=findface(&e1, &e2, obj);

    if (f!=-1) {
      int joo;
      float br1, br2;

      v1=obj->faces[f].v[0];
      v2=obj->faces[f].v[1];
      v3=obj->faces[f].v[2];

      joo=glueIntersektion(&e1, &e2, &obj->vertices[v3], &obj->vertices[v2], &obj->vertices[v1], &lsrpos, &lsrdir);

      br1=1-(float)x/(float)iterations;
      br2=1-(float)(x+1)/(float)iterations;

      glBegin(GL_LINES);
      glColor4f(color[0]*br1, color[1]*br1, color[2]*br1, color[3]*br1); 
     // glColor3f(1, 0, 0);
      glVertex3f(e1.x, e1.y, e1.z);
      glColor4f(color[0]*br2, color[1]*br2, color[2]*br2, color[3]*br2); 
      glVertex3f(lsrpos.x, lsrpos.y, lsrpos.z);
      glEnd();
    } else { 
      float br1, br2;

      br1=1-(float)x/(float)iterations;
      br2=0;

      glBegin(GL_LINES);
      glColor4f(color[0]*br1, color[1]*br1, color[2]*br1, color[3]*br1); 
//      glColor3f(0, 0, 1);
      glVertex3f(e1.x, e1.y, e1.z);
      glColor4f(color[0]*br2, color[1]*br2, color[2]*br2, color[3]*br2); 
      glVertex3f(e2.x, e2.y, e2.z);
      glEnd();

      return;
    } 
  }
}
*/
