
// GLUE1.5, include this and you're set
#include "../glue1.5/glue_mainfile_header.h"

// show black instead of loading bar
int kkapturemode=0;

#include "rocket.c"


/***********************************
******** objects & textures ********
***********************************/
#include "gluemesh.c"
#include "lodder.h"

Texture *noisetex;
Texture *noisetex2;
Texture *stars;
Texture *noise3d;
Texture *logo[3];
Texture *flare1, *flare2, *flare3;
Texture *slit1;

Mesh *subi;
Object *pallo;
Mesh *satellite;

Framebuffer *singlefbo;	
Framebuffer *ssaofbo;
//Framebuffer *doffbo;
Framebuffer *dummyfbo;
Framebuffer *fxfbo, *workfbo, *fxfbo2;

#include "shaders.c"
#include "wtf.c"
#include "smap.c"

Shadowmapfbo *sm;


/***********************************
*************	precalc  *************
***********************************/


void precalc(void) {
  int x;

  // PUT STUFF HERE THAT IS SHOULD BE LOADED/INITIALIZED ONLY ONCE
  // pressing 'r' during demo would cause them to reload unnecessarily
  if (!glueLoadOK) {

    // extensions
    init_point_sprite();
    init_vbo();
    initvtf();

    // fbo
    singlefbo = glueFramebuffer_format(glueXres, glueYres, GL_ALPHA8, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
    ssaofbo = glueFramebuffer2_format(glueXres, glueYres, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LUMINANCE32F_ARB, GL_LUMINANCE, GL_FLOAT, 0);
    fxfbo = glueFramebuffer(glueXres, glueYres, 0);
    fxfbo2 = glueFramebuffer(glueXres, glueYres, 0);
    workfbo = glueFramebuffer(glueXres/2, glueYres/2, 0);
    //ssaofbo = glueFramebuffer2_format(glueXres, glueYres, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA32F_ARB, GL_LUMINANCE_ALPHA, GL_FLOAT, 0);
    //doffbo = glueFramebuffer(glueXres, glueYres, 0);
    dummyfbo = glueFramebuffer(64, 64, 0);
	    
	  sm = shadowmap(2048, 2048);

	  // meshes
    subi=glueLoadmesh("data/typhoon_by_maali.obj", 0);  
    meshlodder(subi);
    pallo=glueSphere(64, 64, 0);
	  satellite=glueLoadmesh("data/lonkero.obj", 0);
	  
	  // textures	  
	  noisetex = glueNoisetexture3(256, 256, 0); 
	  noisetex2 = glueNoisetexture(512, 256, 170, 255, 0); 
    noise3d=glueNoisetexture3d(64, 64, 64, GLUE_NO_MIPMAP);

    // textures
    stars=glueLoadtexture("data/stars2.jpg", 0, 0);
    logo[0]=glueLoadtexture("data/kewlers_c.jpg", "data/kewlers_a.jpg", 0);
    logo[1]=glueLoadtexture("data/proton-k_c.jpg", "data/proton-k_a.jpg", 0);
    logo[2]=glueLoadtexture("data/credits_c.jpg", "data/credits_a.jpg", 0);
    flare1=glueLoadtexture("data/flare1.jpg", 0, 0);
    flare2=glueLoadtexture("data/flare2d.jpg", 0, 0);
    flare3=glueLoadtexture("data/flare3b.jpg", 0, 0);
    slit1=glueLoadtexture("data/slit1.jpg", 0, 0);
  }

  // shaders
  shaders_load();


}



/***********************************
************* effects  *************
***********************************/


void noiseoverlay() {
  float xs=(float)glueXres/(float)noisetex2->xres;
  float ys=(float)glueYres/(float)noisetex2->yres;
  static int moi=143613;
  
  glue_seed=moi;

  gluePush();
  glueSet2d(GLUE_BLEND_MULTIPLY|GLUE_NO_DEPTH);
  glueBindtexture(noisetex2, 0);
  glueTexscale(0, new_v(xs, ys, 1));
  glueTextranslate(0, new_v(ranf(), ranf(), 0));
  moi=glue_seed;
  
  //glColor4f(br, br, br, 0);
  glColor4f(1.0, 1.0, 1.0, 1.0);

  glBegin(GL_POLYGON);
  glTexCoord2f(0, 0); glVertex2f(-1, -1);
  glTexCoord2f(1, 0); glVertex2f( 1, -1);
  glTexCoord2f(1, 1); glVertex2f( 1,  1);
  glTexCoord2f(0, 1); glVertex2f(-1,  1);
  glEnd();
  gluePop();
}


/////////////////////////////////////////////////////////////////////

#define NUMSTARS 2048

void render_flaret(Camera *cam, int depthtexnum, int part, float br, Vertex lpos, float t) {
  static float ranftab[1024];
  
  ONCE(
    for (int i=0; i<1024; i++) ranftab[i]=ranf();
  );
  // 3d flare
  if (part==0) {
    static Vertex posses[NUMSTARS];
  	static float sizes[NUMSTARS];
  	
  	posses[0]=lpos;
  	sizes[0]=1536.0;
  	for (int i=1; i<NUMSTARS; i++) {
  		float zz=1024.0-fmod(ranftab[(i+371)&1023]*4096.0+t*256, 2048.0);
      float r, a;
      r=ranftab[(i+127)&1023]+0.15;
      a=ranftab[(i+555)&1023]*pi*2.0;
  		posses[i]=new_v(
  		  fastsin(a)*r*1024.0, 
  		  fastcos(a)*r*1024.0, 
  		  zz);
  		sizes[i]=2.0-pow(fabs(zz)/1024.0, 4.0)*2.0;
  	}
    glueDrawparticles3(
      flare1, //Texture *tex, 
      posses, //Vertex *vert, 
      0, //Vertex *norm, 
      0, //float *rotobuf, 
      sizes, //float *sizebuf, 
      0, //float *colbuf, 
      NUMSTARS, //int vc, 
      cam, //Camera *camera, 
      1.0, //float size, 
      br, //float br, 
      PART_ROTATE, //int pflags, 
      GLUE_BLEND|GLUE_CHECK_DEPTH); //int flags);

  } else { 
  	
    // 2d horizontal stuff
    Vertex lpos2d = gluePoint2d(&lpos);
    Vertex size=new_v(4.1, 2.1, 0.0);
   
    glueSet2d(GLUE_BLEND|GLUE_NO_DEPTH|GLUE_NO_CULL);
     
    glUseProgramObjectARB(shader_partikle);
    glueGlsl_bindtex(shader_partikle, "spritetex", flare2->texnum, 0, GL_TEXTURE_2D, 0);
    glueGlsl_bindtex(shader_partikle, "depthtex", depthtexnum, 1, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP|GLUE_NEAREST);
    glUniform2fARB(glGetUniformLocationARB(shader_partikle, "pixel"), 1.0/glueXres, 1.0/glueYres);
  
    Vertex lookat=v_sub(&cam->tgt, &cam->pos);
    Vertex dir=v_sub(&lpos, &cam->pos);
    float dist=v_len(cam->pos, lpos);
  
    normalize(&lookat);
    normalize(&dir);
    glUniform1fARB(glGetUniformLocationARB(shader_partikle, "depth"), dist);
    glUniform2fARB(glGetUniformLocationARB(shader_partikle, "samplepos"), lpos2d.x, lpos2d.y);
 
    float brz=br;
    if (lpos2d.x<-1.0) brz+=lpos2d.x+1.0;
    if (lpos2d.x>1.0) brz-=lpos2d.x-1.0;
    if (lpos2d.y<-1.0) brz+=lpos2d.y+1.0;
    if (lpos2d.y>1.0) brz-=lpos2d.y-1.0;
      //if (dot(&lookat, &dir)>0 && lpos2d[i].x>-1.0 && lpos2d[i].x<1.0 && lpos2d[i].y>-1.0 && lpos2d[i].y<1.0) {
  
    glUniform1fARB(glGetUniformLocationARB(shader_partikle, "brite"), clamp(brz));
    //if (dot(&lookat, &dir)>0 && lpos2d[i].x>-1.0 && lpos2d[i].x<1.0 && lpos2d[i].y>-1.0 && lpos2d[i].y<1.0) {
    if (dot(&lookat, &dir)>0) {
    	Vertex s;
    	s.x=1.0/dist*size.x*1000.0;
     	s.y=1.0/dist*size.y*1000.0;
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex2f(lpos2d.x-s.x, lpos2d.y-s.y);
      glTexCoord2f(1.0, 0.0); glVertex2f(lpos2d.x+s.x, lpos2d.y-s.y);
      glTexCoord2f(1.0, 1.0); glVertex2f(lpos2d.x+s.x, lpos2d.y+s.y);
      glTexCoord2f(0.0, 1.0); glVertex2f(lpos2d.x-s.x, lpos2d.y+s.y);
      glEnd();

      glueGlsl_bindtex(shader_partikle, "spritetex", flare3->texnum, 0, GL_TEXTURE_2D, 0);

      s.x*=0.5;
      s.y*=0.5;
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex2f(-lpos2d.x-s.x, lpos2d.y*0.95-s.y);
      glTexCoord2f(1.0, 0.0); glVertex2f(-lpos2d.x+s.x, lpos2d.y*0.95-s.y);
      glTexCoord2f(1.0, 1.0); glVertex2f(-lpos2d.x+s.x, lpos2d.y*0.95+s.y);
      glTexCoord2f(0.0, 1.0); glVertex2f(-lpos2d.x-s.x, lpos2d.y*0.95+s.y);
      glEnd();
/*
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 0.0); glVertex2f(lpos2d.x*1.5-s.x, lpos2d.y*1.05-s.y);
      glTexCoord2f(1.0, 0.0); glVertex2f(lpos2d.x*1.5+s.x, lpos2d.y*1.05-s.y);
      glTexCoord2f(1.0, 1.0); glVertex2f(lpos2d.x*1.5+s.x, lpos2d.y*1.05+s.y);
      glTexCoord2f(0.0, 1.0); glVertex2f(lpos2d.x*1.5-s.x, lpos2d.y*1.05+s.y);
      glEnd();*/
    }
    glUseProgramObjectARB(0);
  } 
}



//////////////////////////////////////////////////////////////////////////


void draw_sub(float t, double row, int pier, Vertex lodpos, Vertex lodpos2) {	
	float loddiam=SYNC(lodder_diam, row);
	float lodpowr=SYNC(lodder_powr, row);
	float lodpush=SYNC(lodder_push, row);
  float zpos=SYNC(subzpos, row);
  // groups: whitestuff, body, propeller
  // prop.pos (x,y,z) -28,776, 220,368, 9.341


  // putket+logo  
  if (pier==1) glUniform2fARB(glGetUniformLocationARB(shader_shadow_mrt, "glow"), SYNC(pipes_br, row), 0.0); 
  glPushMatrix();
  glTranslatef(0.0, 0.0, zpos);
  //if (lodpowr>0.0) 
  render_lodder_sphere(0, lodpos, lodpos2, loddiam, lodpowr, lodpush, t);
  //else glueDrawobject(subi->groups[0], 0);

  // runko
  if (pier==1) glUniform2fARB(glGetUniformLocationARB(shader_shadow_mrt, "glow"), 0.0, 1.0); 
  //if (lodpowr>0.0) 
  render_lodder_sphere(1, lodpos, lodpos2, loddiam, lodpowr, lodpush, t);
  //else glueDrawobject(subi->groups[1], 0);
  
  // propelli
  glPushMatrix();
  glTranslatef(-28.776, -25.5, -220.368);
  glRotatef(t*360*4.0, 0.0, 0.0, 1.0);
  glTranslatef(-0.039, 34.6125, -0.509005);
  //if (lodpowr>0.0) 
  render_lodder_sphere(2, lodpos, lodpos2, loddiam, lodpowr, lodpush, t);
  //else glueDrawobject(subi->groups[2], 0);
  glPopMatrix();

  // propelli
  glPushMatrix();
  glTranslatef(28.776, -25.5, -220.368);
  glRotatef(t*360*4.0, 0.0, 0.0, 1.0);
  glTranslatef(-0.039, 34.6125, -0.509005);
  if (lodpowr>0.0) render_lodder_sphere(2, lodpos, lodpos2, loddiam, lodpowr, lodpush, t);
  else glueDrawobject(subi->groups[2], 0);
  glPopMatrix();
	
  glPopMatrix();
} 


void draw_satellite(float t) {
    
  if (t>=40.0 && t<43.5) {
  	glPushMatrix();
  	glTranslatef((41.5-t)*512.0, 40.0, 64.0);
  	glScalef(0.1, 0.1, 0.1);
    //glRotatef(t*90.0, 0.47, 0.83, -0.23);
  	glueDrawobject(satellite->groups[0], 0);
  	glPopMatrix();
  }	

  if (t>=44.0 && t<47.5) {
  	glPushMatrix();
  	glTranslatef((45.5-t)*512.0, 40.0, 64.0);
  	glScalef(0.1, 0.1, 0.1);
    //glRotatef(t*90.0, 0.47, 0.83, -0.23);
  	glueDrawobject(satellite->groups[0], 0);
  	glPopMatrix();
  }	
  

}


#define numballs 8

void render(float t, double row) {
	Camera cam;
	int x;
	Vertex lpos=new_v(SYNC(light_x, row), SYNC(light_y, row), SYNC(light_z, row));
  Vertex ballposses[numballs];

  float ballsep=SYNC(ball_sep, row);
  int activeball=SYNCi(ball_act, row);
  int activeball2=SYNCi(ball_act2, row);
  float ballmove=SYNC(ball_move, row);
  float lodbr=SYNC(lodder_br, row);
	float loddiam=SYNC(lodder_diam, row);
  
  for (x=0; x<numballs; x++) {
  	double r=row-x*ballsep;
  	ballposses[x]=new_v(SYNC(lodder_xpos, r), SYNC(lodder_ypos, r), SYNC(lodder_zpos, r));
  	ballposses[x].x+=sin(t*2.831+x*1.2679)*ballmove;
  	ballposses[x].y+=sin(t*3.877+x*2.2821)*ballmove;  	  	
  }

  if (activeball<0) activeball=0;
  if (activeball>=numballs) activeball=numballs-1;
  if (activeball2<0) activeball2=0;
  if (activeball2>=numballs) activeball2=numballs-1;

  // shadowmap
  beginrender_shadowmap(sm, lpos, new_v(0.0, 0.0, 0.0), SYNC(light_fov, row), 500.0, 4000.0);
  draw_sub(t, row, 0, ballposses[activeball], ballposses[activeball2]);
  draw_satellite(t);
  endrender_shadowmap();


  glueBeginrender(ssaofbo);    
  gluePrepareflags(black);  
  cam=rocketcamera();
  cam.znear=0.1;
  cam.zfar=10000.0;
  glueCamera(&cam, 0);

  // background
  glUseProgramObjectARB(shader_background_mrt);
  glueGlsl_bindtex(shader_background_mrt, "texture", stars->texnum, 0, GL_TEXTURE_2D, 0);
  glueGlsl_bindtex(shader_background_mrt, "slit1", slit1->texnum, 1, GL_TEXTURE_2D, 0);
  glUniform1fARB(glGetUniformLocationARB(shader_background_mrt, "slitpos"), SYNC(slitpos, row));
  glUniform1fARB(glGetUniformLocationARB(shader_background_mrt, "slitbr"), SYNC(slitbr, row));
  glPushMatrix();
  glScalef(8000.0, 8000.0, 8000.0);
  glueDrawobject(pallo, GLUE_NO_CULL); 
  glPopMatrix();
  glUseProgramObjectARB(0);

  //glueBindtexture(metal, GLUE_ENVI);
  glUseProgramObjectARB(shader_shadow_mrt);
  glUniform3fARB(glGetUniformLocationARB(shader_shadow_mrt, "lightpos"), lpos.x, lpos.y, lpos.z);
  glslbind_shadowmap(shader_shadow_mrt, "shadowmap", 0, sm);  
  glueGlsl_bindtex(shader_shadow_mrt, "noise3d", noise3d->texnum, 1, GL_TEXTURE_3D, GLUE_NO_MIPMAP);
  glUniform1fARB(glGetUniformLocationARB(shader_shadow_mrt, "smap_pixel"), 1.0/sm->map->xres);
  glUniform1fARB(glGetUniformLocationARB(shader_shadow_mrt, "shadowbr"), 1.0);
  draw_sub(t, row, 1, ballposses[activeball], ballposses[activeball2]);  
  // satellite
  glUniform1fARB(glGetUniformLocationARB(shader_shadow_mrt, "shadowbr"), 0.0);
  draw_satellite(t);
  glUseProgramObjectARB(0);
  glueDisabletexture();
  // flare part 1
  render_flaret(&cam, ssaofbo->texnum2, 0, 0.85*SYNC(flarebr, row), lpos, t);

  // spheres
  glUseProgramObjectARB(shader_plasma);
  glueGlsl_bindtex(shader_plasma, "noise3d", noise3d->texnum, 0, GL_TEXTURE_3D, GLUE_NO_MIPMAP);  
  for (x=0; x<numballs; x++) {
  	float lbr=lodbr;
  	//if (x==activeball || x==activeball2) lbr*=4;
    glUniform1fARB(glGetUniformLocationARB(shader_plasma, "time"), t);
    glUniform1fARB(glGetUniformLocationARB(shader_plasma, "brite"), lbr);
    glPushMatrix();
	  glTranslatef(ballposses[x].x, ballposses[x].y, ballposses[x].z);
    glScalef(loddiam, loddiam, loddiam);
    glueDrawobject(pallo, GLUE_NO_CULL|GLUE_BLEND|GLUE_CHECK_DEPTH);
    glPopMatrix();
  }
  glUseProgramObjectARB(0);
  glueEndrender();


  //glueOverlayfbo_depth(sm->map, 1.0, 0);
  //glueOverlayfbo2(ssaofbo, 0.001, 0);
  //glueOverlayfbo(ssaofbo, 1.0, 0);

  // SSAO
  glueBeginrender(singlefbo);
  gluePrepareflags(black); 
  ssaofilter(ssaofbo, noisetex, ranf(), glueXres/40, 3.0);
  glueEndrender(); 
  
  glueBeginrender(fxfbo);  
  ssaocombinefilter(ssaofbo, singlefbo);
  //glueOverlay(noisetex2, 1.0, GLUE_BLEND_MULTIPLY|GLUE_NO_DEPTH);
  noiseoverlay();
  glueEndrender();

  glueBeginrender(fxfbo2);  
  // GLOW
  float fxbr=SYNC(effect_br, row);
  float glowbr=SYNC(glow_br, row);
  blurfilter(fxfbo, workfbo, 1.0, glueXres/SYNC(glow_rad, row), SYNC(glow_minus, row));
  glueBackgroundcolor(glowbr, glowbr, glowbr, 1.0, GLUE_BLEND_MULTIPLY|GLUE_NO_DEPTH);
  glueOverlayfbo(fxfbo, fxbr, GLUE_BLEND);

  // flare part 2
  render_flaret(&cam, ssaofbo->texnum2, 1, 0.666*SYNC(flarebr, row), lpos, t);
  glueEndrender();
    
  dstrukt(fxfbo2, SYNC(d_blockpow, row), SYNC(d_sinpow, row), SYNC(d_linepow, row), SYNC(d_colorsep, row), t);

}


void design(void) {
	double row=bass_get_row(stream);
	float t=row/64.0;

/*****************************************/
/*****		"design" begins hereh  		*****/
/*****************************************/


  render(t, row); 

  int lnum=SYNCi(logonum, row);
  float lbr=SYNC(logobr, row);
  if (lbr>0.0) {
  	alphafade(logo[lnum], lbr, 16.0);
  	//glueOverlay(logo[lnum], lbr, GLUE_BLEND);
  }
  if (t>=66.0) glueDone++;

 // glueDebugf_tiny("%i", BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
}



/***********************************
***********	boring shit ************
***********************************/

// for kkapture, "digital silence"
void blankscreen(float a) {
	ONCE (gluePrepareflags(black););
}

static void initStuff(void) {
	if (kkapturemode) glueLoadingbarproc=&blankscreen;
	glueInit();
	precalc();
	#if GLUE_DEVEL_MODE==0
		gluePrepareflags(black);
		glueSwap();
		Sleep(2000);
	#endif
	glueLoadOK++;

	gluePrepareflags(black);

	#if GLUE_DEVEL_MODE==1
	{
		FILE *f;

		f=fopen("glueload.h", "w");
		fprintf(f, "#define GLUELOADSTEPS %i\n", glueLoadsteps);
		fclose(f);
	}
	#endif

#if GLUE_DEVEL_MODE==0
  // render dummy frame for ATI 
  glueBeginrender(dummyfbo);
  render(0, 0);
  glueEndrender();
  glFinish();
#endif
  
  start_bassrocket();
}

static void runStuff(void) {

  glueFC=0;
  glueVC=0;

	design();
  glFinish();
	BASS_Update(0); /* decrease the chance of missing vsync */
#ifndef SYNC_PLAYER
	if (sync_update(rocket, (int)floor(bass_get_row(stream)))) sync_connect(rocket, "localhost", SYNC_DEFAULT_PORT);
#endif

	if (glueShowinformation) glueInfo(glueGettime());
	if (glueShowdebug) glueShowtinydebug();
	glueSwap();
	if (GLUE_DEVEL_MODE==1) gluePrinterrors();

}

static void closeStuff(void) {
  close_bassrocket();
	glueClose();
}

//GLUECREATEMAINROUTINE(initStuff(), runStuff(), closeStuff())


int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, char *cl, int dm) {
  initStuff();
  while(!glueDone) {
      if (PeekMessage(&MainMsg, NULL, 0, 0, PM_REMOVE)) {
      if (MainMsg.message == WM_QUIT) {
        glueDone++;
      }
      TranslateMessage(&MainMsg);
      DispatchMessage(&MainMsg);
      rocket_messages();
    } else {
      runStuff();
     }
  }
  closeStuff();
  return 0;
}


