
#ifndef FORGET_INCLUDES

#include "glueobj.h"
#include "glueext.h"
#include "gluegen.h"
#ifdef I_LOVE_BILL
#include "gluewin.h"
#else
#include "gluelin.h"
#endif
#include <GL/gl.h>

#endif // FORGET_INCLUDES


/**********************************************************************
  Object related stuff
**********************************************************************/

glue_static void glue_disableallarrays(void) {

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE7_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE3_ARB);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE2_ARB);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE1_ARB);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE0_ARB);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}

glue_static void buildlist2(int list, Vertex *v, Vertex *n, Vertex *t, Vertex *c, int *ind, int vc, int fc, Color *tangents) {

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

  glue_disableallarrays();

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, n);

	if (tangents != 0) {
		glClientActiveTexture(GL_TEXTURE7_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(4, GL_FLOAT, 0, tangents);
	}

  if (t!=0) {
    glClientActiveTexture(GL_TEXTURE3_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
    glClientActiveTexture(GL_TEXTURE2_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
    glClientActiveTexture(GL_TEXTURE1_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
    glClientActiveTexture(GL_TEXTURE0_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
  }

  if (c!=0) {
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, c);
  }

  glNewList(list, GL_COMPILE);
  glDrawElements(GL_TRIANGLES, fc*3, GL_UNSIGNED_INT, ind);
  glEndList();

  glPopClientAttrib();
  glPopAttrib();
}



glue_static void buildlist(int list, Vertex *v, Vertex *n, Vertex *t, Vertex *c, int *ind, int vc, int fc) {

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

  glue_disableallarrays();

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, v);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, n);

  if (t!=0) {
    glClientActiveTexture(GL_TEXTURE3_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
    glClientActiveTexture(GL_TEXTURE2_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
    glClientActiveTexture(GL_TEXTURE1_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
    glClientActiveTexture(GL_TEXTURE0_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(3, GL_FLOAT, 0, t);
  }

  if (c!=0) {
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, c);
  }

  glNewList(list, GL_COMPILE);
  glDrawElements(GL_TRIANGLES, fc*3, GL_UNSIGNED_INT, ind);
  glEndList();

  glPopClientAttrib();
  glPopAttrib();
}





glue_static Vertex *glueTexgen_normals(Vertex *norm, int vc) {
  int x;
  Vertex *t=malloc(sizeof(Vertex)*vc);

  for (x=0; x<vc; x++) {
    t[x].x=atan2(norm[x].x, norm[x].z)/(pi*2.0)+0.5;
    t[x].y=norm[x].y*0.5+0.5;
    t[x].z=0;
  }

  return t;
}

glue_static Vertex *glueVertexcolor_normals(Vertex *norm, int vc) {
  int x;
  Vertex *c=malloc(sizeof(Vertex)*vc);

  for (x=0; x<vc; x++) {
    c[x].x=norm[x].x*0.5+0.5;
    c[x].y=norm[x].y*0.5+0.5;
    c[x].z=norm[x].z*0.5+0.5;
  }

  return c;
}


glue_static void calc_objnormals(Object *obj) {
  Vertex *v, *n;
  int *i;
  int x;

  v=obj->vertices;
  n=obj->normals;

  if (obj->orig_indices) i=obj->orig_indices;
  else i=obj->indices;

  for (x=0; x<obj->vc; x++) n[x]=new_v(0, 0, 0);


  for (x=0; x<obj->fc; x++) {
    int i1, i2, i3;
    Vertex fnorm;

    i1=i[x*3+0];
    i2=i[x*3+1];
    i3=i[x*3+2];

    calc_fnorm(&v[i1], &v[i2], &v[i3], &fnorm);

    n[i1].x+=fnorm.x; n[i1].y+=fnorm.y; n[i1].z+=fnorm.z;
    n[i2].x+=fnorm.x; n[i2].y+=fnorm.y; n[i2].z+=fnorm.z;
    n[i3].x+=fnorm.x; n[i3].y+=fnorm.y; n[i3].z+=fnorm.z;

    obj->faces[x].norm=fnorm;
  }

  if (obj->orig_indices) {
    for (x=0; x<obj->linkcnt; x++) {
      int l1, l2;

      l1=obj->vertex_links[x*2+0]; // splitattu verteksi
      l2=obj->vertex_links[x*2+1]; // originaali
      obj->normals[l1]=obj->normals[l2];
    }
  }

  for (x=0; x<obj->vc; x++) normalize(&n[x]);
}



glue_static void calc_objtangents(Object *obj) {
	int i;
	Vertex *tan1 = malloc(obj->vc*sizeof(Vertex));
	Vertex *tan2 = malloc(obj->vc*sizeof(Vertex));

  obj->tangents = malloc(obj->vc*sizeof(Color));

	for (i = 0; i < obj->vc; i++) {
		tan1[i].x = tan1[i].y = tan1[i].z = 0.0;
		tan2[i].x = tan2[i].y = tan2[i].z = 0.0;
	}

	for (i = 0; i < obj->fc; i++) {
		int i1 = obj->faces[i].v[0];
		int i2 = obj->faces[i].v[1];
		int i3 = obj->faces[i].v[2];

		Vertex v1 = obj->vertices[i1];
		Vertex v2 = obj->vertices[i2];
		Vertex v3 = obj->vertices[i3];

		Vertex w1 = obj->texcoords[i1];
		Vertex w2 = obj->texcoords[i2];
		Vertex w3 = obj->texcoords[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

		float r = 1.0f / (s1*t2 - s2*t1);
        Vertex sdir = new_v((t2*x1-t1*x2)*r, (t2*y1-t1*y2)*r, (t2*z1-t1*z2)*r);
        Vertex tdir = new_v((s1*x2-s2*x1)*r, (s1*y2-s2*y1)*r, (s1*z2-s2*z1)*r);

        tan1[i1] = v_add(&tan1[i1], &sdir);
        tan1[i2] = v_add(&tan1[i2], &sdir);
        tan1[i3] = v_add(&tan1[i3], &sdir);

		tan2[i1] = v_add(&tan2[i1], &tdir);
		tan2[i2] = v_add(&tan2[i2], &tdir);
		tan2[i3] = v_add(&tan2[i3], &tdir);
	}

	for (i = 0; i < obj->vc; i++) {
		Vertex n = obj->normals[i];
		Vertex t = tan1[i];
        Vertex nct;
		float dotti = dot(&n, &t);
		Vertex tmp = new_v(dotti, dotti, dotti);

		tmp = v_mult(&n, &tmp);
		tmp = v_sub(&t, &tmp);
		normalize(&tmp);

		obj->tangents[i].r = tmp.x;
		obj->tangents[i].g = tmp.y;
		obj->tangents[i].b = tmp.z;

		nct = cross(&n, &t);
		dotti = dot(&nct, &tan2[i]);
		obj->tangents[i].a = dotti < 0.0f ? 1.0f : -1.0f;

//		printf("tangent %i: %f %f %f\n", i, tmp.x, tmp.y, tmp.z);
/*		tangent[i].xyz = (t-n*Dot(n, t)).normalize();
		tangent[i].a = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;*/
	}

	free(tan1);
	free(tan2);


/*	for (i = 0; i < obj->fc; i++) {
	    Vertex edge1, edge2;
	    edge1.x = obj->vertices[obj->faces[i].v[1]].x - obj->vertices[obj->faces[i].v[0]].x;
	    edge1.y = obj->vertices[obj->faces[i].v[1]].y - obj->vertices[obj->faces[i].v[0]].y;
	    edge1.z = obj->vertices[obj->faces[i].v[1]].z - obj->vertices[obj->faces[i].v[0]].z;
	    edge2.x = obj->vertices[obj->faces[i].v[2]].x - obj->vertices[obj->faces[i].v[0]].x;
	    edge2.y = obj->vertices[obj->faces[i].v[2]].y - obj->vertices[obj->faces[i].v[0]].y;
	    edge2.z = obj->vertices[obj->faces[i].v[2]].z - obj->vertices[obj->faces[i].v[0]].z;
	    normalize(&edge1);
	    normalize(&edge2);

	    Grid texEdge1, texEdge2;
	    texEdge1.x = obj->texcoords[obj->faces[i].v[1]].x - obj->texcoords[obj->faces[i].v[0]].x;
	    texEdge1.y = obj->texcoords[obj->faces[i].v[1]].y - obj->texcoords[obj->faces[i].v[0]].y;
	    texEdge2.x = obj->texcoords[obj->faces[i].v[2]].x - obj->texcoords[obj->faces[i].v[0]].x;
	    texEdge2.y = obj->texcoords[obj->faces[i].v[2]].y - obj->texcoords[obj->faces[i].v[0]].y;
	    float len = 1.0/sqrt(texEdge1.x*texEdge1.x+texEdge1.y+texEdge1.y);
	    texEdge1.x *= len;
	    texEdge1.y *= len;
	    len = 1.0/sqrt(texEdge2.x*texEdge2.x+texEdge2.y+texEdge2.y);
	    texEdge2.x *= len;
	    texEdge2.y *= len;

	    Vertex t,b,n;
	    n.x = obj->faces[i].norm.x;
	    n.y = obj->faces[i].norm.y;
	    n.z = obj->faces[i].norm.z;

		float det = (texEdge1.x*texEdge2.y)-(texEdge1.y*texEdge2.x);
		if (det < 0.0001f) {
			t.x = 1.0; t.y = 0.0; t.z = 0.0;
			b.x = 0.0; b.y = 1.0; b.z = 0.0;
		} else {
			det = 1.0f/det;

	        t.x = (texEdge2.y*edge1.x-texEdge1.y*edge2.x)*det;
	        t.y = (texEdge2.y*edge1.y-texEdge1.y*edge2.y)*det;
	        t.z = (texEdge2.y*edge1.z-texEdge1.y*edge2.z)*det;

	        b.x = (-texEdge2.x*edge1.x+texEdge1.x*edge2.x)*det;
	        b.y = (-texEdge2.x*edge1.y+texEdge1.x*edge2.y)*det;
	        b.z = (-texEdge2.x*edge1.z+texEdge1.x*edge2.z)*det;

			normalize(&t);
			normalize(&b);
		}

		Vertex bitangent;
		bitangent = cross(&n, &t);

		float bb = dot(&bitangent, &b);
		float handedness = bb < 0.0 ? -1.0 : 1.0;	// unused atm

		obj->tangents[obj->faces[i].v[0]].r = t.x;
		obj->tangents[obj->faces[i].v[0]].g = t.y;
		obj->tangents[obj->faces[i].v[0]].b = t.z;
		obj->tangents[obj->faces[i].v[0]].a = handedness;

		obj->tangents[obj->faces[i].v[1]].r = t.x;
		obj->tangents[obj->faces[i].v[1]].g = t.y;
		obj->tangents[obj->faces[i].v[1]].b = t.z;
		obj->tangents[obj->faces[i].v[1]].a = handedness;

		obj->tangents[obj->faces[i].v[2]].r = t.x;
		obj->tangents[obj->faces[i].v[2]].g = t.y;
		obj->tangents[obj->faces[i].v[2]].b = t.z;
		obj->tangents[obj->faces[i].v[2]].a = handedness;
	}*/

}

glue_static Object *glueLoadobject(int *indices, Vertex *vertices, Vertex *texcoords, int *neighbours, Vertex *colors, Edge *edges, int *orig_indices,
                       int *vertex_links, int vc, int fc, int ec, int linkcnt, int orig_vc, int flgs) {
  Object *obj=malloc(sizeof(Object));
  int flags=flgs;//&objloadmask;
  int x;

  // set pointers etc

  obj->fc=fc;
  obj->vc=vc;
  obj->ec=ec;
  obj->faces=malloc(obj->fc*sizeof(Face));
  obj->normals=malloc(obj->vc*sizeof(Vertex));
  obj->vertices=vertices;
  obj->tangents=0;
  obj->texcoords=texcoords;
  obj->edges=edges;
  obj->center.x=0;
  obj->center.y=0;
  obj->center.z=0;
  obj->size=0;
  obj->shadowlist=glGenLists(1);
  obj->indices=indices;
  obj->colors=colors;

  obj->orig_indices=orig_indices;
  obj->vertex_links=vertex_links;
  obj->linkcnt=linkcnt;


/*for (x=0; x<obj->vc; x++){
	obj->normals[x].y = obj->normals[x].z;
}*/
/*int p;
for (p = 0; p < obj->vc;  p++) {
	float mul = 2.0;
	obj->texcoords[p].x *= mul;
	obj->texcoords[p].y *= mul;
	obj->texcoords[p].z *= mul;
}*/


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

  // calc normals
  calc_objnormals(obj);

  if (obj->texcoords==0 && flags&GLUE_TEXGEN_NORMALS) {
    obj->texcoords=glueTexgen_normals(obj->normals, obj->vc);
  }

  if (obj->colors==0 && flags&GLUE_VCOLOR_NORMALS) {
    obj->colors=glueVertexcolor_normals(obj->normals, obj->vc);
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

glue_static Object *glueLoadobject_flat(int *indices, Vertex *vertices, Vertex *texcoords, int *neighbours, Vertex *colors, Edge *edges, int *orig_indices,
                       int *vertex_links, int vc, int fc, int ec, int linkcnt, int orig_vc, int flgs) {
  Object *obj=malloc(sizeof(Object));
  int flags=flgs;//&objloadmask;
  int x;
  Vertex *vertices2=malloc(sizeof(Vertex)*fc*3);
  int *indices2=malloc(sizeof(int)*fc*3);
  Vertex *texcoords2=0;//=malloc(sizeof(Vertex)*fc*3);
  Vertex *colors2=0;//=malloc(sizeof(Vertex)*fc*3);
  
  if (texcoords) texcoords2=malloc(sizeof(Vertex)*fc*3);
  if (colors) colors2=malloc(sizeof(Vertex)*fc*3);

  
  for (x=0; x<fc; x++) {
    int i1, i2, i3;
    i1=indices[x*3+0];
    i2=indices[x*3+1];
    i3=indices[x*3+2];
    
    vertices2[x*3+0]=vertices[i1];
    vertices2[x*3+1]=vertices[i2];
    vertices2[x*3+2]=vertices[i3];
    
    if (texcoords) {
      texcoords2[x*3+0]=texcoords[i1];
      texcoords2[x*3+1]=texcoords[i2];
      texcoords2[x*3+2]=texcoords[i3];
    }
    
    if (colors) {
      colors2[(x*3+0)]=colors[i1];    
      colors2[(x*3+1)]=colors[i2];
      colors2[(x*3+2)]=colors[i3];
    }
      
    indices2[x*3+0]=x*3+0;
    indices2[x*3+1]=x*3+1;
    indices2[x*3+2]=x*3+2;
  }


  // set pointers etc

  obj->fc=fc;
  obj->vc=fc*3;
  obj->ec=0;
  obj->faces=malloc(obj->fc*sizeof(Face));
  obj->normals=malloc(obj->vc*sizeof(Vertex));
  obj->vertices=vertices2;
  obj->tangents=0;
  obj->texcoords=texcoords2;
  obj->edges=0;
  obj->center.x=0;
  obj->center.y=0;
  obj->center.z=0;
  obj->size=0;
  obj->shadowlist=glGenLists(1);
  obj->indices=indices2;
  obj->colors=colors2;

  obj->orig_indices=0;//orig_indices;
  obj->vertex_links=0;//vertex_links;
  obj->linkcnt=0;//linkcnt;


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

    i1=indices2[x*3+0];
    i2=indices2[x*3+1];
    i3=indices2[x*3+2];

    obj->faces[x].v[0]=i1;
    obj->faces[x].v[1]=i2;
    obj->faces[x].v[2]=i3;

    obj->faces[x].cen.x=(obj->vertices[i1].x+obj->vertices[i2].x+obj->vertices[i3].x)/3.0;
    obj->faces[x].cen.y=(obj->vertices[i1].y+obj->vertices[i2].y+obj->vertices[i3].y)/3.0;
    obj->faces[x].cen.z=(obj->vertices[i1].z+obj->vertices[i2].z+obj->vertices[i3].z)/3.0;
  }

  // calc normals
  calc_objnormals(obj);

  if (obj->texcoords==0 && flags&GLUE_TEXGEN_NORMALS) {
    obj->texcoords=glueTexgen_normals(obj->normals, obj->vc);
  }

  if (obj->colors==0 && flags&GLUE_VCOLOR_NORMALS) {
    obj->colors=glueVertexcolor_normals(obj->normals, obj->vc);
  }

	if (obj->tangents == 0 && flags&GLUE_GEN_TANGENTS) {
		calc_objtangents(obj);
	}

  // build displaylist for object
  obj->objnum=glGenLists(1);

//  buildlist(obj->objnum, obj->vertices, obj->normals, obj->texcoords, obj->colors, indices, obj->vc, obj->fc);
  buildlist2(obj->objnum, obj->vertices, obj->normals, obj->texcoords, obj->colors, indices2, obj->vc, obj->fc, obj->tangents);

  glueLoading();

  return obj;
}


glue_static void renderflags(int flgs) {
  int flags=flgs;//&rendermask;

  if (flags&GLUE_NO_CULL) {
    glDisable(GL_CULL_FACE);
  } else if (flags&GLUE_INV_CULL) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
  } else {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

  if (flags&GLUE_NO_DEPTH) {
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(0);
  } else if (flags&GLUE_CHECK_DEPTH) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(0);

 } else {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(1);
  }

  glDisable(GL_ALPHA_TEST);


  if (flags&GLUE_BLEND) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthFunc(GL_LEQUAL);
  } else if (flags&GLUE_BLEND_SUBTRACT) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glDepthFunc(GL_LEQUAL);
  } else if (flags&GLUE_BLEND_INV_DST) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
    glDepthFunc(GL_LEQUAL);
  } else if (flags&GLUE_BLEND_MULTIPLY) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    glDepthFunc(GL_LEQUAL);
  } else if (flags&GLUE_BLEND_ALPHA) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
  } else if (flags&GLUE_BLEND_ALPHAADD) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
  } else if (flags&GLUE_BLEND_INV_ALPHA) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_GREATER, 0);
  } else if (flags&GLUE_BLEND_INV_DST_ALPHA) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
    glDepthFunc(GL_LEQUAL);
  } else if (flags&GLUE_BLEND_DST_ALPHA) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    //glBlendFunc(GL_DST_ALPHA, GL_ZERO);
    glDepthFunc(GL_LEQUAL);
  } else if (flags&GLUE_BLEND_SATURATE) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    glDepthFunc(GL_LEQUAL);
