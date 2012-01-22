
#ifndef GLUEWIN_H
#define GLUEWIN_H

#ifndef GL_ALL_CLIENT_ATTRIB_BITS
#define GL_ALL_CLIENT_ATTRIB_BITS GL_CLIENT_ALL_ATTRIB_BITS
#endif


#ifndef FORGET_INCLUDES

#include <windows.h>
#include <mmsystem.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include "glue.h"
#include "glue3dx.h"

#endif //FORGET_INCLUDES


typedef struct {
  UINT msg;
  WPARAM wparam; 
  LPARAM lparam;
} Messages;

glue_extern Messages glueMessages;


// VARIABLES
glue_extern int glueDone;
glue_extern int this_is_multisample_calling;
glue_extern int kewlmode, kewlmodecnt;
glue_extern WORD old_gammaramp[768];
glue_extern int ramp_used;
//glue_extern int glueKeyPressed;

glue_extern HDC hdc;              //handle to device context.
glue_extern HGLRC hrc;            //handle to opengl rendering context.
glue_extern HWND hwin;            //handle to the window.
glue_extern WNDCLASS window;      //window class.
glue_extern PIXELFORMATDESCRIPTOR pfd;

glue_extern HDC hdc_fsaa;              //handle to device context.
glue_extern HGLRC hrc_fsaa;            //handle to opengl rendering context.
glue_extern HWND hwin_fsaa;            //handle to the window.
glue_extern WNDCLASS window_fsaa;      //window class.
glue_extern PIXELFORMATDESCRIPTOR pfd_fsaa;

//glue_extern HWND camelHWND=NULL;

glue_extern HINSTANCE hinstance;
glue_extern MSG MainMsg;
glue_extern int glueMode;


typedef __int64 glueInt64_t;


HWND _CreateWindow(WNDCLASS *window, char *lpszClassName, char *lpszWinTitle,
                   char *lpszMenuName, int cbClsExtra, int cbWndExtra, UINT style,
                   HBRUSH hbrBackground, HCURSOR hCursor, HICON hIcon, DWORD dwStyle,
                   int winx, int winy, int width, int height, HWND hWndParent,
                   HMENU hMenu, HINSTANCE hinstance, WNDPROC WindowProc, LPVOID lpParam);
HWND glueOpenconsole(int fullscreen, int zbits, char *title, int x, int y, WNDPROC winproc);
void glueCloseconsole(void);
glue_static void glueSwap(void);
glue_static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
#ifndef INTRO
BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif //INTRO

glue_static void glueError(char errormsg[]);
glue_static void glueErrorf(char *fmt, ...);
glue_static void glueNotice(char errormsg[]);
glue_static void glueNoticef(char *fmt, ...);
glue_static void glueInit(void);
glue_static void glueWritebmp(void);
//void glueWritebmp_focalblur(void);
glue_static float glueGettime(void);
glue_static void glueMod_playfile(char *filename, float bpm, int loop);
glue_static void glueMod_close(void);
glue_static void glueClose();

#define GLUECREATEMAINROUTINE(init, run, close) int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, char *cl, int dm) {\
  init;\
  while(!glueDone) {\
    if (PeekMessage(&MainMsg, NULL, 0, 0, PM_REMOVE)) {\
      if (MainMsg.message == WM_QUIT) {\
        glueDone++;\
      }\
      TranslateMessage(&MainMsg);\
      DispatchMessage(&MainMsg);\
    } else {\
      run;\
     }\
  }\
  close;\
  return 0;\
}

#endif // GLUEWIN_H



