#define GLUE

static HWND glueWindow;
static HDC glueHdc;

//#define glueErrorf printf
//#define glueNoticef printf

#ifndef GL_ALL_CLIENT_ATTRIB_BITS
#define GL_ALL_CLIENT_ATTRIB_BITS GL_CLIENT_ALL_ATTRIB_BITS
#endif


static void glueSwap();
static int glueOpenwindow();
static void glueClosewindow(void);
static void glueInit(int xres, int yres, float aspect);
static void glueClose();
glue_static void glueError(char errormsg[]);
glue_static void glueErrorf(char *fmt, ...);
glue_static void glueNotice(char errormsg[]);
glue_static void glueNoticef(char *fmt, ...);