/*
  } else if (flags&GLUE_BLEND_ALPHASATURATE) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    glDepthFunc(GL_LEQUAL); */
  } else if (flags&GLUE_BLEND_INVERT) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
    glDepthFunc(GL_LEQUAL);
  } else {
    glDisable(GL_BLEND);
  }
/*
  if (!(flags&GLUE_BLEND_ALPHA)) {
    glDisable(GL_ALPHA_TEST);
  }
*/
  glShadeModel(GL_SMOOTH);

  if (flags&GLUE_ERASE_STENCIL) {
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFFL);
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
  }

}

glue_static void glueDrawobject(Object *obj, int flags) {

  gluePush();
  renderflags(flags);
  glCallList(obj->objnum);
  glueFC+=obj->fc;
  glueVC+=obj->vc;
  gluePop();
}


glue_static void glueDrawobject_fast(Object *obj) {

  glCallList(obj->objnum);
  glueFC+=obj->fc;
  glueVC+=obj->vc;
}




glue_static void glueDrawlists2(Vertex *v, Vertex *n, Vertex *t, float *c, int *i, int vc, int fc, int flags) {
//  int x;

  if (fc>0) {

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    glue_disableallarrays();

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, v);

    if (n) {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, n);
    }

    if (t) {
      glClientActiveTexture(GL_TEXTURE3_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, 0, t);
      glClientActiveTexture(GL_TEXTURE2_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, 0, t);
      glClientActiveTexture(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, 0, t);
      glClientActiveTexture(GL_TEXTURE0_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, 0, t);
    }

    if (c) {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_FLOAT, 0, c);

    }

    renderflags(flags);

    glDrawElements(GL_TRIANGLES, fc*3, GL_UNSIGNED_INT, i);

    glPopClientAttrib();
    glPopAttrib();

    glueFC+=fc;
    glueVC+=vc;
  }
}










