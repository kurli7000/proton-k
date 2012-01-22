
#ifndef FORGET_INCLUDES
#include "gluewin.h"
#include "glue.h"
#include "gluetxtr.h"
#ifndef NO_FMOD
	#ifdef USE_FMODEX
		#include "fmodex.h"
	#else
		#include "fmodcrap.h"
	#endif
#endif
#include "glueext.h"
#include "gluegen.h"
#include "gluetinyfont.h"
#include "glue3dx.h"
#include "gluefbo.h"
#include "glueglsl.h"


#endif //FORGET_INCLUDES


#ifndef SIMPLE_FULLSCREEN_DIALOG
#include "dialog/resource.h"
#endif


int glueDone = 0;
int this_is_multisample_calling=0;
int kewlmode=0, kewlmodecnt=0;
WORD old_gammaramp[768];
int ramp_used=0;
//int glueKeyPressed=-1;


Messages glueMessages;



HDC hdc;              //handle to device context.
HGLRC hrc;            //handle to opengl rendering context.
HWND hwin;            //handle to the window.
WNDCLASS window;      //window class.
PIXELFORMATDESCRIPTOR pfd;

HDC hdc_fsaa;              //handle to device context.
HGLRC hrc_fsaa;            //handle to opengl rendering context.
HWND hwin_fsaa;            //handle to the window.
WNDCLASS window_fsaa;      //window class.
PIXELFORMATDESCRIPTOR pfd_fsaa;

HWND camelHWND=NULL;

HINSTANCE hinstance = 0;
MSG MainMsg;
int glueMode=-1;


#ifndef SIMPLE_FULLSCREEN_DIALOG

HWND _CreateWindow(WNDCLASS *window, char *lpszClassName, char *lpszWinTitle,
                   char *lpszMenuName, int cbClsExtra, int cbWndExtra, UINT style,
                   HBRUSH hbrBackground, HCURSOR hCursor, HICON hIcon, DWORD dwStyle,
                   int winx, int winy, int width, int height, HWND hWndParent,
                   HMENU hMenu, HINSTANCE hinstance, WNDPROC WindowProc, LPVOID lpParam) {
  HWND hndl;

  window->cbClsExtra = cbClsExtra;
  window->cbWndExtra = cbWndExtra;
  window->hbrBackground = hbrBackground;
  window->hCursor = hCursor;
  window->hIcon = hIcon;
  window->hInstance = hinstance;
  window->lpfnWndProc = WindowProc;
  window->lpszClassName = lpszClassName;
  window->lpszMenuName = lpszMenuName;
  window->style = style;

  if (!RegisterClass(window)) {
#ifdef HIDE_KEWLERS
    glueNotice("RegisterClass failed");
#else
    glueNotice("RegisterClass failed, you suck");
#endif
    return NULL;
  }

  if (!(hndl = CreateWindow(lpszClassName, lpszWinTitle,dwStyle, winx, winy,
                           width, height, hWndParent, hMenu, hinstance, lpParam))) {
#ifdef HIDE_KEWLERS
    glueNotice("RegisterClass failed");
#else
    glueNotice("RegisterClass failed, you suck");
#endif
    return NULL;
  }

  return hndl;
}



