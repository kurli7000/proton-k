#include "gluemesh.h"
#include "..\glue1.5\glueobj.h" 

static char *tempbuf;

static Vertex *temp_vlist;
static Vertex *temp_vlist_x;
static int *temp_flist;
static Vertex *temp_tlist;
static Vertex *temp_tlist_x;
static Vertex *temp_nlist;
static Vertex *temp_nlist_x;
static int *orig_flist;
static int *muthas;

int datapos=0;
int filesize=0;
int vertexpos=0;
int vertices_splitted=0;
int muthapos=0;
int orig_fpos=0;
int vertexpos_x=0;
int texturepos_x=0;
int normalpos_x=0;
int facepos=0;

#define MAX_VERTS 1000000
#define MAX_FACES 2000000
#define MAX_ATTACH 16


//#define DEBUG_LOADMESH


/********************************
********  file reading  *********
********************************/

int getline(char *dest) {
  static char temp[1000];
  int x=datapos;

  memset(temp, 0, 1000);

  while(tempbuf[datapos]!=10) {
    temp[datapos-x]=tempbuf[datapos];
    datapos++;
    if ((datapos-x)>998) glueError("OBJ line too long!");
    if (datapos>=filesize) return 0;
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
  int vi, ti, ni;
  int newind;
} hasa;

static hasa hasataulu[MAX_VERTS*MAX_ATTACH];


int hashcheck(int vi, int ti, int ni) {
  int x;

  for (x=0; x<MAX_ATTACH; x++) {
    int i=vi*MAX_ATTACH+x;

    if (hasataulu[i].vi==vi && hasataulu[i].ti==ti && hasataulu[i].ni==ni) {
      return hasataulu[i].newind;
    }

    if (hasataulu[i].vi==-1) {

      hasataulu[i].vi=vi;
      hasataulu[i].ti=ti;
      hasataulu[i].ni=ni;
      hasataulu[i].newind=vertexpos;

      if (x>0) {
        muthas[muthapos+0]=vertexpos;
        muthas[muthapos+1]=hasataulu[vi*MAX_ATTACH].newind;
        muthapos+=2;
#ifdef DEBUG_LOADMESH
        if (muthapos>=MAX_FACES*2) glueError("glueLoadOBJ error #11");
        ONCE (glueNoticef("Vertices splitted!"););
#endif
        vertices_splitted++;
      }

      vertexpos++;
#ifdef DEBUG_LOADMESH
      if (vertexpos>=MAX_VERTS) glueError("glueLoadOBJ error #12");
#endif
      return hasataulu[i].newind;
    }
  }

  return -666;
}


int nu_vertex(int vi, int ti, int ni) {
  int plaa;

#ifdef DEBUG_LOADMESH
  if (vi<0 || ti<0 || ni<0) glueError("glueLoadOBJ error #1");
  if (vi>=MAX_VERTS || ti>=MAX_VERTS || ni>=MAX_VERTS) glueError("glueLoadOBJ error #2");
#endif

  plaa=hashcheck(vi, ti, ni);
  if (plaa==-666) {
    glueError("ERREUR: glueLoadOBJ out of hash!");
  }

#ifdef DEBUG_LOADMESH
  if (plaa>=MAX_VERTS || plaa<0) glueError("glueLoadOBJ error #3");
#endif

  temp_vlist[plaa]=temp_vlist_x[vi];
  temp_tlist[plaa]=temp_tlist_x[ti];
  temp_nlist[plaa]=temp_nlist_x[ni];

  if (vertexpos>MAX_VERTS-1) {
    glueError("ERREUR: glueLoadmesh out of vertexbuffer");
  }

  return plaa;
}

 
 
#define VERTEXPART 1
#define TEXCOORDPART 2
#define FACEPART 3
#define NORMALPART 4

void clearstuff() {
  int x;	
  
	for (x=0; x<MAX_VERTS*MAX_ATTACH; x++) hasataulu[x].vi=hasataulu[x].ti=hasataulu[x].ni=-1;

  orig_fpos=0;
  vertices_splitted=0;
  muthapos=0;
  vertexpos_x=0;
  texturepos_x=0;
  vertexpos=0;
  facepos=0;
  normalpos_x=0;
}


glue_static Object *glueLoadobjectX(
  int *indices, 
  Vertex *vertices, 
  Vertex *texcoords, 
  Vertex *normals,
  int *orig_indices,
  int *vertex_links, 
  int vc, 
  int fc, 
  int linkcnt, 
  int orig_vc, 
  int flgs) {
  	
  Object *obj=malloc(sizeof(Object));
  int flags=flgs;//&objloadmask;
  int x;

  // set pointers etc

  obj->fc=fc;
  obj->vc=vc;
  obj->ec=0;
  obj->faces=malloc(obj->fc*sizeof(Face));
  obj->normals=normals; 
  obj->vertices=vertices;
  obj->tangents=0;
  obj->texcoords=texcoords;
  obj->edges=0;
  obj->center.x=0;
  obj->center.y=0;
  obj->center.z=0;
  obj->size=0;
  obj->shadowlist=0;//glGenLists(1);
  obj->indices=indices;
  obj->colors=0;

  obj->orig_indices=orig_indices;
  obj->vertex_links=vertex_links;
  obj->linkcnt=linkcnt;

  // calculate center & size
  for (x=0; x<obj->vc; x++) {
    float d;

    obj->center.x+=obj->vertices[x].x;
    obj->center.y+=obj->vertices[x].y;
    obj->center.z+=obj->vertices[x].z;

    d=sqrt(obj->vertices[x].x*obj->vertices[x].x + obj->vertices[x].y*obj->vertices[x].y + obj->vertices[x].z*obj->vertices[x].z);
    if (d>=obj->size) obj->size=d;
  }

  obj->center.x=obj->center.x/(float)vc;
  obj->center.y=obj->center.y/(float)vc;
  obj->center.z=obj->center.z/(float)vc;


  // set vertex pointer for faces
  for (x=0; x<obj->fc; x++) {
    int i1, i2, i3;
    //Vertex fnorm;

    i1=indices[x*3+0];
    i2=indices[x*3+1];
    i3=indices[x*3+2];

    obj->faces[x].v[0]=i1;
    obj->faces[x].v[1]=i2;
    obj->faces[x].v[2]=i3;

    obj->faces[x].cen.x=(obj->vertices[i1].x+obj->vertices[i2].x+obj->vertices[i3].x)/3.0;
    obj->faces[x].cen.y=(obj->vertices[i1].y+obj->vertices[i2].y+obj->vertices[i3].y)/3.0;
    obj->faces[x].cen.z=(obj->vertices[i1].z+obj->vertices[i2].z+obj->vertices[i3].z)/3.0;
  }

/*
  if (obj->texcoords==0 && flags&GLUE_TEXGEN_NORMALS) {
    obj->texcoords=glueTexgen_normals(obj->normals, obj->vc);
  }

  if (obj->colors==0 && flags&GLUE_VCOLOR_NORMALS) {
    obj->colors=glueVertexcolor_normals(obj->normals, obj->vc);
  }
*/

  if (!normals) {
    obj->normals=malloc(obj->vc*sizeof(Vertex));
    calc_objnormals(obj);
  }

	if (obj->tangents == 0 && flags&GLUE_GEN_TANGENTS) {
		calc_objtangents(obj);
	}

  // build displaylist for object
  obj->objnum=glGenLists(1);

//  buildlist(obj->objnum, obj->vertices, obj->normals, obj->texcoords, obj->colors, indices, obj->vc, obj->fc);
  buildlist2(obj->objnum, obj->vertices, obj->normals, obj->texcoords, obj->colors, indices, obj->vc, obj->fc, obj->tangents);

  glueLoading();

  return obj;
}



Object *load_objdata(int flags) {
	Object *obj;
  Vertex *new_vlist=malloc(sizeof(Vertex)*vertexpos);
  int *new_flist=malloc(sizeof(int)*facepos*3);
  Vertex *new_tlist=0;
  Vertex *new_nlist=0;
  int *new_orig_flist=0;
  int *new_muthas=0;
  
  memcpy(new_vlist, temp_vlist, sizeof(Vertex)*vertexpos);
  memcpy(new_flist, temp_flist, sizeof(int)*facepos*3);
  
#ifdef DEBUG_LOADMESH
  glueNoticef("load_objdata memcpy1 done");
#endif
    
  // texcoords
  if (texturepos_x) {
    new_tlist=malloc(sizeof(Vertex)*vertexpos);
    memcpy(new_tlist, temp_tlist, sizeof(Vertex)*vertexpos);
#ifdef DEBUG_LOADMESH
    glueNoticef("load_objdata tex memcpy done");
#endif
  }

  // normals
  if (normalpos_x) {
    new_nlist=malloc(sizeof(Vertex)*vertexpos);
    memcpy(new_nlist, temp_nlist, sizeof(Vertex)*vertexpos);
#ifdef DEBUG_LOADMESH
    glueNoticef("load_objdata normal memcpy done");
#endif
  }

  // original facedata
  if (vertices_splitted) {
    new_orig_flist=malloc(sizeof(int)*facepos*3);
    new_muthas=malloc(sizeof(int)*muthapos);
    
    memcpy(new_orig_flist, orig_flist, sizeof(int)*facepos*3);
    memcpy(new_muthas, muthas, sizeof(int)*muthapos);
#ifdef DEBUG_LOADMESH
    glueNoticef("load_objdata splitted memcpy done");
#endif
  }
      
  obj = glueLoadobjectX(
    new_flist, //int *indices,
    new_vlist, //Vertex *vertices,
    texturepos_x?new_tlist:0, //Vertex *texcoords,
    new_nlist, //Vertex *normals,
    vertices_splitted?new_orig_flist:0, //int *orig_indices,
    vertices_splitted?new_muthas:0, //int *vertex_links,
    vertexpos, //int vc,
    facepos, //int fc,
    vertices_splitted?muthapos/2:0, //int linkcnt,
    vertices_splitted?vertexpos_x:0, //int orig_vc,
    flags); //int flgs);

#ifdef DEBUG_LOADMESH
  glueNoticef("glueLoadobject done");
#endif

  return obj;
}


Mesh *glueLoadmesh(char *filename, int flags) {
  Mesh *mesh;
  char rivi[1000];
  char subrivi[1000];
  int x;
  int numgroups=0;
  int current_group=0;
  int current_part=0;
  int indexminus_v=1;
  int indexminus_t=1;
  int indexminus_n=1;
    
  // load file
  tempbuf=glueLoadfile(filename);
  filesize=glueFilelength(filename);
  datapos=0;

#ifdef DEBUG_LOADMESH
  glueNoticef("file %s loaded: %i bytes", filename, filesize);
#endif 
  
  // count number of groups
  while (1) {
    if (!getline(rivi)) break;
    substr(rivi, subrivi, 2);
    if (strcmp(subrivi, "g ")==0) numgroups++;
  }
  datapos=0;  

#ifdef DEBUG_LOADMESH
  glueNoticef("found %i groups", numgroups);
#endif 

  if (numgroups<1) {
  	glueNoticef("No groups found in %s, assuming 1", filename);
  	numgroups=1;
  }

  mesh=malloc(sizeof(Mesh));
  mesh->numgroups=numgroups;
  mesh->groups=malloc(sizeof(Object *)*numgroups);

  temp_vlist=malloc(sizeof(Vertex)*MAX_VERTS);
  temp_vlist_x=malloc(sizeof(Vertex)*MAX_VERTS);

  temp_tlist=malloc(sizeof(Vertex)*MAX_VERTS);
  temp_tlist_x=malloc(sizeof(Vertex)*MAX_VERTS);

  temp_nlist=malloc(sizeof(Vertex)*MAX_VERTS);
  temp_nlist_x=malloc(sizeof(Vertex)*MAX_VERTS);

  temp_flist=malloc(sizeof(int)*MAX_FACES*3);
  orig_flist=malloc(sizeof(int)*MAX_FACES*3);

  muthas=malloc(sizeof(int)*MAX_FACES*2);

#ifdef DEBUG_LOADMESH
  glueNoticef("mallox done");
#endif 

  clearstuff();

  while (1) {

    if (!getline(rivi)) break;
    substr(rivi, subrivi, 2);

    // verteksi:
    if (strcmp(subrivi, "v ")==0) {
      float xx=0, yy=0, zz=0;

      // next group, maybe, if not then fuck OBJ
      if (current_part==FACEPART) {
#ifdef DEBUG_LOADMESH
        glueNoticef("group %i loaded, %i vertices, %i texcoords, %i faces, %i normals", 
          current_group, vertexpos_x, texturepos_x, facepos, normalpos_x);
#endif
   
        mesh->groups[current_group]=load_objdata(flags);
   
        indexminus_v+=vertexpos_x;
        indexminus_t+=texturepos_x;
        indexminus_n+=normalpos_x;
        clearstuff();
        current_part=VERTEXPART;
        current_group++;
        if (current_group>numgroups-1) glueError("OBJ data order error with groups"); 
      }

      sscanf(rivi, "v %f %f %f", &xx, &yy, &zz);
      temp_vlist_x[vertexpos_x].x=xx;
      temp_vlist_x[vertexpos_x].y=yy;
      temp_vlist_x[vertexpos_x].z=zz;
      vertexpos_x++;
#ifdef DEBUG_LOADMESH
      if (vertexpos_x>=MAX_VERTS) glueErrorf("glueLoadOBJ error #4");
#endif

    } else if (strcmp(subrivi, "vt")==0) {
      // texcoord
      float uu=0, vv=0;

      current_part=TEXCOORDPART;

      sscanf(rivi, "vt %f %f", &uu, &vv);

      temp_tlist_x[texturepos_x].x=uu;
      temp_tlist_x[texturepos_x].y=vv;
      temp_tlist_x[texturepos_x].z=0;
      texturepos_x++;
      
#ifdef DEBUG_LOADMESH
      if (texturepos_x>=MAX_VERTS) glueErrorf("glueLoadOBJ error #5: %i", texturepos_x);
#endif

    } else if (strcmp(subrivi, "vn")==0) {
      // normal
      float nx=0, ny=0, nz=0;

      current_part=NORMALPART;

      sscanf(rivi, "vn %f %f %f", &nx, &ny, &nz);

      temp_nlist_x[normalpos_x].x=nx;
      temp_nlist_x[normalpos_x].y=ny;
      temp_nlist_x[normalpos_x].z=nz;
      normalpos_x++;
      
#ifdef DEBUG_LOADMESH
      if (normalpos_x>=MAX_VERTS) glueErrorf("glueLoadOBJ error #5321: %i", normalpos_x);
#endif



    } else if (strcmp(subrivi, "f ")==0) {
      // face
      static int v1, v2, v3;
      static int t1, t2, t3;
      static int n1, n2, n3;
      static int vv1, vv2, vv3;

      current_part=FACEPART;

      if (texturepos_x && normalpos_x) { // texturecoords+normals
      	sscanf(rivi, "f %i/%i/%i %i/%i/%i %i/%i/%i", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
      } else if (texturepos_x) { // texcoords
      	sscanf(rivi, "f %i/%i %i/%i %i/%i", &v1, &t1, &v2, &t2, &v3, &t3);
      	n1=n2=n3=indexminus_n;
      } else if (normalpos_x) { // normals
      	sscanf(rivi, "f %i//%i %i//%i %i//%i", &v1, &n1, &v2, &n2, &v3, &n3);
        t1=t2=t3=indexminus_t;
      } else {
        sscanf(rivi, "f %i %i %i", &v1, &v2, &v3);
        t1=t2=t3=indexminus_t;
      	n1=n2=n3=indexminus_n;
      }
      
      v1-=indexminus_v;
      v2-=indexminus_v;
      v3-=indexminus_v;
      t1-=indexminus_t;
      t2-=indexminus_t;
      t3-=indexminus_t;
      n1-=indexminus_n;
      n2-=indexminus_n;
      n3-=indexminus_n;

#ifdef DEBUG_LOADMESH
       if (v1<0 || v2<0 || v3<0) glueErrorf("index out of bounds error");
       if (v1>vertexpos_x || v2>vertexpos_x || v3>vertexpos_x) glueErrorf("index out of bounds error");
       if (texturepos_x) {
         if (t1<0 || t2<0 || t3<0) glueErrorf("texcoord index out of bounds error");
         if (t1>texturepos_x || t2>texturepos_x || t3>texturepos_x) glueErrorf("texcoord index out of bounds error");
       }
       if (normalpos_x) {
         if (n1<0 || n2<0 || n3<0) glueErrorf("normal index out of bounds error");
         if (n1>normalpos_x || n2>normalpos_x || n3>normalpos_x) glueErrorf("normal index out of bounds error");
       }
#endif 

      vv1=nu_vertex(v1, t1, n1);
      vv2=nu_vertex(v2, t2, n2);
      vv3=nu_vertex(v3, t3, n3);

#ifdef DEBUG_LOADMESH
      if (vv1<0 || vv1>=MAX_VERTS) glueError("glueLoadOBJ error #13");
      if (vv2<0 || vv2>=MAX_VERTS) glueError("glueLoadOBJ error #14");
      if (vv3<0 || vv3>=MAX_VERTS) glueError("glueLoadOBJ error #15");
#endif 

#ifdef DEBUG_LOADMESH
      if (orig_fpos<0 || orig_fpos>=MAX_FACES) glueError("glueLoadOBJ error #22");
#endif
      orig_flist[orig_fpos*3+0]=hasataulu[(v1)*MAX_ATTACH].newind;
      orig_flist[orig_fpos*3+1]=hasataulu[(v2)*MAX_ATTACH].newind;
      orig_flist[orig_fpos*3+2]=hasataulu[(v3)*MAX_ATTACH].newind;
      
#ifdef DEBUG_LOADMESH
      if (orig_flist[orig_fpos*3+0]<0) glueError("glueLoadOBJ error #16");
      if (orig_flist[orig_fpos*3+0]>=MAX_FACES) glueError("glueLoadOBJ error #17");
      if (orig_flist[orig_fpos*3+1]<0) glueError("glueLoadOBJ error #18");
      if (orig_flist[orig_fpos*3+1]>=MAX_FACES) glueError("glueLoadOBJ error #19");
      if (orig_flist[orig_fpos*3+2]<0) glueError("glueLoadOBJ error #20");
      if (orig_flist[orig_fpos*3+2]>=MAX_FACES) glueError("glueLoadOBJ error #21");
#endif 
      orig_fpos++;
      
#ifdef DEBUG_LOADMESH
      if (orig_fpos>=MAX_FACES) glueError("glueLoadOBJ error #9");
#endif 

      temp_flist[facepos*3+0]=vv1;
      temp_flist[facepos*3+1]=vv2;
      temp_flist[facepos*3+2]=vv3;
      facepos++;
#ifdef DEBUG_LOADMESH
      if (facepos>=MAX_FACES) glueError("glueLoadOBJ error #10");
#endif 
    }
  }

#ifdef DEBUG_LOADMESH
  glueNoticef("last group %i loaded, %i vertices, %i texcoords, %i faces, %i normals", 
    current_group, vertexpos_x, texturepos_x, facepos, normalpos_x);
#endif 

  // load the last group too
  mesh->groups[current_group]=load_objdata(flags);

  free(tempbuf);
  free(temp_vlist_x);
  free(temp_vlist);
  free(temp_flist);
  free(temp_tlist);
  free(temp_tlist_x);
  free(temp_nlist);
  free(temp_nlist_x);
  free(orig_flist);
  free(muthas);
  
  
  return mesh;
}