glue_static void glueDrawlists(Vertex *v, Vertex *t, int *i, int vc, int fc, int flags) {
  int x;
  Vertex *n=malloc(sizeof(Vertex)*vc);
  Vertex *c=0;

  if (fc>0) {

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);


    for (x=0; x<vc; x++) {
      n[x].x=0;
      n[x].y=0;
      n[x].z=0;
    }

    // calculate normals
    {
      for (x=0; x<fc; x++) {
        int i1, i2, i3;
        Vertex fnorm;

        i1=i[x*3+0];
        i2=i[x*3+1];
        i3=i[x*3+2];

        //if (i1>vc-1 || i1<0) glueErrorf("err1: i1=%i, x=%i", i1, x);
        //if (i2>vc-1 || i2<0) glueError("err1");
        //if (i3>vc-1 || i3<0) glueError("err1");
        calc_fnorm(&v[i1], &v[i2], &v[i3], &fnorm);

        n[i1].x+=fnorm.x; n[i1].y+=fnorm.y; n[i1].z+=fnorm.z;
        n[i2].x+=fnorm.x; n[i2].y+=fnorm.y; n[i2].z+=fnorm.z;
        n[i3].x+=fnorm.x; n[i3].y+=fnorm.y; n[i3].z+=fnorm.z;
      }

      for (x=0; x<vc; x++) normalize(&n[x]);
    }

    // calculate vertexcolors
    if (flags&GLUE_VCOLOR_NORMALS) {
      c=glueVertexcolor_normals(n, vc);
    }

    // draw
    renderflags(flags);

    {

      glue_disableallarrays();

      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, v);
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, 0, n);

      if (t!=0) {
        glClientActiveTexture(GL_TEXTURE3_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, 0, t);
        glClientActiveTexture(GL_TEXTURE2_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, 0, t);
        glClientActiveTexture(GL_TEXTURE1_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, 0, t);
        glClientActiveTexture(GL_TEXTURE0_ARB);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, 0, t);
      }

      if (c!=0) {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, 0, c);
      }

      glDrawElements(GL_TRIANGLES, fc*3, GL_UNSIGNED_INT, i);
    }



    glPopClientAttrib();
    glPopAttrib();

    glueFC+=fc;
    glueVC+=vc;
  }

  free(n);
  if (c!=0) free(c);
}