HWND glueOpenconsole(int fullscreen, int zbits, char *title, int x, int y, WNDPROC winproc) {
  DEVMODE vmode;
  int width, height, bpp;
  GLuint pixelformat;

  if (!EnumDisplaySettings(NULL, glueMode, &vmode)) return NULL;

  width=vmode.dmPelsWidth;
  height=vmode.dmPelsHeight;
  bpp=vmode.dmBitsPerPel;
  glueXres=width;
  glueYres=height;

  if (!fullscreen) { // window
    if (!(hwin = _CreateWindow(&window, "GLUE", title, NULL, 0, 0, CS_VREDRAW | CS_HREDRAW,
                               GetStockObject(4),
                               LoadCursor(NULL, IDC_ARROW), LoadIcon(NULL, IDI_WINLOGO),
                               WS_SYSMENU | WS_CAPTION | WS_VISIBLE, x, y, width+6, height+25,
                               NULL, NULL, GetModuleHandle(0), winproc, NULL))) {
      return NULL;
    }
  } else { // fullscreen
    if (!(hwin = _CreateWindow(&window, "GLUE", title, NULL, 0, 0, 0,
                               GetStockObject(4),
                               LoadCursor(NULL, IDC_ARROW), LoadIcon(NULL, IDI_WINLOGO),
                               WS_POPUP | WS_VISIBLE, 0, 0, width, height,
                               NULL, NULL, GetModuleHandle(0), winproc, NULL))) {
      return NULL;
    }

    vmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

    if (ChangeDisplaySettings(&vmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
      return NULL;
    }
  }

  ShowCursor(!fullscreen);
  ShowWindow(hwin, SW_SHOWNORMAL);
  UpdateWindow(hwin);

  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = bpp;
  pfd.cDepthBits = zbits;
  //pfd.cStencilBits =GLUE_USE_SHADOWS==1?8:0;
  pfd.iLayerType = PFD_MAIN_PLANE;

  if (!(hdc = GetDC(hwin))) return NULL;
  if (!(pixelformat = ChoosePixelFormat(hdc, &pfd))) return NULL;
  if (!SetPixelFormat(hdc, pixelformat, &pfd)) return NULL;
  if (!(hrc = wglCreateContext(hdc))) return NULL;
  if (!wglMakeCurrent(hdc, hrc)) return NULL;

  {
    PIXELFORMATDESCRIPTOR p;

    DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &p);
    glueSbits=p.cDepthBits;
  }

/*
  ramp_used=0;

  if (glueGamma!=-1) {
    if (GetDeviceGammaRamp(hdc, (LPVOID)&old_gammaramp)) {
      int x;
      WORD ramppi[768];

      for (x=0; x<256; x++) {
        ramppi[0*256+x]=pow(x/255.0, glueGamma)*65535;
        ramppi[1*256+x]=pow(x/255.0, glueGamma)*65535;
        ramppi[2*256+x]=pow(x/255.0, glueGamma)*65535;
      }

      if (!SetDeviceGammaRamp(hdc, (LPVOID)&ramppi)) {
        glueNotice("Gamma ramp shit failed, using defaults or whatever and things");
      } else {
        ramp_used=1;
      }

    } else {
      glueNotice("Gamma ramp shit failed, using defaults or whatever and things");
    }
  }
  */
  return hwin;
}


void glueCloseconsole(void) {
  long john;

  if (/*glueMultisample==*/0) {
//    if (ramp_used) SetDeviceGammaRamp(hdc, (LPVOID)&old_gammaramp);

    if (hrc)	{
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(hrc);
      hrc = NULL;
    }

    if (hdc && !ReleaseDC(hwin, hdc)) {
      hdc = NULL;
    }

    if (hwin != NULL) {
      DestroyWindow(hwin);
      hwin = NULL;
    }

    } else {

//    if (ramp_used) SetDeviceGammaRamp(hdc_fsaa, (LPVOID)&old_gammaramp);

    if (hrc_fsaa)	{
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(hrc_fsaa);
      hrc_fsaa = NULL;
    }

    if (hdc_fsaa && !ReleaseDC(hwin_fsaa, hdc_fsaa)) {
      hdc_fsaa = NULL;
    }

    if (hwin_fsaa != NULL) {
      DestroyWindow(hwin_fsaa);
      hwin_fsaa = NULL;
    }
  }

  ShowCursor(1);

  if (glueFullscreen==1) {
    john=ChangeDisplaySettings(NULL, 0);

    if (john != DISP_CHANGE_SUCCESSFUL) {
      char erreur[100];

      if (john==DISP_CHANGE_RESTART) sprintf(erreur, "DISP_CHANGE_RESTART");
      if (john==DISP_CHANGE_BADFLAGS)	sprintf(erreur, "DISP_CHANGE_BADFLAGS");
      if (john==DISP_CHANGE_FAILED)	sprintf(erreur, "DISP_CHANGE_FAILED");
      if (john==DISP_CHANGE_BADMODE) sprintf(erreur, "DISP_CHANGE_BADMODE");
      if (john==DISP_CHANGE_NOTUPDATED) sprintf(erreur, "DISP_CHANGE_NOTUPDATED");

#ifdef HIDE_KEWLERS
    glueNoticef("Error restoring display mode: %s", erreur);
#else
    glueNoticef("Error restoring display mode, your windows sucks: %s", erreur);
#endif

    }
  }
}

#endif // SIMPLE_FULLSCREEN_DIALOG



int mdisk_pause=0;

glue_static void glueSwap() {
  //if (glueMultisample)
  SwapBuffers(hdc_fsaa);
  //else SwapBuffers(hdc);
}


