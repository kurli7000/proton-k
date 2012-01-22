// glsl.c

//#define GL_GLEXT_PROTOTYPES
#ifndef FORGET_INCLUDES
#include "glue.h"
#ifndef INTRO
#include "gluewin.h"
#else
#include "gluewin_intro.h"
#endif
#include "gluetxtr.h"
#include "glueext.h"
#include "glueglsl.h"
#endif

//int glsl_readLen = 0;
int glsl_maxInstructions;


glue_static int glsl_compile(int shaderType, const char *name, const char *code) {
	//printf("compiling %i chars:\n%s", len, code);
	int result=0;
	int shader = glCreateShaderObjectARB(shaderType);

 	glShaderSourceARB(shader, 1, &code, NULL);
  glCompileShaderARB(shader);

 	glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &result);  
  if (!result) {
	  static char errmsg[1024];
	  glGetInfoLogARB(shader, sizeof(errmsg), NULL, errmsg);
#if GLUE_DEVEL_MODE==1
  	glueNoticef("Error compiling shader %s [%s]: %s", name, shaderType==GL_VERTEX_SHADER_ARB?"VS":"FS", errmsg);
#else
 		glueErrorf("Error compiling shader %s [%s]: %s", name, shaderType==GL_VERTEX_SHADER_ARB?"VS":"FS", errmsg);
#endif
  }
  
	return shader;
}


glue_static int glueGlsl_loadstring(const char *name, const char *vs_in, const char *fs_in, char *defines) {
	int prog=glCreateProgramObjectARB();
	int result=0;
	char vs[10000], fs[10000];
	
	sprintf(vs, "%s\n%s", defines, vs_in);
	sprintf(fs, "%s\n%s", defines, fs_in);
	
	if (vs != "") glAttachObjectARB(prog, glsl_compile(GL_VERTEX_SHADER_ARB, name, vs));
	if (fs != "") glAttachObjectARB(prog, glsl_compile(GL_FRAGMENT_SHADER_ARB, name, fs));
 
  glLinkProgramARB(prog);
  glGetObjectParameterivARB(prog, GL_OBJECT_LINK_STATUS_ARB, &result);
  if (!result) {   	
  	static char errmsg[1024];
 		glGetInfoLogARB(prog, sizeof(errmsg), NULL, errmsg);
#if GLUE_DEVEL_MODE==1
  	glueNoticef("Error linking shader %s\n%s", name, errmsg);
#else
  	glueErrorf("Error linking shader %s\n%s", name, errmsg);
#endif
    return 0;
	}
  
	return prog;
}

#ifndef INTRO

glue_static int glueGlsl_loadfile(const char *name, char *vs, char *fs, char *defines) {
	char *vcode="", *fcode="";
	int proggis=0;
	char *temp1=0, *temp2=0;
	int vsize, fsize;

  // glueError will crash out of this loop in final build
  while (!proggis) {
	  if (vs != "") { 
	  	vcode = glueLoadfile(vs);
	  	vsize = glueFilelength(vs);
	    temp1 = malloc(vsize+1);
	    memcpy(temp1, vcode, vsize);
      temp1[vsize]=0;
    }
   	if (fs != "") { 
   		fcode = glueLoadfile(fs); 
   		fsize = glueFilelength(fs);
      temp2 = malloc(fsize+1);
      memcpy(temp2, fcode, fsize);
      temp2[fsize]=0;
    }
    proggis=glueGlsl_loadstring(name, temp1, temp2, defines);
  }

  free(vs);
  free(fs);
  free(temp1);
  free(temp2);  
  
	return proggis;
}

#endif

glue_static void glueGlsl_bindtex(int shader, char *pname, int texnum, int unit, int type, int flags) {
	int texUnit;

  glUniform1iARB(glGetUniformLocationARB(shader, pname), unit);
  
	switch (unit) {
		case 0: texUnit = GL_TEXTURE0_ARB; break;
		case 1: texUnit = GL_TEXTURE1_ARB; break;
		case 2: texUnit = GL_TEXTURE2_ARB; break;
		case 3: texUnit = GL_TEXTURE3_ARB; break;
		case 4: texUnit = GL_TEXTURE4_ARB; break;
		case 5: texUnit = GL_TEXTURE5_ARB; break;
		case 6: texUnit = GL_TEXTURE6_ARB; break;
		case 7: texUnit = GL_TEXTURE7_ARB; break;	// reserved for tangent
		default: texUnit = GL_TEXTURE0_ARB;
	}

  glClientActiveTexture(texUnit);
	glActiveTexture(texUnit);
	glEnable(type);
	glBindTexture(type, texnum);

  texflags(flags);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}


glue_static void glueGlsl_stop() {
	glUseProgramObjectARB(0);
	glActiveTexture(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE5_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE6_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE7_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE8_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0_ARB);
}