glue_static void glueShownormals(Object *obj, float len_vn, float len_fn, int flags) {
  int x;

  gluePush();

  renderflags(flags);

  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glLineWidth(1.0);
  glDisable(GL_TEXTURE_2D);

  glBegin(GL_LINES);

  if (len_vn>0) {

    glColor3f(1, 0 ,0);
    for (x=0; x<obj->vc; x++) {
      glVertex3f(obj->vertices[x].x, obj->vertices[x].y, obj->vertices[x].z);
      glVertex3f(obj->vertices[x].x+obj->normals[x].x*len_vn,
                 obj->vertices[x].y+obj->normals[x].y*len_vn,
                 obj->vertices[x].z+obj->normals[x].z*len_vn);
    }
  }


  if (len_fn>0) {

    glColor3f(0, 0, 1);
    for (x=0; x<obj->fc; x++) {
      Vertex *n=&obj->faces[x].norm;
      int v1=obj->faces[x].v[0], v2=obj->faces[x].v[1], v3=obj->faces[x].v[2];
      Vertex v=new_v(
        (obj->vertices[v1].x+obj->vertices[v2].x+obj->vertices[v3].x)/3.0,
        (obj->vertices[v1].y+obj->vertices[v2].y+obj->vertices[v3].y)/3.0,
        (obj->vertices[v1].z+obj->vertices[v2].z+obj->vertices[v3].z)/3.0);

      glVertex3f(v.x, v.y, v.z);
      glVertex3f(v.x+n->x*len_fn, v.y+n->y*len_fn, v.z+n->z*len_fn);
    }
  }

  glEnd();

  glueFC+=obj->fc;
  glueVC+=obj->vc;

  gluePop();
}