glue_static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  static unsigned char mjut=0;


  glueMessages.msg=msg;
  glueMessages.wparam=wparam;
  glueMessages.lparam=lparam;

  switch(msg) {
    case WM_DESTROY:
      if (!this_is_multisample_calling) PostQuitMessage(0);
      break;
    case WM_KEYDOWN:
     	//glueKeyPressed = wparam;
      switch(wparam) {
        case VK_ESCAPE:
#ifdef GLUE_USE_3DX
          glueWrite3dxfile();
#endif          
          if (glueTicktablepos>0) glueWriteticktable();
          glueDone++;
          break;
#if GLUE_DEVEL_MODE==1
        case VK_SPACE: {
#ifndef NO_FMOD
#ifndef INTRO
            float t=fmod_gettime();
            glueTicktable[glueTicktablepos++]=(int)(t*64);
#endif // intro
#endif // no_fmod
            break;
          }
        case VK_SHIFT:
          break;
        case VK_BACK:
          break;
#endif // devel_mode

#ifndef INTRO
#ifndef NO_FMOD
        case VK_NEXT:
          {
            if (fmod_pausetime==0) {
              int t=floor(fmod_gettime());
              fmod_settime(t+1);
            } else {
              fmod_pausetime+=1/256.0;
            }
          }
          break;
        case VK_PRIOR:
          {
            if (fmod_pausetime==0) {
              int t=floor(fmod_gettime());
              fmod_settime(t-1);
            } else {
              fmod_pausetime-=1/256.0;
            }
          }
          break;
        case VK_HOME:
          fmod_settime(0);
          break;
        case VK_PAUSE:
          if (fmod_pausetime==0) {
            fmod_pausetime=fmod_gettime();
          } else {
            fmod_pausetime=0;
          }
          break;
#endif
#endif

        default:
          break;
      }
      break;
    case WM_KEYUP:
    		//glueKeyPressed=-1;
      break;
    case WM_CHAR:
      switch((char)wparam) {
/*
        case 'k':
          if (kewlmodecnt==0) kewlmodecnt++;
          break;
        case 'e':
          if (kewlmodecnt==1) kewlmodecnt++;
          break;
        case 'w':
          if (kewlmodecnt==2) kewlmodecnt++;
          break;
        case 'l':
          if (kewlmodecnt==3) kewlmode++;
          break;  */

#if GLUE_DEVEL_MODE==1
        case 'i':
          glueShowinformation=(glueShowinformation+1)%2;
          break;
        case 'd':
          glueShowdebug=(glueShowdebug+1)%2;
          break;
        case 's':
          glueWritebmp();
          break;
        case 'r':
          precalc();
          break;
#ifndef INTRO
#ifndef NO_FMOD
        case 'p':  
          if (fmod_pausetime==0) {
            fmod_pausetime=fmod_gettime();
          } else {
            fmod_pausetime=0;
            set_timerdelta();
          }        
          break;
        case 'm':
          mjut=(mjut+1)&1;
          #ifndef USE_FMODEX
	          FSOUND_SetMute(FSOUND_ALL, mjut);
	        #else
	          FMOD_Channel_SetMute(0, mjut);
	        #endif
#endif
#endif
#endif // develmode
        default:
          break;
      }
      break;
    case WM_MOUSEMOVE: {
      int xpos = LOWORD(lparam);
      int ypos = HIWORD(lparam);

      glueMousekey = wparam;
      glueMousepos.x=1.0*xpos/glueXres;
      glueMousepos.y=1.0*ypos/glueYres;
      glueMousepos.z=0;
      //glueDebugf_tiny("xpos=%i, ypos=%i", xpos, ypos);

      break;
    }

    break;

#ifndef NO_FMOD
#if GLUE_DEVEL_MODE==1
    case WM_MOUSEWHEEL: {
      float delta = GET_WHEEL_DELTA_WPARAM(wparam)/1024.0;
      float t=fmod_gettime();
      if (fmod_pausetime==0) {
        fmod_settime(t+delta);
      } else {
        fmod_pausetime+=delta*0.01;
      }

      break;
    }
#endif
#endif

/*
#if GLUE_DEVEL_MODE==1
    case WM_COPYDATA: {
      PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lparam;

      camelHWND=(HWND)wparam;
      handle_camel_message((char *)pcds->lpData, pcds->cbData);
      break;
    }
#endif // devel_mode
*/
    default:
      break;
  }

  return (DefWindowProc(hwnd, msg, wparam, lparam));
}



#ifndef INTRO
#ifndef SIMPLE_FULLSCREEN_DIALOG

//LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  int mode;
  int selection;
  HWND resolist;
  HWND aspectlist;
  BOOL rval;
  DEVMODE devmode;
  char description[255];
  int foundsomething;
  int index;
  int aspect;
