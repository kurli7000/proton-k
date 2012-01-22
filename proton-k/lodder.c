
#include "lodder.h"

Vertex *lodder_vertices[MAX_GROUPS][MAX_LOD_REDUCTION];
//float *lodder_colors[MAX_GROUPS][MAX_LOD_REDUCTION];
//Vertex *lodder_normals[MAX_GROUPS][MAX_LOD_REDUCTION];
Mesh *loddermesh;

/* käyttöön jos tarvitaan useita lod-meshejä
typedef struct {
  Vertex *lodder_vertices[MAX_GROUPS][MAX_LOD_REDUCTION];
  Mesh *loddermesh;
} Lodder;
*/


///////////////////////////////////// logging

static int lognumber=0;

void logerror(char *fmt, ...) {
  static char buf[8000];
  int retval;
  va_list ap;
  static FILE *f=0;
  static int current_log=0;
  static char filename[100];
  
  if (!f) {
 	  sprintf(filename, "log%03i.obj", lognumber);
  	f=fopen(filename, "w");
  }
  	
  if (current_log!=lognumber) {
  	fclose(f);
 	  sprintf(filename, "log%03i.obj", lognumber);
  	f=fopen(filename, "w");
  	current_log=lognumber;
  }

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  
  fprintf(f, buf);
  fflush(f);
}

void logerror_next() {
	lognumber++;
}

void dump_obj(Vertex *data, int vc, int *indices, int fc) {
 	for (int ii=0; ii<vc; ii++) logerror("v %8.4f %8.4f %8.4f\n", data[ii].x, data[ii].y, data[ii].z);
 	logerror("g apuva\n");
 	for (int ii=0; ii<fc; ii++) logerror("f %i %i %i\n", indices[ii*3+0]+1, indices[ii*3+1]+1, indices[ii*3+2]+1);  		
  logerror_next();
}


////////////////////////////////////////////// lodder mainloop


void meshlodder(Mesh *mesh) {
  Mvert *mv;
  int x;
  int group, lod;
  
  if (mesh->numgroups>MAX_GROUPS) glueError("too many groups in mesh!");
  loddermesh=mesh;
  
  // loop mesh groups
  for (group=0; group<mesh->numgroups; group++) {
  	Object *obu=mesh->groups[group];
  	int vc=obu->vc;

//    if (obu->orig_indices) glueError("object contains splitted shit, no way jose!");

    mv=malloc(sizeof(Mvert)*vc);

    for (x=0; x<vc; x++) {
    	mv[x].pos=obu->vertices[x];
  	  mv[x].parent=NPAR;
    }

    // fix parents for splitted vertices
    if (obu->orig_indices) {
      for (x=0; x<obu->linkcnt; x++) {
        int splitted, parent;

        splitted=obu->vertex_links[x*2+0];
        parent=obu->vertex_links[x*2+1]; 
        mv[splitted].parent=parent;
      }
    } 
    

    // loop lods
    for (lod=0; lod<MAX_LOD_REDUCTION; lod++) {
    	int killed;

      //int mergevertices(Mvert *data, int vc, int *indices, int fc) {
      killed=mergevertices(mv, vc, obu->indices, obu->fc);
#ifdef DEBUG_LODDER
      glueNoticef("mergevertices group %i lod %i, vc=%i killed=%i", group, lod, vc, killed);
#endif
      lodder_vertices[group][lod]=unpack_mvbuffer(mv, vc);
#ifdef DEBUG_LODDER
      dump_obj(lodder_vertices[group][lod], vc, obu->indices, obu->fc);
#endif
      //lodder_normals[group][lod]=calc_lodder_normals(lodder_vertices[group][lod], vc, i2, fc2);
    } // lod-loop
  
    free(mv);  
    glueLoading();
  } // mesh group loop
}  

////////////////////////////////////////////// rootfinder

int getroot(Mvert *vbuf, int x, int *depth) {
  int p, i=x;
  int loop=0;
  
  while (1) {
    p=vbuf[i].parent;
    if (p==NPAR) break;
    i=p;
    loop++;
    if (loop>1000000) glueError("getroot loop detected");
  }
  
  *depth=loop;
  // optimoi
  // vbuf[x].parent=i;
  return i;
}

////////////////////////////////////////////// merge shit