/**********************************************************************
  Objects
**********************************************************************/

glue_static Object *glueCone(float length, float radius, int slices, int flags) {
  Object *obj;
  Vertex *v=malloc(sizeof(Vertex)*(slices+1));
  int *i=malloc(sizeof(int)*slices*3);
  int x;

  v[0].x=0;
  v[0].y=length;
  v[0].z=0;

  for (x=0; x<slices; x++) {
    float k=(float)x/slices*2.0*pi;

    v[x+1].x=sin(k)*radius;
    v[x+1].y=0;
    v[x+1].z=cos(k)*radius;
  }

  for (x=0; x<slices; x++) {
    i[x*3+0]=0;
    i[x*3+1]=(x%slices)+1;
    i[x*3+2]=((x+1)%slices)+1;
  }

  obj=glueLoadobject(i, v, 0, 0, 0, 0, 0, 0, slices+1, slices, 0, 0, 0, flags);
  return obj;
}


glue_static Object *glueCube(float r, int flags) {
  Object *obj;
  Vertex *v=malloc(sizeof(Vertex)*24);
  Vertex *t=malloc(sizeof(Vertex)*24);
  int *i=malloc(sizeof(int)*12*3);

  // ylos y+
  v[0].x=-r; v[0].y=r; v[0].z=-r;
  v[1].x= r; v[1].y=r; v[1].z=-r;
  v[2].x= r; v[2].y=r; v[2].z= r;
  v[3].x=-r; v[3].y=r; v[3].z= r;

  t[0].x=0; t[0].y=0; t[0].z=0;
  t[1].x=1; t[1].y=0; t[1].z=0;
  t[2].x=1; t[2].y=1; t[2].z=0;
  t[3].x=0; t[3].y=1; t[3].z=0;

  i[0*3+0]=0; i[0*3+1]=2; i[0*3+2]=1;
  i[1*3+0]=0; i[1*3+1]=3; i[1*3+2]=2;

  // alas y-
  v[4].x=-r; v[4].y=-r; v[4].z=-r;
  v[5].x= r; v[5].y=-r; v[5].z=-r;
  v[6].x= r; v[6].y=-r; v[6].z= r;
  v[7].x=-r; v[7].y=-r; v[7].z= r;

  t[4].x=0; t[4].y=0; t[4].z=0;
  t[5].x=1; t[5].y=0; t[5].z=0;
  t[6].x=1; t[6].y=1; t[6].z=0;
  t[7].x=0; t[7].y=1; t[7].z=0;

  i[2*3+0]=4; i[2*3+1]=5; i[2*3+2]=6;
  i[3*3+0]=4; i[3*3+1]=6; i[3*3+2]=7;

  // eteen z-
  v[ 8].x=-r; v[ 8].y=-r; v[ 8].z=-r;
  v[ 9].x= r; v[ 9].y=-r; v[ 9].z=-r;
  v[10].x= r; v[10].y= r; v[10].z=-r;
  v[11].x=-r; v[11].y= r; v[11].z=-r;

  t[ 8].x=0; t[ 8].y=0; t[ 8].z=0;
  t[ 9].x=1; t[ 9].y=0; t[ 9].z=0;
  t[10].x=1; t[10].y=1; t[10].z=0;
  t[11].x=0; t[11].y=1; t[11].z=0;

  i[4*3+0]=8; i[4*3+1]=10; i[4*3+2]=9;
  i[5*3+0]=8; i[5*3+1]=11; i[5*3+2]=10;

  // eteen z+
  v[12].x=-r; v[12].y=-r; v[12].z=r;
  v[13].x= r; v[13].y=-r; v[13].z=r;
  v[14].x= r; v[14].y= r; v[14].z=r;
  v[15].x=-r; v[15].y= r; v[15].z=r;

  t[12].x=0; t[12].y=0; t[12].z=0;
  t[13].x=1; t[13].y=0; t[13].z=0;
  t[14].x=1; t[14].y=1; t[14].z=0;
  t[15].x=0; t[15].y=1; t[15].z=0;

  i[6*3+0]=12; i[6*3+1]=13; i[6*3+2]=14;
  i[7*3+0]=12; i[7*3+1]=14; i[7*3+2]=15;

  // vasen x+
  v[16].x=r; v[16].y=-1; v[16].z=-1;
  v[17].x=r; v[17].y=-1; v[17].z= 1;
  v[18].x=r; v[18].y= 1; v[18].z= 1;
  v[19].x=r; v[19].y= 1; v[19].z=-1;

  t[16].x=0; t[16].y=0; t[16].z=0;
  t[17].x=1; t[17].y=0; t[17].z=0;
  t[18].x=1; t[18].y=1; t[18].z=0;
  t[19].x=0; t[19].y=1; t[19].z=0;

  i[8*3+0]=16; i[8*3+1]=18; i[8*3+2]=17;
  i[9*3+0]=16; i[9*3+1]=19; i[9*3+2]=18;

  // oikea x-
  v[20].x=-r; v[20].y=-1; v[20].z=-1;
  v[21].x=-r; v[21].y=-1; v[21].z= 1;
  v[22].x=-r; v[22].y= 1; v[22].z= 1;
  v[23].x=-r; v[23].y= 1; v[23].z=-1;

  t[20].x=0; t[20].y=0; t[20].z=0;
  t[21].x=1; t[21].y=0; t[21].z=0;
  t[22].x=1; t[22].y=1; t[22].z=0;
  t[23].x=0; t[23].y=1; t[23].z=0;

  i[10*3+0]=20; i[10*3+1]=21; i[10*3+2]=22;
  i[11*3+0]=20; i[11*3+1]=22; i[11*3+2]=23;

  obj=glueLoadobject(i, v, t, 0, 0, 0, 0, 0, 24, 12, 0, 0, 0, flags);
  return obj;
}