//  float desktopaspect;
  static int index_43=0;
  static int index_1610=0;
  static int index_169=0;
  static float aspects[1024];


  switch(msg) {
    case WM_INITDIALOG:

      if (GLUE_DEVEL_MODE!=1) CheckDlgButton(hwnd, IDC_FULLSCREEN, BST_CHECKED);
      //CheckDlgButton(hwnd, IDC_MULTISAMPLE, BST_CHECKED);

      // display mode listbox
      resolist=GetDlgItem(hwnd, IDC_RESOLIST);
      if (resolist==NULL) glueError("Error 7251");

      foundsomething=0;
      rval=TRUE;
      int fukkendorf = getenv("FUCK_GLUE")!=NULL;
      for (mode=0; rval; mode++) {

        rval=EnumDisplaySettings(NULL, mode, &devmode);

        if ((devmode.dmPelsWidth >= GLUE_MIN_XRES &&
            devmode.dmPelsWidth <= GLUE_MAX_XRES &&
            devmode.dmPelsHeight >= GLUE_MIN_YRES &&
            devmode.dmPelsHeight <= GLUE_MAX_YRES || fukkendorf) &&
            devmode.dmBitsPerPel>31) {

          foundsomething++;
#if GLUE_DEVEL_MODE==1
          sprintf(description, "%i - %s: %d x %d, %d bpp %d Hz", mode, devmode.dmDeviceName, (int)devmode.dmPelsWidth, (int)devmode.dmPelsHeight, (int)devmode.dmBitsPerPel, (int)devmode.dmDisplayFrequency);
#else
          sprintf(description, "%s: %d x %d, %d bpp %d Hz", devmode.dmDeviceName, (int)devmode.dmPelsWidth, (int)devmode.dmPelsHeight, (int)devmode.dmBitsPerPel, (int)devmode.dmDisplayFrequency);
#endif
          index=SendMessage(resolist, LB_ADDSTRING, 0, (LPARAM)description);
          if (index==LB_ERR) glueError("Error 5127");
          if (SendMessage(resolist, LB_SETITEMDATA, (WPARAM)index, (LPARAM)mode)==LB_ERR) glueError("Error 1189");

          if (mode<1024) aspects[mode]=(float)devmode.dmPelsWidth/(float)devmode.dmPelsHeight;
          else glueError("Error 7661");

          ONCE (
            if (SendMessage(resolist, LB_SETCURSEL, (WPARAM)index, 0)==LB_ERR) glueError("Error 8812");
          );
        }
      }

#ifdef HIDE_KEWLERS
      if (foundsomething==0) glueErrorf("Could not find any suitable display modes");
#else
      if (foundsomething==0) glueErrorf("Could not find any suitable display modes, buy some real hardware asshole!");
#endif

      // aspect ratio listbox
      aspectlist=GetDlgItem(hwnd, IDC_ASPECTLIST);
      if (aspectlist==NULL) glueError("Error 6109");

      sprintf(description, "4:3");
      index=SendMessage(aspectlist, LB_ADDSTRING, 0, (LPARAM)description);
      if (index==LB_ERR) glueError("Error 7989");
      if (SendMessage(aspectlist, LB_SETITEMDATA, (WPARAM)index, (LPARAM)0)==LB_ERR) glueError("Error 7619");
      if (SendMessage(aspectlist, LB_SETCURSEL, (WPARAM)index, 0)==LB_ERR) glueError("Error 1231");
      index_43=index;

      sprintf(description, "16:10");
      index=SendMessage(aspectlist, LB_ADDSTRING, 0, (LPARAM)description);
      if (index==LB_ERR) glueError("Error 8661");
      if (SendMessage(aspectlist, LB_SETITEMDATA, (WPARAM)index, (LPARAM)1)==LB_ERR) glueError("Error 6561");
      index_1610=index;

     sprintf(description, "16:9");
      index=SendMessage(aspectlist, LB_ADDSTRING, 0, (LPARAM)description);
      if (index==LB_ERR) glueError("Error 8491");
      if (SendMessage(aspectlist, LB_SETITEMDATA, (WPARAM)index, (LPARAM)2)==LB_ERR) glueError("Error 9914");
      index_169=index;

      break;
    case WM_COMMAND:
	    switch(LOWORD(wParam)) {
        case ID_RUN:

          if (IsDlgButtonChecked(hwnd, IDC_FULLSCREEN)) glueFullscreen=1;
          else glueFullscreen=0;

          if (IsDlgButtonChecked(hwnd, IDC_LOOP)) glueLoop=1;
          else glueLoop=0;

          if (IsDlgButtonChecked(hwnd, IDC_MULTISAMPLE)) glueMultisample=1;
          else glueMultisample=0;

          if (IsDlgButtonChecked(hwnd, IDC_GAMMA)) glueGamma=1.0/1.4;
          else glueGamma=-1.0;

          // get selected resoluution
          resolist=GetDlgItem(hwnd, IDC_RESOLIST);
          if (resolist==NULL) glueError("Error 9456");

          selection=SendMessage(resolist, LB_GETCURSEL, 0, 0);
          if (selection==LB_ERR) glueError("Error 7678");
          mode=SendMessage(resolist, LB_GETITEMDATA, selection, 0);
          if (mode==LB_ERR) glueError("Error 3256");
          glueMode=mode;
          // get selected aspect ratio
          aspectlist=GetDlgItem(hwnd, IDC_ASPECTLIST);
          if (aspectlist==NULL) glueError("Error 4589");

          selection=SendMessage(aspectlist, LB_GETCURSEL, 0, 0);
          if (selection==LB_ERR) glueError("Error 9822");
          aspect=SendMessage(aspectlist, LB_GETITEMDATA, selection, 0);

          if (aspect==LB_ERR) glueError("Error 6123");
          else if (aspect==0) glueAspect=4.0/3.0;
          else if (aspect==1) glueAspect=16.0/10.0;
          else if (aspect==2) glueAspect=16.0/9.0;
          else glueError("Error 9218");

          glueWidescreen=glueAspect/glue_widescreen;

          EndDialog(hwnd, 1);
          break;
        case ID_EXIT:
          EndDialog(hwnd, 0);
          break;

        // yritet‰‰n valkata oikea aspekti automaattisesti
          /*
        case IDC_RESOLIST:

          resolist=GetDlgItem(hwnd, IDC_RESOLIST);
          if (resolist==NULL) glueError("zzzbla1");

          selection=SendMessage(resolist, LB_GETCURSEL, 0, 0);
          if (selection==LB_ERR) glueError("zzzbla2");
          mode=SendMessage(resolist, LB_GETITEMDATA, selection, 0);
          if (mode==LB_ERR) glueError("zzzbla3");

          aspectlist=GetDlgItem(hwnd, IDC_ASPECTLIST);
          if (aspectlist==NULL) glueError("zzzbla4");

          if ((int)(aspects[mode]*1000)==(int)(4.0/3.0*1000)) if (SendMessage(aspectlist, LB_SETCURSEL, (WPARAM)index_43, 0)==LB_ERR) glueError("vittu1");
          if ((int)(aspects[mode]*1000)==(int)(16.0/10.0*1000)) if (SendMessage(aspectlist, LB_SETCURSEL, (WPARAM)index_1610, 0)==LB_ERR) glueError("vittu2");
          if ((int)(aspects[mode]*1000)==(int)(19.0/9.0*1000)) if (SendMessage(aspectlist, LB_SETCURSEL, (WPARAM)index_169, 0)==LB_ERR) glueError("vittu3");

          break; */
      }
      break;
    case WM_DESTROY:
	    EndDialog(hwnd, 0);
	    break;
    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
#ifndef GLUE_NO_DIALOGLOGO
        BITMAP bm;


        HDC hdcmem = CreateCompatibleDC(hdc);
        HBITMAP hbmold;
        HBITMAP logo;

        logo=LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_LOGO));
        if (logo==NULL) glueError("Error 1299");

        hbmold = SelectObject(hdcmem, logo);

        if (!GetObject(logo, sizeof(bm), &bm)) glueError("Error 8162");
        if (!BitBlt(hdc, 2, 2, bm.bmWidth, bm.bmHeight, hdcmem, 0, 0, SRCCOPY)) glueError("Error 1712");

        SelectObject(hdcmem, hbmold);
        DeleteDC(hdcmem);

        EndPaint(hwnd, &ps);
