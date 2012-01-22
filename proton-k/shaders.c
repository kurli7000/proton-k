// shaders.c

int shader_blur;
int shader_ssao;
int shader_ssaocombine;
int shader_shadow_mrt;
int shader_background_mrt;
int shader_partikle;
int shader_plasma;
int shader_alphafade;
int shader_dstrukt;


static void shaders_load() {
	
	shader_blur = glueGlsl_loadfile("blur", "data/blur.vs", "data/blur.fs", "");
  shader_ssao = glueGlsl_loadfile("ssao", "data/ssao.vs", "data/ssao.fs", "");
  shader_ssaocombine = glueGlsl_loadfile("ssaocombine", "data/ssaocombine.vs", "data/ssaocombine2.fs", "");
  shader_shadow_mrt = glueGlsl_loadfile("shadowmrt", "data/shadow_mrt.vs", "data/shadow_mrt.fs", "");
  shader_background_mrt = glueGlsl_loadfile("background", "data/background_mrt.vs", "data/background_mrt.fs", "");
  shader_partikle = glueGlsl_loadfile("partikle", "data/partikle.vs", "data/partikle.fs", "");
  shader_plasma = glueGlsl_loadfile("plasma", "data/plasma.vs", "data/plasma.fs", "");  
  shader_alphafade = glueGlsl_loadfile("alphafade", "data/alphafade.vs", "data/alphafade.fs", "");
  shader_dstrukt = glueGlsl_loadfile("dstrukt", "data/dstrukt.vs", "data/dstrukt.fs", "");
}




static void dstrukt(Framebuffer *src, float blockpow, float sinpow, float linepow, float colorsep, float t) {

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  gluePrepareflags(0);
  glClear(GL_COLOR_BUFFER_BIT);
  glueSet2d(GLUE_NO_DEPTH);

  glUseProgramObjectARB(shader_dstrukt);
  glueGlsl_bindtex(shader_dstrukt, "t_color", src->texnum, 0, GL_TEXTURE_2D, GLUE_NO_MIPMAP|GLUE_CLAMP);
  glueGlsl_bindtex(shader_dstrukt, "t_noise", noisetex->texnum, 1, GL_TEXTURE_2D, GLUE_NO_MIPMAP|GLUE_NEAREST);
  glUniform1fARB(glGetUniformLocationARB(shader_dstrukt, "blockpow"), blockpow);
  glUniform1fARB(glGetUniformLocationARB(shader_dstrukt, "sinpow"), sinpow);
  glUniform1fARB(glGetUniformLocationARB(shader_dstrukt, "linepow"), linepow);
  glUniform1fARB(glGetUniformLocationARB(shader_dstrukt, "colorsep"), colorsep);
  glUniform1fARB(glGetUniformLocationARB(shader_dstrukt, "time"), t);  

  glue2dquad();
  glUseProgramObjectARB(0);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

}


void blurfilter(Framebuffer *src, Framebuffer *work, float pixel, int rad, float minus) {
  int x;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glueBeginrender(work);
  gluePrepareflags(0);
  glClear(GL_COLOR_BUFFER_BIT);
  glueSet2d(GLUE_NO_DEPTH);
  glueDisabletexture();

  glUseProgramObjectARB(shader_blur);
  glueGlsl_bindtex(shader_blur, "t_color", src->texnum, 0, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP);
	glUniform2fARB(glGetUniformLocationARB(shader_blur, "direction"), pixel/src->xres, 0.0);
	glUniform1iARB(glGetUniformLocationARB(shader_blur, "radius"), rad);
	glUniform1fARB(glGetUniformLocationARB(shader_blur, "minus"), minus);
  glue2dquad();
  glUseProgramObjectARB(0);
  glueEndrender();

  glClear(GL_COLOR_BUFFER_BIT);
  glueSet2d(GLUE_NO_DEPTH);

  glUseProgramObjectARB(shader_blur);
  glueGlsl_bindtex(shader_blur, "t_color", work->texnum, 0, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP);
	glUniform2fARB(glGetUniformLocationARB(shader_blur, "direction"), 0, pixel/work->yres);
  float radmultiplier=(float)work->yres/(float)src->yres;
//  glueNoticef("blur multiplier shit %5.2f", radmultiplier);
	glUniform1iARB(glGetUniformLocationARB(shader_blur, "radius"), rad*radmultiplier);
  glue2dquad();
  glUseProgramObjectARB(0);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}



static void ssaofilter(Framebuffer *mrtfbo, Texture *noise, float t, float ballsize, float darken) {

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  gluePrepareflags(0);
  glClear(GL_COLOR_BUFFER_BIT);
  glueSet2d(GLUE_NO_DEPTH);

  glUseProgramObjectARB(shader_ssao);
    glUniform1fARB(glGetUniformLocationARB(shader_ssao, "t"), t);
    glUniform1fARB(glGetUniformLocationARB(shader_ssao, "ballsize"), ballsize);
    glUniform1fARB(glGetUniformLocationARB(shader_ssao, "darken"), darken);
    glUniform2fARB(glGetUniformLocationARB(shader_ssao, "pixel"), 1.0/(float)mrtfbo->xres, 1.0/(float)mrtfbo->yres);
	  //glueGlsl_bindtex(shader_ssao, "colortex", mrtfbo->texnum, 0, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP|GLUE_NEAREST);
	  glueGlsl_bindtex(shader_ssao, "depthtex", mrtfbo->texnum2, 0, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP|GLUE_NEAREST);
	  glueGlsl_bindtex(shader_ssao, "noise", noise->texnum, 1, GL_TEXTURE_2D, GLUE_NO_MIPMAP|GLUE_NEAREST);
	  glue2dquad();
  glUseProgramObjectARB(0);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


static void ssaocombinefilter(Framebuffer *src, Framebuffer *ssao) {

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  gluePrepareflags(0);
  glClear(GL_COLOR_BUFFER_BIT);
  glueSet2d(GLUE_NO_DEPTH);

  glUseProgramObjectARB(shader_ssaocombine);
  glUniform2fARB(glGetUniformLocationARB(shader_ssaocombine, "pixel"), 1.0/(float)ssao->xres, 1.0/(float)ssao->yres);
	glueGlsl_bindtex(shader_ssaocombine, "colortex", src->texnum, 0, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP);
	glueGlsl_bindtex(shader_ssaocombine, "ssaotex", ssao->texnum, 1, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP);
	glueGlsl_bindtex(shader_ssaocombine, "depthtex", src->texnum2, 2, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP|GLUE_NEAREST);
//glueGlsl_bindtex(shader_ssaocombine, "depthtex", mrtfbo->texnum2, 0, GL_TEXTURE_2D, GLUE_CLAMP|GLUE_NO_MIPMAP|GLUE_NEAREST);
	glue2dquad();
  glUseProgramObjectARB(0);



  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

}


static void alphafade(Texture *src, float limit, float ramp) {

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glueSet2d(GLUE_NO_DEPTH|GLUE_BLEND_ALPHAADD);

  glUseProgramObjectARB(shader_alphafade);
  glueGlsl_bindtex(shader_alphafade, "t_color", src->texnum, 0, GL_TEXTURE_2D, GLUE_NO_MIPMAP);
  glUniform1fARB(glGetUniformLocationARB(shader_alphafade, "limit"), limit);
  glUniform1fARB(glGetUniformLocationARB(shader_alphafade, "ramp"), ramp);
  glue2dquad();
  glUseProgramObjectARB(0);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

}