glue_static Object *glueSphere(int xslices, int yslices, int flags) {
  int vc=(yslices+1)*(xslices+1);
  int fc=yslices*xslices*2;
  Vertex *v=malloc(sizeof(Vertex)*vc);
  Vertex *t=malloc(sizeof(Vertex)*vc);
  int *i=malloc(sizeof(int)*fc*3);
  Object *obj;
  int x, y;

  for (y=0; y<yslices+1; y++) for (x=0; x<xslices+1; x++) {
    float a1=2.0*x*pi/(float)(xslices)*1.0;
    float a2=1.0*y*pi/(float)(yslices);

    v[y*(xslices+1)+x].x=sin(a1)*sin(a2);
    v[y*(xslices+1)+x].y=cos(a2);
    v[y*(xslices+1)+x].z=cos(a1)*sin(a2);
    t[y*(xslices+1)+x].x=(float)x/(1.0*xslices);
    t[y*(xslices+1)+x].y=(float)y/(1.0*yslices);
    t[y*(xslices+1)+x].z=0;
  }

  for (y=0; y<yslices; y++) for (x=0; x<xslices; x++) {
    i[(y*xslices+x)*6+0]=(y+0)*(xslices+1)+x;
    i[(y*xslices+x)*6+1]=(y+1)*(xslices+1)+x;
    i[(y*xslices+x)*6+2]=(y+1)*(xslices+1)+x+1;
    i[(y*xslices+x)*6+3]=(y+0)*(xslices+1)+x;
    i[(y*xslices+x)*6+4]=(y+1)*(xslices+1)+x+1;
    i[(y*xslices+x)*6+5]=(y+0)*(xslices+1)+x+1;
  }

  obj=glueLoadobject(i, v, t, 0, 0, 0, 0, 0, vc, fc, 0, 0, 0, flags);
  return obj;
}