#endif
      }
    default:
      return 0;
  }
  return 1;
}

#endif // SIMPLE_FULLSCREEN_DIALOG
#endif //INTRO




#ifdef SIMPLE_FULLSCREEN_DIALOG


glue_static HWND _CreateWindow_old(WNDCLASS *window, char *lpszClassName, char *lpszWinTitle,
                   char *lpszMenuName, int cbClsExtra, int cbWndExtra, UINT style,
                   HBRUSH hbrBackground, HCURSOR hCursor, HICON hIcon, DWORD dwStyle,
                   int winx, int winy, int width, int height, HWND hWndParent,
                   HMENU hMenu, HINSTANCE hinstance, WNDPROC WindowProc, LPVOID lpParam) {
  HWND hndl;

  window->cbClsExtra = cbClsExtra;
  window->cbWndExtra = cbWndExtra;
  window->hbrBackground = hbrBackground;
  window->hCursor = hCursor;
  window->hIcon = hIcon;
  window->hInstance = hinstance;
  window->lpfnWndProc = WindowProc;
  window->lpszClassName = lpszClassName;
  window->lpszMenuName = lpszMenuName;
  window->style = style;

  if(!RegisterClass(window)) return NULL;

  if(!(hndl = CreateWindow(lpszClassName, lpszWinTitle,dwStyle, winx, winy,
                           width, height, hWndParent, hMenu, hinstance, lpParam))) {
    return NULL;
  }

  return hndl;
}

