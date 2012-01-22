
#include "glext.h"

glue_static int glueGlsl_loadstring(const char *name, const char *vs_in, const char *fs_in, char *defines);
glue_static int glueGlsl_loadfile(const char *name, char *vs, char *fs, char *defines);
glue_static void glueGlsl_bindtex(int shader, char *pname, int texnum, int unit, int type, int flags);
glue_static void glueGlsl_init();
glue_static void glueGlsl_stop();

// glsl.h

glue_extern 	PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB        ;
glue_extern 	PFNGLGETHANDLEARBPROC            glGetHandleARB           ;
glue_extern   PFNGLDETACHOBJECTARBPROC         glDetachObjectARB        ;
glue_extern 	PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB  ;
glue_extern 	PFNGLSHADERSOURCEARBPROC         glShaderSourceARB        ;
glue_extern 	PFNGLCOMPILESHADERARBPROC        glCompileShaderARB       ;
glue_extern 	PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB ;
glue_extern 	PFNGLATTACHOBJECTARBPROC         glAttachObjectARB        ;
glue_extern 	PFNGLLINKPROGRAMARBPROC          glLinkProgramARB         ;
glue_extern 	PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB    ;
glue_extern 	PFNGLVALIDATEPROGRAMARBPROC      glValidateProgramARB     ;
glue_extern 	PFNGLUNIFORM1FARBPROC            glUniform1fARB           ;
glue_extern 	PFNGLUNIFORM2FARBPROC            glUniform2fARB           ;
glue_extern 	PFNGLUNIFORM3FARBPROC            glUniform3fARB           ;
glue_extern 	PFNGLUNIFORM4FARBPROC            glUniform4fARB           ;
glue_extern 	PFNGLUNIFORM1IARBPROC            glUniform1iARB           ;
glue_extern 	PFNGLUNIFORM2IARBPROC            glUniform2iARB           ;
glue_extern 	PFNGLUNIFORM3IARBPROC            glUniform3iARB           ;
glue_extern 	PFNGLUNIFORM4IARBPROC            glUniform4iARB           ;
glue_extern 	PFNGLUNIFORM1FVARBPROC           glUniform1fvARB          ;
glue_extern 	PFNGLUNIFORM2FVARBPROC           glUniform2fvARB          ;
glue_extern 	PFNGLUNIFORM3FVARBPROC           glUniform3fvARB          ;
glue_extern 	PFNGLUNIFORM4FVARBPROC           glUniform4fvARB          ;
glue_extern 	PFNGLUNIFORM1IVARBPROC           glUniform1ivARB          ;
glue_extern 	PFNGLUNIFORM2IVARBPROC           glUniform2ivARB          ;
glue_extern 	PFNGLUNIFORM3IVARBPROC           glUniform3ivARB          ;
glue_extern 	PFNGLUNIFORM4IVARBPROC           glUniform4ivARB          ;
glue_extern 	PFNGLUNIFORMMATRIX2FVARBPROC     glUniformMatrix2fvARB    ;
glue_extern 	PFNGLUNIFORMMATRIX3FVARBPROC     glUniformMatrix3fvARB    ;
glue_extern 	PFNGLUNIFORMMATRIX4FVARBPROC     glUniformMatrix4fvARB    ;
glue_extern 	PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB;
glue_extern 	PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
glue_extern 	PFNGLGETINFOLOGARBPROC           glGetInfoLogARB          ;
glue_extern 	PFNGLGETATTACHEDOBJECTSARBPROC   glGetAttachedObjectsARB  ;
glue_extern 	PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB  ;
glue_extern 	PFNGLGETACTIVEUNIFORMARBPROC     glGetActiveUniformARB    ;
glue_extern 	PFNGLGETUNIFORMFVARBPROC         glGetUniformfvARB        ;
glue_extern 	PFNGLGETUNIFORMIVARBPROC         glGetUniformivARB        ;
glue_extern 	PFNGLGETSHADERSOURCEARBPROC      glGetShaderSourceARB     ;
glue_extern 	PFNGLGETPROGRAMIVARBPROC         glGetProgramivARB        ;
/*
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
*/