glue_static Object *glueSphere_wrap(int xslices, int yslices, int flags) {
  int vc=(yslices+1)*xslices;
  int fc=yslices*xslices*2;
  Vertex *v=malloc(sizeof(Vertex)*vc);
  Vertex *t=malloc(sizeof(Vertex)*vc);
  int *i=malloc(sizeof(int)*fc*3);
  Object *obj;
  int x, y;

  for (y=0; y<yslices+1; y++) for (x=0; x<xslices; x++) {
    float a1=2.0*x*pi/(float)(xslices);
    float a2=1.0*(y+0.0000001)*pi/(float)(yslices);

    v[y*xslices+x].x=sin(a1)*sin(a2);
    v[y*xslices+x].y=cos(a2);
    v[y*xslices+x].z=cos(a1)*sin(a2);
    t[y*xslices+x].x=(float)x/(1.0*xslices);
    t[y*xslices+x].y=(float)y/(1.0*yslices);
    t[y*xslices+x].z=0;
  }

  for (y=0; y<yslices; y++) for (x=0; x<xslices; x++) {
    i[(y*xslices+x)*6+0]=(y+0)*xslices+x;
    i[(y*xslices+x)*6+1]=(y+1)*xslices+x;
    i[(y*xslices+x)*6+2]=(y+1)*xslices+((x+1)%xslices);
    i[(y*xslices+x)*6+3]=(y+0)*xslices+x;
    i[(y*xslices+x)*6+4]=(y+1)*xslices+((x+1)%xslices);
    i[(y*xslices+x)*6+5]=(y+0)*xslices+((x+1)%xslices);
  }

  obj=glueLoadobject(i, v, t, 0, 0, 0, 0, 0, vc, fc, 0, 0, 0, flags);
  return obj;
}

