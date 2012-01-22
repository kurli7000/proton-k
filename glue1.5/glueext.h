
#ifndef GLUEEXT_H
#define GLUEEXT_H

#ifndef FORGET_INCLUDES
#include <GL\gl.h>
#include "glue.h"
#endif


/* MULTISAMPLING */
#define GL_MULTISAMPLE_ARB 0x809D
#define WGL_DRAW_TO_WINDOW_ARB              0x2001
#define WGL_SUPPORT_OPENGL_ARB              0x2010
#define WGL_ACCELERATION_ARB                0x2003
#define WGL_DOUBLE_BUFFER_ARB               0x2011
#define WGL_COLOR_BITS_ARB                  0x2014
#define WGL_ALPHA_BITS_ARB                  0x201B
#define WGL_DEPTH_BITS_ARB                  0x2022
#define WGL_STENCIL_BITS_ARB                0x2023
#define WGL_FULL_ACCELERATION_ARB           0x2027
#define WGL_SAMPLE_BUFFERS_ARB	            0x2041
#define WGL_SAMPLES_ARB		                  0x2042



/*
  //  Accepted by the <piAttributes> parameter of
  //  wglGetPixelFormatAttribivEXT, wglGetPixelFormatAttribfvEXT, and
  //  the <piAttribIList> and <pfAttribIList> of wglChoosePixelFormatEXT:
*/


/*
	SAMPLE_ALPHA_TO_COVERAGE_ARB	     0x809E
	SAMPLE_ALPHA_TO_ONE_ARB		     0x809F
	SAMPLE_COVERAGE_ARB		     0x80A0
	MULTISAMPLE_BIT_ARB		     0x20000000
	SAMPLE_BUFFERS_ARB		     0x80A8
	SAMPLES_ARB			     0x80A9
	SAMPLE_COVERAGE_VALUE_ARB	     0x80AA
	SAMPLE_COVERAGE_INVERT_ARB	     0x80AB
*/

/*

  MULTITEXTURING

*/




// multitexturing functions
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat u);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat u, GLfloat v);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
glue_extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2f;
glue_extern PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3f;
glue_extern PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4f;
glue_extern PFNGLACTIVETEXTUREARBPROC glActiveTexture;
glue_extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture;


// prkl
#if GLUE_USE_3DTEXTURE==1
extern void APIENTRY (*glTexImage3D)(GLenum as, GLint, GLint ad, GLsizei ag, GLsizei ah, GLsizei aj, GLint ak, GLenum al, GLenum am, const GLvoid *);
extern void APIENTRY (*glCopyTexSubImage3D)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint);
#define GL_TEXTURE_3D                           0x806F
#endif

#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE7_ARB                   0x84C7

#define GL_ACTIVE_TEXTURE_ARB             0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB      0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB          0x84E2

#define GL_COMBINE_ARB                    0x8570
#define GL_COMBINE_RGB_ARB                0x8571
#define GL_COMBINE_ALPHA_ARB              0x8572
#define GL_SOURCE0_RGB_ARB                0x8580
#define GL_SOURCE1_RGB_ARB                0x8581
#define GL_SOURCE2_RGB_ARB                0x8582
#define GL_SOURCE0_ALPHA_ARB              0x8588
#define GL_SOURCE1_ALPHA_ARB              0x8589
#define GL_SOURCE2_ALPHA_ARB              0x858A
#define GL_OPERAND0_RGB_ARB               0x8590
#define GL_OPERAND1_RGB_ARB               0x8591
#define GL_OPERAND2_RGB_ARB               0x8592
#define GL_OPERAND0_ALPHA_ARB             0x8598
#define GL_OPERAND1_ALPHA_ARB             0x8599
#define GL_OPERAND2_ALPHA_ARB             0x859A
#define GL_RGB_SCALE_ARB                  0x8573
#define GL_ADD_SIGNED_ARB                 0x8574
#define GL_INTERPOLATE_ARB                0x8575
#define GL_SUBTRACT_ARB                   0x84E7
#define GL_CONSTANT_ARB                   0x8576
#define GL_PRIMARY_COLOR_ARB              0x8577
#define GL_PREVIOUS_ARB                   0x8578

#define GL_TEXTURE_WRAP_R                 0x8072

/*

  VSYNC

* /
typedef int (APIENTRY * PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
*/


/* CUBEMAPPING */

