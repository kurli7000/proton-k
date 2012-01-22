#include "glueloadobj.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <io.h>


typedef struct {
  float x, y, z;
  int k;
} vertex;

typedef struct {
  int v[3];
  int n[3];
  int used;
} face;


static char *tempbuf;
int datapos=0;

static vertex *temp_vlist;
static vertex *temp_vlist_x;
static face *temp_flist;
static vertex *temp_tlist;
static vertex *temp_tlist_x;
//static vertex *temp_clist;

static face *orig_flist;
int orig_fpos=0;
int vertices_splitted=0;
static int *muthas; 
int muthapos=0;
static int mapped=0;


static int vertexpos_x=0;
static int texturepos_x=0;
static int vertexpos=0;
static int facepos=0;
static int facepos2=0;
int size=0;

#define MAX_VERTS 500000
#define MAX_FACES 1000000
#define MAX_ATTACH 32





/********************************
********  file reading  *********
********************************/

int getline(char *dest) {
  char temp[10000];
  int x=datapos;

  memset(temp, 0, 10000);

  while(tempbuf[datapos]!=10) {
    temp[datapos-x]=tempbuf[datapos];
    datapos++;
    if (datapos>=size) return 0;
  }
  
  sprintf(dest, "%s", temp);
  datapos++;
  return 1;
}


void substr(char *sourke, char *dest, int len) {
  int x;
  
  for (x=0; x<len; x++) {
    dest[x]=sourke[x];
  }

  dest[len]=0;
}



/**********************************************
********  multiple texcoord-shitfuck  *********
**********************************************/

typedef struct {
  int vi, ti;
  int newind;
} hasa;

static hasa hasataulu[MAX_VERTS*MAX_ATTACH];


int hashcheck(int vi, int ti) {
  int x;
  
  for (x=0; x<MAX_ATTACH; x++) {
    int i=vi*MAX_ATTACH+x;
    
    if (hasataulu[i].vi==vi && hasataulu[i].ti==ti) {
      return hasataulu[i].newind;
    }
    
    if (hasataulu[i].vi==-1) {
      
      hasataulu[i].vi=vi;
      hasataulu[i].ti=ti;
      hasataulu[i].newind=vertexpos;
      
      if (x>0) {
        muthas[muthapos+0]=vertexpos;
        muthas[muthapos+1]=hasataulu[vi*MAX_ATTACH].newind;
        muthapos+=2;
        vertices_splitted++;
      }

      vertexpos++;

      return hasataulu[i].newind;
    }
  }
  
  return -666;
}


int nu_vertex(int vi, int ti) {
  int plaa;
  
  plaa=hashcheck(vi, ti);
  if (plaa==-666) glueErrorf("\n\nERREUR: out of hash!\n\n"); 
      
  temp_vlist[plaa]=temp_vlist_x[vi];
  temp_tlist[plaa]=temp_tlist_x[ti];  
  
  if (vertexpos>MAX_VERTS-1) glueErrorf("\n\nERREUR: out of vertexbuffer\n\n");    
  
  return plaa;
}