glue_static Object *glueSemisphere(int xslices, int yslices, int flags) {
  int vc=(yslices+1)*(xslices+1);
  int fc=yslices*xslices*2;
  Vertex *v=malloc(sizeof(Vertex)*vc);
  Vertex *t=malloc(sizeof(Vertex)*vc);
  int *i=malloc(sizeof(int)*fc*3);
  Object *obj;
  int x, y;

  for (y=0; y<yslices+1; y++) for (x=0; x<xslices+1; x++) {
    float a1=2.0*x*pi/(float)(xslices);
    float a2=0.5*y*pi/(float)(yslices);

    v[y*(xslices+1)+x].x=(sin(a1)*sin(a2));
    v[y*(xslices+1)+x].y=(cos(a2));
    v[y*(xslices+1)+x].z=(cos(a1)*sin(a2));
    t[y*(xslices+1)+x].x=(float)x/(1.0*xslices);
    t[y*(xslices+1)+x].y=(float)y/(1.0*yslices);
    t[y*(xslices+1)+x].z=0;
  }

  for (y=0; y<yslices; y++) for (x=0; x<xslices; x++) {
    i[(y*xslices+x)*6+0]=(y+0)*(xslices+1)+x;
    i[(y*xslices+x)*6+1]=(y+1)*(xslices+1)+x;
    i[(y*xslices+x)*6+2]=(y+1)*(xslices+1)+x+1;
    i[(y*xslices+x)*6+3]=(y+0)*(xslices+1)+x;
    i[(y*xslices+x)*6+4]=(y+1)*(xslices+1)+x+1;
    i[(y*xslices+x)*6+5]=(y+0)*(xslices+1)+x+1;
  }

  obj=glueLoadobject(i, v, t, 0, 0, 0, 0, 0, vc, fc, 0, 0, 0, flags);
  return obj;

}


glue_static Object *gluePlane(int xsize, int ysize, Vertex scale, int flags) {
  Object *obj;
  Vertex *v=malloc(sizeof(Vertex)*(xsize+1)*(ysize+1));
  Vertex *t=malloc(sizeof(Vertex)*(xsize+1)*(ysize+1));
  int *i=malloc(sizeof(int)*xsize*ysize*2*3);
  int x, y;

  for (y=0; y<ysize+1; y++) for (x=0; x<xsize+1; x++) {
    v[y*(xsize+1)+x].x=(x-(xsize/2+0.5))*scale.x;
    v[y*(xsize+1)+x].y=0;
    v[y*(xsize+1)+x].z=(y-(ysize/2+0.5))*scale.z;
    t[y*(xsize+1)+x].x=x;
    t[y*(xsize+1)+x].y=y;
    t[y*(xsize+1)+x].z=0;
  }

  for (y=0; y<ysize; y++) for (x=0; x<xsize; x++) {
    i[y*xsize*6+x*6+0]=(y+0)*(xsize+1)+(x+0);
    i[y*xsize*6+x*6+1]=(y+1)*(xsize+1)+(x+0);
    i[y*xsize*6+x*6+2]=(y+1)*(xsize+1)+(x+1);
    i[y*xsize*6+x*6+3]=(y+0)*(xsize+1)+(x+0);
    i[y*xsize*6+x*6+4]=(y+1)*(xsize+1)+(x+1);
    i[y*xsize*6+x*6+5]=(y+0)*(xsize+1)+(x+1);
  }

  obj=glueLoadobject(i, v, t, 0, 0, 0, 0, 0, (xsize+1)*(ysize+1), xsize*ysize*2, 0, 0, 0, flags);
  return obj;
}


glue_static Object *glueCylinder(int xslices, int yslices, int flags) {
  int vc=(yslices+1)*(xslices+1);
  int fc=yslices*xslices*2;
  Vertex *v=malloc(sizeof(Vertex)*vc);
  Vertex *t=malloc(sizeof(Vertex)*vc);
  int *i=malloc(sizeof(int)*fc*3);
  Object *obj;
  int x, y;

  for (y=0; y<yslices+1; y++) for (x=0; x<xslices+1; x++) {
    float a1=2.0*x*pi/(float)(xslices);

    v[y*(xslices+1)+x].x=sin(a1);
    v[y*(xslices+1)+x].y=-y+(float)yslices/2.0;
    v[y*(xslices+1)+x].z=cos(a1);
    t[y*(xslices+1)+x].x=(float)x/(1.0*xslices);
    t[y*(xslices+1)+x].y=(float)y/(1.0*yslices);
    t[y*(xslices+1)+x].z=0;
  }

  for (y=0; y<yslices; y++) for (x=0; x<xslices; x++) {
    i[(y*xslices+x)*6+0]=(y+0)*(xslices+1)+x+0;
    i[(y*xslices+x)*6+1]=(y+1)*(xslices+1)+x+0;
    i[(y*xslices+x)*6+2]=(y+1)*(xslices+1)+x+1;
    i[(y*xslices+x)*6+5]=(y+1)*(xslices+1)+x+1;
    i[(y*xslices+x)*6+3]=(y+0)*(xslices+1)+x+1;
    i[(y*xslices+x)*6+4]=(y+0)*(xslices+1)+x+0;

  }

  obj=glueLoadobject(i, v, t, 0, 0, 0, 0, 0, vc, fc, 0, 0, 0, flags);
  return obj;
}

