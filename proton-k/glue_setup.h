/*

  GLUE_SETUP.H

  This is the configuration file for GLUE V1.5
  Put this file in your project folder and make sure that the project folder is in the "additional include dirs"-list
  Change this and you must recompile the whole engine

*/


#ifndef SETUP_H
#define SETUP_H

#define GLUE_USE_ARBSHADOW 1
#define GLUE_USE_ALPHA 1
#define GLUE_USE_CUBE 0
#define GLUE_USE_MPEG2 0
#define GLUE_DEVEL_MODE 0
#define glue_widescreen (16.0/9.0)
#define HIDE_KEWLERS
#define I_LOVE_BILL
//#define FMOD_LEVEL_MULT 0.5
//#define FMOD_OFFSET_MS 1400
//#define GLUE_USE_3DX
#define GLUE_FLOAT_TEXTURE
#define GLUE_VTF // vertex texture fetch
#define NO_FMOD
//#define GLUE_FORCE_MULTISAMPLE

#define GLUE_NO_DIALOGLOGO
#define GLUE_MIN_XRES 640
#define GLUE_MIN_YRES 360
#define GLUE_MAX_XRES 1920
#define GLUE_MAX_YRES 1080


// once you have the final count of textures/objects being loaded,
// uncomment this so the loading bar length becomes right
#include <glueload.h>

// When using a datafile made with datafile-tool, uncomment this
//#define GLUE_DATAFILE_IN_USE

// prototype for precalc, this is called from gluewin when 'r' is pressed
void precalc(void);


#endif