int mergevertices(Mvert *data, int vc, int *indices, int fc) {
  int killed=0;
  int x;
  float ea=0.0;
  int ec=0;
  
  //  calc edge average
  for (x=0; x<fc; x++) {
  	int v1, v2, v3;
  	int d;

  	v1=getroot(data, indices[x*3+0], &d);
  	v2=getroot(data, indices[x*3+1], &d);
  	v3=getroot(data, indices[x*3+2], &d);
  	
   	if (v1==v2 || v2==v3 || v3==v1) {
  	  // degenerate
  	} else {
    	ea+=v_len(data[v1].pos, data[v2].pos);
  	  ea+=v_len(data[v2].pos, data[v3].pos);
  	  ea+=v_len(data[v3].pos, data[v1].pos);  		
  		ec+=3;
  	} 	
  }
  ea/=(float)ec;
  ea*=0.75; // kludge
  
  // merge
  for (x=0; x<fc; x++) {
  	int v1, v2, v3;
    int d;
    
  	v1=getroot(data, indices[x*3+0], &d);
  	v2=getroot(data, indices[x*3+1], &d);
  	v3=getroot(data, indices[x*3+2], &d);
 
#ifdef DEBUG_LODDER
    if (v1<0 || v1>=vc) glueErrorf("getroot fail v1: %i", v1);
    if (v2<0 || v2>=vc) glueErrorf("getroot fail v2: %i", v2);
    if (v3<0 || v3>=vc) glueErrorf("getroot fail v3: %i", v3);
#endif
  	
  	if (v1==v2 || v2==v3 || v3==v1) {
  	  // degenerate
  	} else {
    	float e1=v_len(data[v1].pos, data[v2].pos);
  	  float e2=v_len(data[v2].pos, data[v3].pos);
  	  float e3=v_len(data[v3].pos, data[v1].pos);

      // if one of the edges is shorter than avg*kludge, kill the shortest
      if (e1<ea || e2<ea || e3<ea) {
    		if (e1<e2 && e1<e3) {
      		data[v2].parent = v1;			
      		killed++;
  		  } else if (e2<e1 && e2<e3) {
  		    data[v3].parent = v2;  			
    		  killed++;
		    } else { // if (e3<e1 && e3<e2) {
    		  data[v1].parent = v3;		
    		  killed++;
  	    }
  	  }
  	}		
  }
  return killed;
}

///////////////////////////////////////// unpack

Vertex *unpack_mvbuffer(Mvert *data, int vc) {
  Vertex *v=malloc(sizeof(Vertex)*vc);
  int x;

  for (x=0; x<vc; x++) {
  	int depth;
  	int i=getroot(data, x, &depth);
    v[x]=data[i].pos;
  }
  return v;
}


////////////////////////////////////////// render


void calc_fnorm_nn(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *n) {
  n->x=(v1->y-v3->y)*(v2->z-v3->z)-(v1->z-v3->z)*(v2->y-v3->y);
  n->y=(v1->z-v3->z)*(v2->x-v3->x)-(v1->x-v3->x)*(v2->z-v3->z);
  n->z=(v1->x-v3->x)*(v2->y-v3->y)-(v1->y-v3->y)*(v2->x-v3->x);
}


