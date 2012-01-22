
#include <time.h>

static void glueSwap() {
  glFinish();
  SwapBuffers(glueHdc);
}

/*
static float biisi_starttime=0;

static float getmsec() {
  return clock()/(float)CLOCKS_PER_SEC*1000.0;
}

//#ifndef biisi_bpm
//#define biisi_bpm 128
//#endif

static float glueGettime() {

  return (getmsec()-biisi_starttime)/60000.0*biisi_bpm/15.0;

}
*/

/**********************************************************************
   Message stuff
*********************************************************************/



static void glueError(char errormsg[]) {
  int retval;

  //ssStop();
  //ssClose();
  retval=MessageBox(0, errormsg, glueName, MB_ICONSTOP|MB_OK);
  glueClose();
  exit(0);
}

static void glueNotice(char errormsg[]) {
  int retval;

  retval=MessageBox(0, errormsg, glueName, MB_ICONWARNING|MB_OK);
}


static void glueErrorf(char *fmt, ...) {
  static char buf[8000];
  int retval;
  va_list ap;
  
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  retval=MessageBox(0, buf, glueName, MB_ICONSTOP|MB_OK);
  va_end(ap);
  glueClosewindow();
  exit(0);
}

static void glueNoticef(char *fmt, ...) {
  static char buf[8000];
  int retval;
  va_list ap;
  
  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  retval=MessageBox(0, buf, glueName, MB_ICONWARNING|MB_OK);
  va_end(ap);
}




/**********************************************************************
   Window/init stuff
*********************************************************************/



static int glueOpenwindow() {
  PIXELFORMATDESCRIPTOR pfd;
  DEVMODE mode;
  int pixelformat;


  if (glueFullscreen) {
    mode.dmSize=sizeof(mode);
    mode.dmPelsWidth=glueXres;
    mode.dmPelsHeight=glueYres;
    mode.dmBitsPerPel=32;
    mode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
    if (ChangeDisplaySettings(&mode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) return 0;
  }

  pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion=1;
  pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER|PFD_GENERIC_ACCELERATED;
  pfd.iPixelType=PFD_TYPE_RGBA;
  pfd.cColorBits=24;
  pfd.cAccumBits=0; 
  pfd.cDepthBits=16;
  pfd.cStencilBits=0;
  pfd.iLayerType=PFD_MAIN_PLANE;

  glueWindow=CreateWindowEx(WS_EX_TOPMOST, "STATIC", glueName, WS_VISIBLE|WS_POPUP,
    0, 0, glueXres, glueYres, NULL, NULL, 0, NULL);

  glueHdc=GetDC(glueWindow);
  pixelformat=ChoosePixelFormat(glueHdc, &pfd);
  SetPixelFormat(glueHdc, pixelformat, &pfd); 
  wglMakeCurrent(glueHdc, wglCreateContext(glueHdc)); 

  if (glueFullscreen) ShowCursor(0), SetCursor(0);

  return 1;
}


static void glueClosewindow(void) {
  if (glueFullscreen) {
    ChangeDisplaySettings(NULL, 0);
    ShowCursor(1);
  }
}




static void glueInit(int xres, int yres, float aspect) {
  int rval;
  char msg[100];

  glueFullscreen=1;
  glueXres=xres;//glueDefaultxres;
  glueYres=yres;//glueDefaultyres;
  glueAspect=aspect;
  glueWidescreen=glueAspect/glue_widescreen;

  sprintf(msg, "Fullscreen %ix%i?", xres, yres);

  if (MessageBox(NULL, msg, glueName, MB_YESNO|MB_ICONQUESTION)==IDNO) glueFullscreen=0;
  if (!glueOpenwindow()) glueErrorf("Init failed!");

  eftexture_xsize=texreso(glueXres);
  eftexture_ysize=texreso(glueYres*glueWidescreen);


  init_extensions();
  glueInitfbo();
  glueGlsl_init();

  testit();

  glueDummytex=glueColortexture(1, 1, 255, 255, 255, 255, GLUE_NO_MIPMAP);
}


static void glueClose() {
  glueClosewindow();
}