HWND glueOpenconsole_old(int fullscreen, int width, int height, int bpp, int zbits, char *title,
                     int x, int y, WNDPROC winproc) {
  HWND hwnd;
  DEVMODE vmode;
  GLuint pixelformat;
  PIXELFORMATDESCRIPTOR pfd;

  if (!fullscreen) { // window
    if (!(hwnd = _CreateWindow_old(&window, glueName, glueName, NULL, 0, 0, CS_VREDRAW | CS_HREDRAW,
                               GetStockObject(4),
                               LoadCursor(NULL, IDC_ARROW), LoadIcon(NULL, IDI_WINLOGO),
                               WS_SYSMENU | WS_CAPTION | WS_VISIBLE, x, y, width, height,
                               NULL, NULL, GetModuleHandle(0), winproc, NULL))) {
      return NULL;
    } else {
      hwin = hwnd;
    }
  } else { // fullscreen
    if(!(hwnd = _CreateWindow_old(&window, glueName, glueName, NULL, 0, 0, 0,
                              GetStockObject(4),
                              LoadCursor(NULL, IDC_ARROW), LoadIcon(NULL, IDI_WINLOGO),
                              WS_POPUP | WS_VISIBLE, 0, 0, width, height,
                              NULL, NULL, GetModuleHandle(0), winproc, NULL))) {
      return NULL;
    } else {
      hwin = hwnd;
    }

    memset(&vmode, 0, sizeof(vmode));
    vmode.dmSize = sizeof(vmode);
    vmode.dmPelsWidth = width;
    vmode.dmPelsHeight = height;
    vmode.dmBitsPerPel = bpp;
    vmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    if (hwnd != NULL) {
      if (ChangeDisplaySettings(&vmode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
        return NULL;
      }
    } else {
      return NULL;
    }
  }

  ShowCursor(!fullscreen);
  ShowWindow(hwnd, SW_SHOWNORMAL);
  UpdateWindow(hwnd);

  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = bpp;
  pfd.cDepthBits = zbits;
  pfd.iLayerType = PFD_MAIN_PLANE;

  if (hwnd != NULL) {
    if (!(hdc = GetDC(hwnd))) return NULL;

    if (!(pixelformat = ChoosePixelFormat(hdc, &pfd))) return NULL;

    if (!SetPixelFormat(hdc, pixelformat, &pfd)) return NULL;

    if (!(hrc = wglCreateContext(hdc))) return NULL;
    if (!wglMakeCurrent(hdc, hrc)) return NULL;
  }
  return hwnd;
}


glue_static void glueCloseconsole_old(void) {


  if (!ChangeDisplaySettings(NULL, 0)) {
    // hmm
  }

  if (hrc)	{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    hrc = NULL;
  }

  if(hdc && !ReleaseDC(hwin, hdc)) {
    hdc = NULL;
  }

  if(hwin != NULL) {
    DestroyWindow(hwin);
    hwin = NULL;
  }

  ShowCursor(TRUE);
}



#endif // SIMPLE_FULLSCREEN_DIALOG


/**********************************************************************
   Message stuff
*********************************************************************/


glue_static void glueError(char errormsg[]) {
  int retval;


  retval=MessageBox(0, errormsg, glueName, MB_ICONSTOP|MB_OK);
  if (getenv("FUCK_GLUE")) return;
#ifdef SIMPLE_FULLSCREEN_DIALOG
  glueCloseconsole_old();
#else
  glueCloseconsole();
#endif
  exit(0);
}


glue_static void glueErrorf(char *fmt, ...) {
  static char buf[8000];
  int retval;
  va_list ap;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  glueError(buf);
}

glue_static void glueNotice(char errormsg[]) {
  int retval;

  retval=MessageBox(0, errormsg, glueName, MB_ICONWARNING|MB_OK);
}

glue_static void glueNoticef(char *fmt, ...) {
  static char buf[8000];
  int retval;
  va_list ap;

  va_start(ap, fmt);
  vsprintf(buf, fmt, ap);
  retval=MessageBox(0, buf, glueName, MB_ICONWARNING|MB_OK);
  va_end(ap);
}


glue_static void glueInit(void) {
  int rval;

  glueFullscreen=1;
  glueXres=1024;
  glueYres=768;

#ifdef SIMPLE_FULLSCREEN_DIALOG

  if (MessageBox(NULL, "Fullscreen?", glueName, MB_YESNO|MB_ICONQUESTION)==IDNO) glueFullscreen=0;
  else glueFullscreen=1;

  if (!glueOpenconsole_old(glueFullscreen, glueXres, glueYres, 32, 16, glueName, 0, 0, WindowProc)) glueError("Init failed!");

#else
  rval=DialogBox(hinstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, DlgProc);
  if (rval!=1) {
    //glueError("error opening dialog");
    exit(0);
  }

// ei toimi en‰‰, mode asetetaan numeron perusteella...
//  if (MessageBox(NULL, "Fullscreen?", glueName, MB_YESNO|MB_ICONQUESTION)==IDNO) glueFullscreen=0;
//  else glueFullscreen=1;

  if (!glueOpenconsole(glueFullscreen, 16, glueName, 0, 0, WindowProc)) {
#ifdef HIDE_KEWLERS
    glueError("Error occured while creating a window!");
#else
	glueError("Something went wrong while creating a window!");
#endif
  }
#endif // SIMPLE_FULLSCREEN_DIALOG


  eftexture_xsize=texreso(glueXres);
  eftexture_ysize=texreso(glueYres*glueWidescreen);

//  srand((unsigned)timer_start);
//  BuildFont();

  init_extensions();
  glueInitfbo();
  glueGlsl_init();

  testit();
  init_tinyfont();

  glueDummytex=glueColortexture(1, 1, 255, 255, 255, 255, GLUE_NO_MIPMAP);

}


glue_static void glueWritebmp() {
  int w=glueXres;
  int h=glueYres;
  static int num=0;
  char filename[13];
  FILE *f;
  BITMAPINFOHEADER infoHeader;
  BITMAPFILEHEADER fileHeader;
  unsigned int i;
  unsigned char *img=malloc(w*h*3);

  memset(img, 0, w*h*3);
  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img);

  sprintf(filename, "kwl%05i.bmp", num);
//  if (!f=fopen(filename, "wb")) { return 0; }
  f=fopen(filename, "wb");

  // Define whats going in the infoHeader.
  infoHeader.biSize = sizeof(BITMAPINFOHEADER);
  infoHeader.biPlanes        = 1;
  infoHeader.biBitCount      = 24;
  infoHeader.biCompression   = BI_RGB;
  infoHeader.biSizeImage     = w*h*3;
  infoHeader.biXPelsPerMeter = 0;
  infoHeader.biYPelsPerMeter = 0;
  infoHeader.biClrUsed       = 0;
  infoHeader.biClrImportant  = 0;
  infoHeader.biWidth         = w;
  infoHeader.biHeight        = h;

  // Define whats going in the file header.
  fileHeader.bfSize       = sizeof(BITMAPFILEHEADER);
  fileHeader.bfType       = 0x4D42;
  fileHeader.bfReserved1  = 0;
  fileHeader.bfReserved2  = 0;
  fileHeader.bfOffBits    = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

  for (i=0; i<infoHeader.biSizeImage; i+=3) {
    unsigned char swap;

    swap=img[i+0];
    img[i+0]=img[i+2];
    img[i+2]=swap;
  }

  fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), f);
  fwrite(&infoHeader, 1, sizeof(BITMAPINFOHEADER), f);
  fwrite(img, 1, infoHeader.biSizeImage, f);
  fclose(f);

  free(img);
  num++;
}



