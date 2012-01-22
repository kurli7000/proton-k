
#ifndef FORGET_INCLUDES
#include <string.h>
#include "glueext.h"
#include "glue.h"
#ifdef I_LOVE_BILL
#include "gluewin.h"
#else
#include "gluelin.h"
#endif
#include "glue.h"
#endif
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
glue_extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;

glue_static int glueOpenconsole_multisample(int fullscreen, int zbits, char *title, int x, int y, WNDPROC winproc, int reallymultisample/* hack */);


PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;

PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2f = NULL;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3f = NULL;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4f = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTexture = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTexture = NULL;

#if GLUE_USE_3DTEXTURE==1
void APIENTRY (*glTexImage3D)(GLenum as, GLint, GLint ad, GLsizei ag, GLsizei ah, GLsizei aj, GLint ak, GLenum al, GLenum am, const GLvoid *);
void APIENTRY (*glCopyTexSubImage3D)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint);
#endif




int GLUE_CUBEMAP_OK=0;
int GLUE_PSPRITE_OK=0;





glue_static int checkext(char *extension) {
  char *extlist=(char *)glGetString(GL_EXTENSIONS);

  if (!extension || !extlist) return 0;

  while (*extlist) {
    unsigned int len=strcspn(extlist, " ");

    if (strlen(extension)==len && strncmp(extension, extlist, len)==0) return 1;
    extlist+=len+1;
  }

  return 0;
}


glue_static void (*glueGetProcAddress(char *proc))(void) {
  #ifdef I_LOVE_BILL
    return (void(*)(void))wglGetProcAddress(proc);
  #else
    return (void(*)(void))SDL_GL_GetProcAddress(proc);
  #endif
}




#ifndef SIMPLE_FULLSCREEN_DIALOG

glue_static int glueOpenconsole_multisample(int fullscreen, int zbits, char *title, int x, int y, WNDPROC winproc, int reallymultisample) {
  DEVMODE vmode;
  int width, height, bpp;
  BOOLEAN valid;
  UINT numformats;
  int pixelformat;
  float fAttributes[] = {0, 0};
  int iAttributes_multisample[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB, 24,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_DEPTH_BITS_ARB, zbits,
    WGL_STENCIL_BITS_ARB, 0, //GLUE_USE_SHADOWS==1?8:0,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
    WGL_SAMPLES_ARB, 4,
    0, 0};
  int iAttributes_nomultisample[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB, 24,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_DEPTH_BITS_ARB, zbits,
    WGL_STENCIL_BITS_ARB, 0, //GLUE_USE_SHADOWS==1?8:0,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    0, 0};

  if (hrc)	{
    wglDeleteContext(hrc);
    hrc = NULL;
  }

  if (hdc) {
    ReleaseDC(hwin, hdc);
    hdc = NULL;
  }

  if (hwin) {
    this_is_multisample_calling=1;
    DestroyWindow(hwin);
    this_is_multisample_calling=0;
    hwin = NULL;
  }


  if (!EnumDisplaySettings(NULL, glueMode, &vmode)) return 0;

  width=vmode.dmPelsWidth;
  height=vmode.dmPelsHeight;
  bpp=vmode.dmBitsPerPel;

  if (!fullscreen) { // window
    if (!(hwin_fsaa = _CreateWindow(&window_fsaa, glueName_fsaa, title, NULL, 0, 0, CS_VREDRAW | CS_HREDRAW,
                                    GetStockObject(4),
                                    LoadCursor(NULL, IDC_ARROW), LoadIcon(NULL, IDI_WINLOGO),
                                    WS_SYSMENU | WS_CAPTION | WS_VISIBLE, x, y, width+6, height+25,
                                    NULL, NULL, GetModuleHandle(0), winproc, NULL))) {
      return 0;
    }
  } else { // fullscreen
    if (!(hwin_fsaa = _CreateWindow(&window_fsaa, glueName_fsaa, title, NULL, 0, 0, 0,
                                    GetStockObject(4),
                                    LoadCursor(NULL, IDC_ARROW), LoadIcon(NULL, IDI_WINLOGO),
                                    WS_POPUP | WS_VISIBLE, 0, 0, width, height,
                                    NULL, NULL, GetModuleHandle(0), winproc, NULL))) {
      return 0;
    }
  }


  ShowWindow(hwin_fsaa, SW_SHOWNORMAL);
  UpdateWindow(hwin_fsaa);


  memset(&pfd_fsaa, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd_fsaa.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd_fsaa.nVersion = 1;
  pfd_fsaa.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
  pfd_fsaa.iPixelType = PFD_TYPE_RGBA;
  pfd_fsaa.cColorBits = bpp;
  pfd_fsaa.cDepthBits = zbits;
  pfd_fsaa.cStencilBits = 0; //GLUE_USE_SHADOWS==1?8:0;
  pfd_fsaa.iLayerType = PFD_MAIN_PLANE;

  if (!(hdc_fsaa = GetDC(hwin_fsaa))) {
    return 0;
  }

  valid = wglChoosePixelFormatARB(hdc_fsaa, reallymultisample ? iAttributes_multisample : iAttributes_nomultisample, fAttributes, 1, &pixelformat, &numformats);

  if (!valid || numformats<1) {
#ifdef HIDE_KEWLERS
    glueError(reallymultisample ?
    	"Did not find any suitable pixelformats with multisampling, try without FSAA" :
    	"Did not find any suitable pixelformats without multisampling, try with FSAA");
#else
    glueError(reallymultisample ?
    	"Did not find any suitable pixelformats for multisampling for your POS 3d-card" :
    	"Did not find any suitable pixelformats for your POS 3d-card"
    	);
#endif
  }

  if (!SetPixelFormat(hdc_fsaa, pixelformat, &pfd_fsaa)) {
    return 0;
  }

  if (!(hrc_fsaa = wglCreateContext(hdc_fsaa))) {
    return 0;
  }

  if (!wglMakeCurrent(hdc_fsaa, hrc_fsaa)) {
    return 0;
  }


  {
    PIXELFORMATDESCRIPTOR p;

    DescribePixelFormat(hdc_fsaa, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &p);
    glueSbits=p.cDepthBits;
  }

/*
  if (ramp_used==1) {
    int x;
    WORD ramppi[768];

    for (x=0; x<256; x++) {
      ramppi[0*256+x]=pow(x/255.0, glueGamma)*65535;
      ramppi[1*256+x]=pow(x/255.0, glueGamma)*65535;
      ramppi[2*256+x]=pow(x/255.0, glueGamma)*65535;
    }

    if (!SetDeviceGammaRamp(hdc_fsaa, (LPVOID)&ramppi)) {
      glueNotice("Gamma ramp shit failed, using defaults or whatever and things");
    }
  }
*/

  return 1;
}

#endif


glue_static void init_extensions(void) {


  // MULTITEXTURING

  int multitexturecount = 0;
  char erreur[100];
  int mt_target;

#ifdef HIDE_KEWLERS
  if (!checkext("GL_ARB_multitexture")) glueError("Your display adapter does not support the required extension GL_ARB_multitexture!");
  if (!checkext("GL_ARB_texture_env_combine")) glueError("Your display adapter does not support the required extension  GL_ARB_texture_env_combine!");
#else
  if (!checkext("GL_ARB_multitexture")) glueError("Your shitty 3d card doesnt support GL_ARB_multitexture!");
  if (!checkext("GL_ARB_texture_env_combine")) glueError("Your shitty 3d card doesnt support GL_ARB_texture_env_combine!");
#endif

  mt_target=4;

  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &multitexturecount);
  if (multitexturecount<mt_target) {
#ifdef HIDE_KEWLERS
    sprintf(erreur, "Your display adapter has only %i multitexturing units, and %i is needed", multitexturecount, mt_target);
#else
    sprintf(erreur, "Your shitty 3d card has only %i multitexturing units, and %i is required to view this demo!", multitexturecount, mt_target);
#endif
	  glueError(erreur);
  }

  glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FARBPROC) glueGetProcAddress("glMultiTexCoord2fARB");
  glActiveTexture = (PFNGLACTIVETEXTUREARBPROC) glueGetProcAddress("glActiveTextureARB");
  glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREARBPROC) glueGetProcAddress("glClientActiveTextureARB");
	glMultiTexCoord3f = (PFNGLMULTITEXCOORD3FARBPROC) glueGetProcAddress("glMultiTexCoord3fARB");
	glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FARBPROC) glueGetProcAddress("glMultiTexCoord4fARB");

  if (!glActiveTexture || !glMultiTexCoord2f || !glClientActiveTexture || !glMultiTexCoord3f || !glMultiTexCoord4f)
    glueError("wglGetProcAddress error on multitexture!");


  // CUBEMAPZ