Object *glueLoadOBJ(char *filename, int flags) {
  FILE *in;
  char rivi[10000];
  char subrivi[10000];
  int maxface=0;
  int x;

  Vertex *vertices=0;
  Vertex *texcoords=0;
  int *indices=0;
  int *orig_indices=0;
  int *vertexlinks=0;
  Object *obj;
  
     
  
  in=fopen(filename, "rb");
  if (!in) glueErrorf("error opening file: %s", filename); 
  size=filelength(fileno(in));
  tempbuf=malloc(size);
  fread(tempbuf, size, 1, in);
  fclose(in);

  temp_vlist_x=malloc(sizeof(vertex)*MAX_VERTS);
  temp_vlist=malloc(sizeof(vertex)*MAX_VERTS);  
  temp_flist=malloc(sizeof(face)*MAX_FACES);
  temp_tlist=malloc(sizeof(vertex)*MAX_VERTS);
  temp_tlist_x=malloc(sizeof(vertex)*MAX_VERTS);
  orig_flist=malloc(sizeof(face)*MAX_FACES);
  muthas=malloc(sizeof(int)*MAX_FACES*2);
  
  for (x=0; x<MAX_VERTS*MAX_ATTACH; x++) hasataulu[x].vi=hasataulu[x].ti=-1;


  while (1) {
    
    if (!getline(rivi)) break;
    substr(rivi, subrivi, 2);
    

    if (strcmp(subrivi, "v ")==0) {     // vertex
      float xx=0, yy=0, zz=0;

      sscanf(rivi, "v %f %f %f", &xx, &yy, &zz);

      temp_vlist_x[vertexpos_x].x=xx;
      temp_vlist_x[vertexpos_x].y=yy;
      temp_vlist_x[vertexpos_x].z=zz;
      vertexpos_x++;
      
    } else if (strcmp(subrivi, "vt")==0) {     // texcoord
      float uu=0, vv=0;

      sscanf(rivi, "vt %f %f", &uu, &vv);
      
      temp_tlist_x[texturepos_x].x=uu;
      temp_tlist_x[texturepos_x].y=vv;
      temp_tlist_x[texturepos_x].z=0;      
      texturepos_x++;
      mapped=1;
      
    } else if (strcmp(subrivi, "f ")==0) {      // face
      static int v1, v2, v3;
      static int t1, t2, t3;
      static int vv1, vv2, vv3;

      if (mapped) sscanf(rivi, "f %i/%i %i/%i %i/%i", &v1, &t1, &v2, &t2, &v3, &t3);
      else {
        sscanf(rivi, "f %i %i %i", &v1, &v2, &v3);
        t1=t2=t3=1;
      }
      
      if (v1>maxface) maxface=v1;
      if (v2>maxface) maxface=v2;
      if (v3>maxface) maxface=v3;

      vv1=nu_vertex(v1-1, t1-1);
      vv2=nu_vertex(v2-1, t2-1);
      vv3=nu_vertex(v3-1, t3-1);
      
      orig_flist[orig_fpos].v[0]=hasataulu[(v1-1)*MAX_ATTACH].newind;
      orig_flist[orig_fpos].v[1]=hasataulu[(v2-1)*MAX_ATTACH].newind;
      orig_flist[orig_fpos].v[2]=hasataulu[(v3-1)*MAX_ATTACH].newind;
      orig_fpos++;

      temp_flist[facepos].v[0]=vv1;
      temp_flist[facepos].v[1]=vv2;
      temp_flist[facepos].v[2]=vv3;
      temp_flist[facepos].used=0;
      
      facepos++;      
    } 
  }
  
  //glueNoticef("scan ok, vpos:%i  fpos:%i   tpos:%i ", vertexpos_x, facepos, texturepos_x);
  //if (mapped) glueNoticef("mapped");
  //if (vertices_splitted) glueNoticef("vertices_splitted");

  vertices=malloc(sizeof(Vertex)*vertexpos);
  for (x=0; x<vertexpos; x++) {
    vertices[x].x=temp_vlist[x].x;
    vertices[x].y=temp_vlist[x].y;
    vertices[x].z=temp_vlist[x].z;
  }
   
  if (mapped) {
    texcoords=malloc(sizeof(Vertex)*vertexpos);
    for (x=0; x<vertexpos; x++) {
      texcoords[x].x=temp_tlist[x].x;
      texcoords[x].y=temp_tlist[x].y;
      texcoords[x].z=temp_tlist[x].z;
    }
  }
  
  indices=malloc(sizeof(int)*facepos*3);
  for (x=0; x<facepos; x++) {
    indices[x*3+0]=temp_flist[x].v[0];
    indices[x*3+1]=temp_flist[x].v[1];
    indices[x*3+2]=temp_flist[x].v[2];
  }
  
  
  if (vertices_splitted) {
  
    orig_indices=malloc(sizeof(int)*orig_fpos*3);
    for (x=0; x<orig_fpos; x++) {
      orig_indices[x*3+0]=orig_flist[x].v[0];
      orig_indices[x*3+1]=orig_flist[x].v[1];
      orig_indices[x*3+2]=orig_flist[x].v[2];
    }
    
    vertexlinks=malloc(sizeof(int)*muthapos);
    for (x=0; x<muthapos; x++) vertexlinks[x]=muthas[x];
    
  }
     
  obj = glueLoadobject(
    indices, //int *indices, 
    vertices, //Vertex *vertices, 
    mapped?texcoords:0, //Vertex *texcoords, 
    0, //int *neighbours, 
    0, //Vertex *colors, 
    0, //Edge *edges, 
    vertices_splitted?orig_indices:0, //int *orig_indices, 
    vertices_splitted?vertexlinks:0, //int *vertex_links, 
    vertexpos, //int vc, 
    facepos, //int fc, 
    0, //int ec, 
    vertices_splitted?muthapos/2:0, //int linkcnt, 
    vertices_splitted?vertexpos_x:0, //int orig_vc, 
    flags); //int flgs) {


  free(tempbuf);
  free(temp_vlist_x);
  free(temp_vlist);  
  free(temp_flist);
  free(temp_tlist);
  free(temp_tlist_x);
  free(orig_flist);
  free(muthas);
  
  return obj;
    
}