glue_static int clx(int a) { return a<0?0:a>glueXres?glueXres:a; }
glue_static int cly(int a) { return a<0?0:a>glueYres?glueYres:a; }



/*
void glueWritebmp_focalblur() {
  int w=glueXres;
  int h=glueYres;
  static int num=0;
  char filename[12];
  FILE *f;
  BITMAPINFOHEADER infoHeader;
  BITMAPFILEHEADER fileHeader;
  unsigned int i;
  int x, y;
  unsigned char *img1=malloc(w*h*3);
  unsigned int  *img2=malloc(w*h*3*8);
  unsigned char *img3=malloc(w*h*3);
  float *dbuf=malloc(w*h*sizeof(float));

  memset(img1, 0, w*h*3);
  memset(img2, 0, w*h*3*8);
  memset(img3, 0, w*h*3);
  memset(dbuf, 0, w*h*sizeof(float));

  glPushAttrib(GL_PIXEL_MODE_BIT);

  glPixelTransferf(GL_DEPTH_SCALE, 1.0);
  glPixelTransferf(GL_DEPTH_BIAS,  0);

  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, img1);
  glReadPixels(0, 0, w, h, GL_DEPTH_COMPONENT, GL_FLOAT, dbuf);

  glPopAttrib();

  sprintf(filename, "kwl%05i.bmp", num);
  f=fopen(filename, "wb");

  // Define whats going in the infoHeader.
  infoHeader.biSize = sizeof(BITMAPINFOHEADER);
  infoHeader.biPlanes        = 1;
  infoHeader.biBitCount      = 24;
  infoHeader.biCompression   = BI_RGB;
  infoHeader.biSizeImage     = w*h*3;
  infoHeader.biXPelsPerMeter = 0;
  infoHeader.biYPelsPerMeter = 0;
  infoHeader.biClrUsed       = 0;
  infoHeader.biClrImportant  = 0;
  infoHeader.biWidth         = w;
  infoHeader.biHeight        = h;

  // Define whats going in the file header.
  fileHeader.bfSize       = sizeof(BITMAPFILEHEADER);
  fileHeader.bfType       = 0x4D42;
  fileHeader.bfReserved1  = 0;
  fileHeader.bfReserved2  = 0;
  fileHeader.bfOffBits    = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

  for (y=0; y<h; y++) for (x=0; x<w; x++) {
    int u, v, c;
    //float d=clamp(fabs(dbuf[y*glueXres+x]-0.9)*5.0);
    float d=clamp(fabs(pow(1.0-dbuf[y*glueXres+x],0.4)-0.3)*2.0);
    float r=1.0+d*8;
    float pa=pi*r*r*0.75;

    for (c=0; c<3; c++) {
      for (v=-(int)(r)-1; v<(int)(r)+2; v++) for (u=-(int)(r)-1; u<(int)(r)+2; u++) {
        int xx, yy;
        float kpps;

        kpps=clamp(r-sqrt(v*v+u*u));
        xx=clx(x+u);
        yy=cly(y+v);
        img2[yy*w*3+xx*3+c]+=img1[y*w*3+x*3+c]*65536*kpps/pa;
      }
    }
  }

  for (i=0; i<infoHeader.biSizeImage; i+=3) {

    img3[i+0]=r0255(img2[i+2]/65536);
    img3[i+1]=r0255(img2[i+1]/65536);
    img3[i+2]=r0255(img2[i+0]/65536);
  }

/ *
  for (i=0; i<infoHeader.biSizeImage; i+=3) {
    unsigned char swap;
    //float c=clamp(pow(fabs(dbuf[i/3]-0.9), 2.0)*10);  aeka hjuvae
    float c=clamp(fabs(pow(1.0-dbuf[i/3],0.4)-0.3)*2.0);

    img3[i+0]=c*255;
    img3[i+1]=c*255;
    img3[i+2]=c*255;
  }
* /
  fwrite(&fileHeader, 1, sizeof(BITMAPFILEHEADER), f);
  fwrite(&infoHeader, 1, sizeof(BITMAPINFOHEADER), f);
  fwrite(img3, 1, infoHeader.biSizeImage, f);
  fclose(f);

  free(img1);
  free(img2);
  free(img3);
  free(dbuf);
  num++;
}
*/

#ifndef INTRO
#ifndef NO_FMOD
float glueGettime(void) {
  return fmod_gettime();
}
void glueMod_playfile(char *filename, float bpm, int loop) {
	#ifndef USE_FMODEX
    fmod_play(filename, bpm, loop);
	#else
		// wrapper for fmod 3.x functionality - always use channel 0, i.e. overwrite old sound that is playing
		fmod_load(0, bpm, filename, 0);
		fmod_play(0, loop);
	#endif
}
void glueMod_close(void) {
  fmod_close();
}
#endif
#endif


glue_static void glueClose() {
#ifdef SIMPLE_FULLSCREEN_DIALOG
  glueCloseconsole_old();
#else
  glueCloseconsole();
#endif
}