/**
 * Registers GLSL extensions
 */
 
 
	PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB            = NULL;
	PFNGLGETHANDLEARBPROC            glGetHandleARB               = NULL;
	PFNGLDETACHOBJECTARBPROC         glDetachObjectARB            = NULL;
	PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB      = NULL;
	PFNGLSHADERSOURCEARBPROC         glShaderSourceARB            = NULL;
	PFNGLCOMPILESHADERARBPROC        glCompileShaderARB           = NULL;
	PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB     = NULL;
	PFNGLATTACHOBJECTARBPROC         glAttachObjectARB            = NULL;
	PFNGLLINKPROGRAMARBPROC          glLinkProgramARB             = NULL;
	PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB        = NULL;
	PFNGLVALIDATEPROGRAMARBPROC      glValidateProgramARB         = NULL;
	PFNGLUNIFORM1FARBPROC            glUniform1fARB               = NULL;
	PFNGLUNIFORM2FARBPROC            glUniform2fARB               = NULL;
	PFNGLUNIFORM3FARBPROC            glUniform3fARB               = NULL;
	PFNGLUNIFORM4FARBPROC            glUniform4fARB               = NULL;
	PFNGLUNIFORM1IARBPROC            glUniform1iARB               = NULL;
	PFNGLUNIFORM2IARBPROC            glUniform2iARB               = NULL;
	PFNGLUNIFORM3IARBPROC            glUniform3iARB               = NULL;
	PFNGLUNIFORM4IARBPROC            glUniform4iARB               = NULL;
	PFNGLUNIFORM1FVARBPROC           glUniform1fvARB              = NULL;
	PFNGLUNIFORM2FVARBPROC           glUniform2fvARB              = NULL;
	PFNGLUNIFORM3FVARBPROC           glUniform3fvARB              = NULL;
	PFNGLUNIFORM4FVARBPROC           glUniform4fvARB              = NULL;
	PFNGLUNIFORM1IVARBPROC           glUniform1ivARB              = NULL;
	PFNGLUNIFORM2IVARBPROC           glUniform2ivARB              = NULL;
	PFNGLUNIFORM3IVARBPROC           glUniform3ivARB              = NULL;
	PFNGLUNIFORM4IVARBPROC           glUniform4ivARB              = NULL;
	PFNGLUNIFORMMATRIX2FVARBPROC     glUniformMatrix2fvARB        = NULL;
	PFNGLUNIFORMMATRIX3FVARBPROC     glUniformMatrix3fvARB        = NULL;
	PFNGLUNIFORMMATRIX4FVARBPROC     glUniformMatrix4fvARB        = NULL;
	PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB    = NULL;
	PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB    = NULL;
	PFNGLGETINFOLOGARBPROC           glGetInfoLogARB              = NULL;
	PFNGLGETATTACHEDOBJECTSARBPROC   glGetAttachedObjectsARB      = NULL;
	PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB      = NULL;
	PFNGLGETACTIVEUNIFORMARBPROC     glGetActiveUniformARB        = NULL;
	PFNGLGETUNIFORMFVARBPROC         glGetUniformfvARB            = NULL;
	PFNGLGETUNIFORMIVARBPROC         glGetUniformivARB            = NULL;
	PFNGLGETSHADERSOURCEARBPROC      glGetShaderSourceARB         = NULL;
	PFNGLGETPROGRAMIVARBPROC         glGetProgramivARB            = NULL;
	
