#define WIN32_LEAN_AND_MEAN
#include <time.h>
    
// include glue setup file
// also contains the datafile-stuff
#include "glue_setup.h"


// Tell here where GLUE is
#define gluedirraw(x) ../glue1.5/x
// some stringification macros
#define gluestringify(x) #x
#define gluestringify2(x) gluestringify(x)
#define gluedir(x) gluestringify2(gluedirraw(x))
// include the necessary glue headers
#include gluedir(glue.h)
#include gluedir(fmodcrap.h)
#ifdef I_LOVE_BILL
  #include gluedir(gluewin.h)
#else
  #include gluedir(gluelin.h)
#endif
#include gluedir(gluetinyfont.h)
#include gluedir(fmodcrap.h)
#include gluedir(gluegen.h)
#include gluedir(glueobj.h)
#include gluedir(gluetxtr.h)
#include gluedir(glueext.h)
#include gluedir(gluepart.h)
#include gluedir(gluemem.h)
#include gluedir(gluesort.h)
#include gluedir(gluefont.h)
#include gluedir(gluenoise.h)
#ifdef GLUE_USE_3DX
  #include gluedir(glue3dx.h)
#endif
#if GLUE_USE_ARBSHADOW==1
	#include gluedir(gluesmap.h)
#endif
#if GLUE_USE_MPEG2==1
  #include gluedir(gluempeg.h)
#endif
#if GLUE_USE_CUBE==1
  #include gluedir(gluecube.h)
#endif
#include gluedir(gluefbo.h)
#include gluedir(glueovrl.h)
#include gluedir(glueglsl.h)
#include gluedir(gluesect.h)