void render_lodder_sphere(int group, Vertex spherepos, Vertex spherepos2, float diam, float powr, float push, float timex) {
  Vertex *loddata[MAX_LOD_REDUCTION+1];  
  Vertex *v, *n;
  int vc=loddermesh->groups[group]->vc;
  int fc=loddermesh->groups[group]->fc;
  int *i=loddermesh->groups[group]->indices;
  float *c;
  
  unsigned char *lodlevels;

  // assign lod buffers, 0=original
  loddata[0]=loddermesh->groups[group]->vertices;
  for (int i=0; i<MAX_LOD_REDUCTION; i++) loddata[i+1]=lodder_vertices[group][i];

  v=tmpmalloc(sizeof(Vertex)*vc);
  n=tmpmalloc(sizeof(Vertex)*vc);
  c=tmpmalloc(sizeof(float)*vc*4);
  lodlevels=tmpmalloc(vc);
  
  // calc vertices
  for (int x=0; x<vc; x++) {
    Vertex l0pos=loddata[0][x];
    float xx=(l0pos.x-spherepos.x);
    float yy=(l0pos.y-spherepos.y);
    float zz=(l0pos.z-spherepos.z);
    float xx2=(l0pos.x-spherepos2.x);
    float yy2=(l0pos.y-spherepos2.y);
    float zz2=(l0pos.z-spherepos2.z);
    float dist=sqrt(xx*xx+yy*yy+zz*zz);
    float dist2=sqrt(xx2*xx2+yy2*yy2+zz2*zz2);
    float powah=r0(diam-dist)/diam; // 0..1 sphere
    float powah2=r0(diam-dist2)/diam; 
    float lod=clamp((powah+powah2)*powr);
    //lod=clamp(powr);
    float lodfract=lod*(float)MAX_LOD_REDUCTION;
    int bufnum=(int)floor(lodfract);

    lodfract=fmod(lodfract, 1.0);
    
    c[x*3+0]=c[x*3+1]=c[x*3+2]=clamp((lod-0.1)*4.0);
    // calc lodded pos  
    if (lod==0.0) {
      v[x]=l0pos;
      lodlevels[x]=0;
    } else if (lod==1.0) {
      v[x]=loddata[MAX_LOD_REDUCTION][x];
      lodlevels[x]=1;
    } else {
      Vertex l0=loddata[bufnum][x];
      Vertex l1=loddata[bufnum+1][x];
      v[x].x = l0.x*(1.0-lodfract)+l1.x*(lodfract);
      v[x].y = l0.y*(1.0-lodfract)+l1.y*(lodfract);
      v[x].z = l0.z*(1.0-lodfract)+l1.z*(lodfract);
      lodlevels[x]=1;      
    }
    
    // push
    Vertex pd=new_v(v[x].x-spherepos.x, v[x].y-spherepos.y, v[x].z-spherepos.z);
    Vertex pd2=new_v(v[x].x-spherepos2.x, v[x].y-spherepos2.y, v[x].z-spherepos2.z);
    normalize(&pd);
    normalize(&pd2);
    v[x].x+=push*powah*pd.x+push*powah2*pd2.x;
    v[x].y+=push*powah*pd.y+push*powah2*pd2.y;
    v[x].z+=push*powah*pd.z+push*powah2*pd2.z;

    // little something for normal
    n[x]=new_v(0.0, 0.00001, 0.0);
  }

  // calc normals
  for (int x=0; x<fc; x++) {
    Vertex fnorm;
    Vertex *v1, *v2, *v3;
    Vertex *n1, *n2, *n3;
    v1=&v[i[x*3+0]];
    v2=&v[i[x*3+1]];
    v3=&v[i[x*3+2]];
    n1=&n[i[x*3+0]];
    n2=&n[i[x*3+1]];
    n3=&n[i[x*3+2]];
  	calc_fnorm_nn(v1, v2, v3, &fnorm);
    n1->x+=fnorm.x; n1->y+=fnorm.y; n1->z+=fnorm.z;
    n2->x+=fnorm.x; n2->y+=fnorm.y; n2->z+=fnorm.z;
    n3->x+=fnorm.x; n3->y+=fnorm.y; n3->z+=fnorm.z;
  }

  // normalize normals, for lod 0 use original normal
  for (int x=0; x<vc; x++) {
  	if (lodlevels[x]==0) n[x]=loddermesh->groups[group]->normals[x];
  	else normalize(&n[x]);
  }

  // render arrays
//  glUniform1fARB(glGetUniformLocationARB(shader, "depthmult"), 1.0);  
  glue_disableallarrays();
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, n);
  /*
  Vertex *t=loddermesh->groups[group]->texcoords;
  if (t) {
    glClientActiveTexture(GL_TEXTURE0_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
  }*/
  
  // alkup. verteksit texturekoordinaateiks
  glClientActiveTexture(GL_TEXTURE1_ARB);
  glActiveTexture(GL_TEXTURE1_ARB);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(3, GL_FLOAT, 0, loddata[0]);
  
  glDrawElements(GL_TRIANGLES, fc*3, GL_UNSIGNED_INT, i);

/*
  // wireframe  
  //glUniform1fARB(glGetUniformLocationARB(shader, "depthmult"), 0.0);  
  //renderflags(GLUE_BLEND_ALPHAADD|GLUE_CHECK_DEPTH);
  renderflags(GLUE_BLEND|GLUE_CHECK_DEPTH);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(1.0, 1.0);
  //glEnable(GL_LINE_SMOOTH);
  glLineWidth(glueXres/320);
  glueDisabletexture();
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_FLOAT, 0, c);
  glDrawElements(GL_TRIANGLES, fc*3, GL_UNSIGNED_INT, i);
  glue_disableallarrays();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDisable(GL_POLYGON_OFFSET_LINE);
*/
  glue_disableallarrays();

  tmpfree(lodlevels);
  tmpfree(c);  
  tmpfree(n);
  tmpfree(v);  
}



