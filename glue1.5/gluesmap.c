
#ifndef FORGET_INCLUDES
#include "gluesmap.h"
#include "glueext.h"
#include "gluetxtr.h"
#include "gluegen.h"
#endif




glue_static Shadowmap *glueShadowmap(int xres, int yres) {
  Shadowmap *sm=malloc(sizeof(Shadowmap));

	glGenTextures(1, &sm->texnum);
	sm->xres=xres;
	sm->yres=yres;

	glBindTexture(GL_TEXTURE_2D, sm->texnum);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, xres, yres, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  return sm;
}





glue_static void glueShadowmap_render(skene *efekti, Camera *light, float aspect, float time1, float time2, float *knobs, int *switches, Shadowmap *dest) {
  Texture dummytex;


  gluePrepareflags2(black);

  dummytex.xres=dest->xres;
  dummytex.yres=dest->yres;
  dummytex.scale=new_v(1, 1, 1);
  //glueDest=&dummytex;
  //glueCamera(light, 0);
  
  glueViewport();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glTranslatef(light->cen.x, light->cen.y, 0);
  gluPerspective(light->per, aspect, light->znear, light->zfar);
  //glRotatef(light->tilt, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();  

  gluLookAt(
    light->pos.x,
    light->pos.y,
    light->pos.z,
    light->tgt.x,
    light->tgt.y,
    light->tgt.z,
    light->yvec.x,
    light->yvec.y,
    light->yvec.z);


	glGetFloatv(GL_MODELVIEW_MATRIX, dest->lviewmat);
  glGetFloatv(GL_PROJECTION_MATRIX, dest->lprojmat);

  glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	glShadeModel(GL_FLAT);
	glColorMask(0, 0, 0, 0);
	
	efekti(time1, time2, knobs, switches, 1|2|8, 0);

  glActiveTexture(GL_TEXTURE0_ARB);

	glBindTexture(GL_TEXTURE_2D, dest->texnum);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dest->xres, dest->yres);
	
  glueDisabletexture();
  glDisable(GL_POLYGON_OFFSET_FILL);
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
	
}


glue_static void glueBindtexture_shadow(Shadowmap *sm, float ambient) {
	float biasMatrix[16]={
    0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f};	
	float textureMatrix[16];
	float textureMatrixR0[4];
	float textureMatrixR1[4];
	float textureMatrixR2[4];
	float textureMatrixR3[4];
	int i;

  glueDisabletexture();
  glActiveTexture(GL_TEXTURE0_ARB);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(biasMatrix);
	glMultMatrixf(sm->lprojmat);
	glMultMatrixf(sm->lviewmat);
	glGetFloatv(GL_MODELVIEW_MATRIX, textureMatrix);
	glPopMatrix();


	for (i=0; i<4; i++) {
		textureMatrixR0[i]=textureMatrix[i*4];
		textureMatrixR1[i]=textureMatrix[i*4+1];
		textureMatrixR2[i]=textureMatrix[i*4+2];
		textureMatrixR3[i]=textureMatrix[i*4+3];
	}
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sm->texnum);

	glEnable(GL_TEXTURE_GEN_S);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrixR0);

	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrixR1);

	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrixR2);

	glEnable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrixR3);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, ambient); 
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}


glue_static void glueShowshadowmap(Shadowmap *sm) {

  glueSet2d(0);  
    
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sm->texnum);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
 
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glBegin(GL_POLYGON);
  glTexCoord2f(0, 0); glVertex2f(-1, -1);
  glTexCoord2f(1, 0); glVertex2f( 1, -1);
  glTexCoord2f(1, 1); glVertex2f( 1,  1);
  glTexCoord2f(0, 1); glVertex2f(-1,  1);
  glEnd();

}


glue_static void glueBindtexture_shadowprojector(Shadowmap *sm, Texture *tex) {
	float biasMatrix[16]={
    0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f};	
	float textureMatrix[16];
	float textureMatrixR0[4];
	float textureMatrixR1[4];
	float textureMatrixR2[4];
	float textureMatrixR3[4];
	int i;

  glueDisabletexture();
  glActiveTexture(GL_TEXTURE0_ARB);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(biasMatrix);
	glMultMatrixf(sm->lprojmat);
	glMultMatrixf(sm->lviewmat);
	glGetFloatv(GL_MODELVIEW_MATRIX, textureMatrix);
	glPopMatrix();


	for (i=0; i<4; i++) {
		textureMatrixR0[i]=textureMatrix[i*4];
		textureMatrixR1[i]=textureMatrix[i*4+1];
		textureMatrixR2[i]=textureMatrix[i*4+2];
		textureMatrixR3[i]=textureMatrix[i*4+3];
	}
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sm->texnum);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_GEN_S);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrixR0);

	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrixR1);

	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrixR2);

	glEnable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrixR3);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FAIL_VALUE_ARB, ambient); 
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, white);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //////////


  do_tu(GL_TEXTURE1_ARB, tex, GLUE_CLAMP, GLUE_BLEND_MULTIPLY, GLUE_BLEND_MULTIPLY);

  glActiveTexture(GL_TEXTURE1_ARB);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  
	glEnable(GL_TEXTURE_GEN_S);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, textureMatrixR0);

	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, textureMatrixR1);

	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, textureMatrixR2);

	glEnable(GL_TEXTURE_GEN_Q);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, textureMatrixR3);

  glActiveTexture(GL_TEXTURE0_ARB);




}