#define GL_NORMAL_MAP_ARB 0x8511
#define GL_REFLECTION_MAP_ARB 0x8512
#define GL_TEXTURE_CUBE_MAP_ARB 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB 0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x851A
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB 0x851C


/* POINT SPRITE SHIT */

#define GL_POINT_SPRITE_ARB 0x8861
#define GL_COORD_REPLACE_ARB 0x8862

#if GLUE_USE_ARBSHADOW==1

/* DEPTH TEXTURING AND SHADOW MAPS SHIT (shamelessly ripped from Lev Povalahev's extgl.h */

#define GL_INTENSITY                                            0x8049
#define GL_INTENSITY4                                           0x804A
#define GL_INTENSITY8                                           0x804B
#define GL_INTENSITY12                                          0x804C
#define GL_INTENSITY16                                          0x804D
#define GL_GEQUAL                                               0x0206

#define GL_ALPHA_TEST                                           0x0BC0
#define GL_ALPHA_TEST_FUNC                                      0x0BC1
#define GL_ALPHA_TEST_REF                                       0x0BC2

#define GL_TEXTURE_GEN_S                                        0x0C60
#define GL_TEXTURE_GEN_T                                        0x0C61
#define GL_TEXTURE_GEN_R                                        0x0C62
#define GL_TEXTURE_GEN_Q                                        0x0C63
#define GL_EYE_LINEAR                                           0x2400

#define GL_S                                                    0x2000
#define GL_T                                                    0x2001
#define GL_R                                                    0x2002
#define GL_Q                                                    0x2003

#define GL_TEXTURE_GEN_MODE                                     0x2500

#define GL_DEPTH_TEXTURE_MODE                                   0x884B
#define GL_COMPARE_R_TO_TEXTURE                                 0x884E

#define GL_TEXTURE_COMPARE_MODE_ARB                             0x884C
#define GL_TEXTURE_COMPARE_FUNC_ARB                             0x884D
#define GL_COMPARE_R_TO_TEXTURE_ARB                             0x884E

#define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB                       0x80BF

#define GL_DEPTH_COMPONENT16_ARB                                0x81A5
#define GL_DEPTH_COMPONENT24_ARB                                0x81A6
#define GL_DEPTH_COMPONENT32_ARB                                0x81A7
#define GL_TEXTURE_DEPTH_SIZE_ARB                               0x884A
#define GL_DEPTH_TEXTURE_MODE_ARB                               0x884B

#endif // ARB_shadow



/*

  GAMMA RAMP

*/
/*
BOOL wglGetGammaTableParametersI3D(HDC hDC, int iAttribute, int *piValue);
BOOL wglSetGammaTableParametersI3D(HDC hDC, int iAttribute, const int *piValue)
BOOL wglGetGammaTableI3D(HDC hDC, int iEntries, USHORT *puRed, USHORT *puGreen, USHORT *puBlue);
* /

//typedef BOOL (WINAPI * PFNWGLGETGAMMATABLEI3DPROC) (HDC hDC, int iEntries, USHORT* puRed, USHORT *puGreen, USHORT *puBlue);
//typedef BOOL (WINAPI * PFNWGLGETGAMMATABLEPARAMETERSI3DPROC) (HDC hDC, int iAttribute, int* piValue);
typedef BOOL (WINAPI * PFNWGLSETGAMMATABLEI3DPROC) (HDC hDC, int iEntries, const USHORT* puRed, const USHORT *puGreen, const USHORT *puBlue);
//typedef BOOL (WINAPI * PFNWGLSETGAMMATABLEPARAMETERSI3DPROC) (HDC hDC, int iAttribute, const int* piValue);

PFNWGLSETGAMMATABLEI3DPROC wglSetGammaTableI3D = NULL;
* /



// Accepted by the <iAttribute> parameter of wglGetGammaTableParametersI3D:

#define WGL_GAMMA_TABLE_SIZE_I3D 0x204E
#define WGL_GAMMA_EXCLUDE_DESKTOP_I3D 0x204F

// Accepted by the <iAttribute> parameter of wglSetGammaTableParametersI3D:

#define WGL_GAMMA_EXCLUDE_DESKTOP_I3D 0x204F

*/


// VARIABLES
glue_extern int GLUE_CUBEMAP_OK;
glue_extern int GLUE_PSPRITE_OK;


// PROTOTYPES

glue_static int checkext(char *extension);
glue_static void (*glueGetProcAddress(char *proc))(void);
glue_static void init_extensions(void);

#endif // GLUEEXT_H