glue_static void glueGlsl_init() {
  //glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FARBPROC) glueGetProcAddress("glMultiTexCoord2fARB");

	glDeleteObjectARB        = (PFNGLDELETEOBJECTARBPROC)         wglGetProcAddress("glDeleteObjectARB");
	glGetHandleARB           = (PFNGLGETHANDLEARBPROC)            wglGetProcAddress("glGetHandleARB");
	glDetachObjectARB        = (PFNGLDETACHOBJECTARBPROC)         wglGetProcAddress("glDetachObjectARB");
	glCreateShaderObjectARB  = (PFNGLCREATESHADEROBJECTARBPROC)   wglGetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB        = (PFNGLSHADERSOURCEARBPROC)         wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB       = (PFNGLCOMPILESHADERARBPROC)        wglGetProcAddress("glCompileShaderARB");
	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)  wglGetProcAddress("glCreateProgramObjectARB");
	glAttachObjectARB        = (PFNGLATTACHOBJECTARBPROC)         wglGetProcAddress("glAttachObjectARB");
	glLinkProgramARB         = (PFNGLLINKPROGRAMARBPROC)          wglGetProcAddress("glLinkProgramARB");
	glUseProgramObjectARB    = (PFNGLUSEPROGRAMOBJECTARBPROC)     wglGetProcAddress("glUseProgramObjectARB");
	glValidateProgramARB     = (PFNGLVALIDATEPROGRAMARBPROC)      wglGetProcAddress("glValidateProgramARB");
	glUniform1fARB           = (PFNGLUNIFORM1FARBPROC)            wglGetProcAddress("glUniform1fARB");
	glUniform2fARB           = (PFNGLUNIFORM2FARBPROC)            wglGetProcAddress("glUniform2fARB");
	glUniform3fARB           = (PFNGLUNIFORM3FARBPROC)            wglGetProcAddress("glUniform3fARB");
	glUniform4fARB           = (PFNGLUNIFORM4FARBPROC)            wglGetProcAddress("glUniform4fARB");
	glUniform1iARB           = (PFNGLUNIFORM1IARBPROC)            wglGetProcAddress("glUniform1iARB");
	glUniform2iARB           = (PFNGLUNIFORM2IARBPROC)            wglGetProcAddress("glUniform2iARB");
	glUniform3iARB           = (PFNGLUNIFORM3IARBPROC)            wglGetProcAddress("glUniform3iARB");
	glUniform4iARB           = (PFNGLUNIFORM4IARBPROC)            wglGetProcAddress("glUniform4iARB");
	glUniform1fvARB          = (PFNGLUNIFORM1FVARBPROC)           wglGetProcAddress("glUniform1fvARB");
	glUniform2fvARB          = (PFNGLUNIFORM2FVARBPROC)           wglGetProcAddress("glUniform2fvARB");
	glUniform3fvARB          = (PFNGLUNIFORM3FVARBPROC)           wglGetProcAddress("glUniform3fvARB");
	glUniform4fvARB          = (PFNGLUNIFORM4FVARBPROC)           wglGetProcAddress("glUniform4fvARB");
	glUniform1ivARB          = (PFNGLUNIFORM1IVARBPROC)           wglGetProcAddress("glUniform1ivARB");
	glUniform2ivARB          = (PFNGLUNIFORM2IVARBPROC)           wglGetProcAddress("glUniform2ivARB");
	glUniform3ivARB          = (PFNGLUNIFORM3IVARBPROC)           wglGetProcAddress("glUniform3ivARB");
	glUniform4ivARB          = (PFNGLUNIFORM4IVARBPROC)           wglGetProcAddress("glUniform4ivARB");
	glUniformMatrix2fvARB    = (PFNGLUNIFORMMATRIX2FVARBPROC)     wglGetProcAddress("glUniformMatrix2fvARB");
	glUniformMatrix3fvARB    = (PFNGLUNIFORMMATRIX3FVARBPROC)     wglGetProcAddress("glUniformMatrix3fvARB");
	glUniformMatrix4fvARB    = (PFNGLUNIFORMMATRIX4FVARBPROC)     wglGetProcAddress("glUniformMatrix4fvARB");
	glGetObjectParameterfvARB= (PFNGLGETOBJECTPARAMETERFVARBPROC) wglGetProcAddress("glGetObjectParameterfvARB");
	glGetObjectParameterivARB= (PFNGLGETOBJECTPARAMETERIVARBPROC) wglGetProcAddress("glGetObjectParameterivARB");
	glGetInfoLogARB          = (PFNGLGETINFOLOGARBPROC)           wglGetProcAddress("glGetInfoLogARB");
	glGetAttachedObjectsARB  = (PFNGLGETATTACHEDOBJECTSARBPROC)   wglGetProcAddress("glGetAttachedObjectsARB");
	glGetUniformLocationARB  = (PFNGLGETUNIFORMLOCATIONARBPROC)   wglGetProcAddress("glGetUniformLocationARB");
	glGetActiveUniformARB    = (PFNGLGETACTIVEUNIFORMARBPROC)     wglGetProcAddress("glGetActiveUniformARB");
	glGetUniformfvARB        = (PFNGLGETUNIFORMFVARBPROC)         wglGetProcAddress("glGetUniformfvARB" );
	glGetUniformivARB        = (PFNGLGETUNIFORMIVARBPROC)         wglGetProcAddress("glGetUniformivARB");
	glGetShaderSourceARB     = (PFNGLGETSHADERSOURCEARBPROC)      wglGetProcAddress("glGetShaderSourceARB");
	glGetProgramivARB        = (PFNGLGETPROGRAMIVARBPROC)         wglGetProcAddress("glGetProgramivARB");
/*
	glGenFramebuffersEXT	= (PFNGLGENFRAMEBUFFERSEXTPROC)		wglGetProcAddress("glGenFramebuffersEXT");
	glGenRenderbuffersEXT	= (PFNGLGENRENDERBUFFERSEXTPROC)	wglGetProcAddress("glGenRenderbuffersEXT");
	glBindRenderbufferEXT	= (PFNGLBINDRENDERBUFFEREXTPROC)	wglGetProcAddress("glBindRenderbufferEXT");
	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glBindFramebufferEXT	= (PFNGLBINDFRAMEBUFFEREXTPROC)		wglGetProcAddress("glBindFramebufferEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
*/

	if (!glLinkProgramARB) glueError("Your hardware doesn't support GLSL, gtfo");

	glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, &glsl_maxInstructions);
}