#if GLUE_USE_CUBE==1
  if (checkext("GL_ARB_texture_cube_map")) {
    GLUE_CUBEMAP_OK=1;
  } else {
    glueError("Required extension GL_ARB_texture_cube_map not supported, see you in another computer.");
  }
#endif


#if GLUE_USE_ARBSHADOW==1

    if (!checkext("GL_ARB_depth_texture"))
      glueError("Your display adapter does not support GL_ARB_depth_texture");

    if (!checkext("GL_ARB_shadow"))
      glueError("Your display adapter does not support GL_ARB_shadow");

    //if (!checkext("GL_ARB_shadow_ambient"))
    //  glueError("Your display adapter does not support GL_ARB_shadow_ambient");

#endif

#if GLUE_USE_3DTEXTURE==1
  glTexImage3D = (void*)glueGetProcAddress("glTexImage3D");
  if (!glTexImage3D) glueError("wglGetProcAddress error on basic opengl 1.2 functions - what the fuck is wrong with your drivers? I need 3d textures!");
  glCopyTexSubImage3D = (void*)glueGetProcAddress("glCopyTexSubImage3D");
  if (!glCopyTexSubImage3D) glueError("wglGetProcAddress error on basic opengl 3.5 functions - what the fuck is wrong with your drivers? I need 3d textures!");
#endif

  // MULTISAMPLING

#ifndef SIMPLE_FULLSCREEN_DIALOG

  if (glueMultisample==1) {

    if (!checkext("GL_ARB_multisample")) {
      glueMultisample=0;
#ifdef HIDE_KEWLERS
      glueNotice("Your display adapter does not support GL_ARB_multisample, FSAA is disabled");
#else
      glueNotice("Your shitty 3d card doesnt support GL_ARB_multisample, turning it off");
#endif

    }

  }

  wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) glueGetProcAddress("wglChoosePixelFormatARB");
  if (/*glueMultisample==1 && */!wglChoosePixelFormatARB) {
    glueMultisample=0;
    glueNotice("Error fetching address for wglChoosePixelFormatARB");
  }

  //if (glueMultisample==1) {
    if (!glueOpenconsole_multisample(glueFullscreen, 16, glueMultisample ? glueName_fsaa : glueName, 0, 0, WindowProc, glueMultisample))
      glueError(glueMultisample ? "Multisample init failed!" : "window init failed");
  //}

#endif // SIMPLE_FULLSCREEN_DIALOG


}
