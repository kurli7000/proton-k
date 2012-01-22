
#include "sync/sync.h"
#include "sync/base.h"
#include "bass.h"

// use this if you wanna get values before init, for ATI or sumtin
float SYNC(const struct sync_track *t, double row) {
	if (t) return sync_get_val(t, row);
  else return 0.0;
}

int SYNCi(const struct sync_track *t, double row) {
	return (int)floor(SYNC(t, row));
}


HSTREAM stream;
#define bpm 124.0
#define rpb 8  /* rows per beat */
const char biisi[]="data/Pulss.mp3";
double row_rate = ((double)(bpm) / 60.0) * rpb;


struct sync_device *rocket;
const struct sync_track *cam_rot, *cam_dist, *cam_xoffs, *cam_yoffs, *cam_zoffs, *cam_fov, *cam_tilt, *cam_tgtx, *cam_tgty, *cam_tgtz;
//const struct sync_track *phone_size, *phone_ypos, *phone_dispbr, *phone_fx, *phone_xplode;
const struct sync_track *lodder_xpos, *lodder_ypos, *lodder_zpos, *lodder_diam, *lodder_powr, *lodder_push;
const struct sync_track *light_x, *light_y, *light_z, *light_fov;
//const struct sync_track *shredder_limit;
//const struct sync_track *part_pow, *part_frq, *part_time;
//const struct sync_track *dof_radius, *dof_ramp;
const struct sync_track *effect_br, *glow_br, *glow_rad, *glow_minus;
const struct sync_track *logonum, *logobr;
const struct sync_track *lodder_br;
const struct sync_track *slitpos, *slitbr, *flarebr, *subzpos;
const struct sync_track *d_blockpow, *d_sinpow, *d_linepow, *d_colorsep;
const struct sync_track *pipes_br;
const struct sync_track *ball_sep, *ball_act, *ball_move, *ball_act2;

#define TRACKS \
	cam_rot = sync_get_track(rocket, "cam.rot"),\
	cam_dist = sync_get_track(rocket, "cam.dist");\
	cam_xoffs = sync_get_track(rocket, "cam.xoffset");\
	cam_yoffs = sync_get_track(rocket, "cam.yoffset");\
	cam_zoffs = sync_get_track(rocket, "cam.zoffset");\
	cam_fov = sync_get_track(rocket, "cam.fov");\
	cam_tilt = sync_get_track(rocket, "cam.tilt");\
  cam_tgtx = sync_get_track(rocket, "tgt.x");\
  cam_tgty = sync_get_track(rocket, "tgt.y");\
  cam_tgtz = sync_get_track(rocket, "tgt.z");\
  lodder_xpos = sync_get_track(rocket, "lodder.xpos");\
  lodder_ypos = sync_get_track(rocket, "lodder.ypos");\
  lodder_zpos = sync_get_track(rocket, "lodder.zpos");\
  lodder_diam = sync_get_track(rocket, "lodder.diam");\
  lodder_powr = sync_get_track(rocket, "lodder.powr");\
  lodder_push = sync_get_track(rocket, "lodder.push");\
  light_x = sync_get_track(rocket, "light.x");\
  light_y = sync_get_track(rocket, "light.y");\
  light_z = sync_get_track(rocket, "light.z");\
  light_fov = sync_get_track(rocket, "light.fov");\
  effect_br = sync_get_track(rocket, "effect.br");\
  glow_br = sync_get_track(rocket, "glow.br");\
  glow_rad = sync_get_track(rocket, "glow.rad");\
  glow_minus = sync_get_track(rocket, "glow.minus");\
  logonum = sync_get_track(rocket, "logo.num");\
  logobr = sync_get_track(rocket, "logo.br");\
  lodder_br = sync_get_track(rocket, "lodder.br");\
  slitpos = sync_get_track(rocket, "slit.pos");\
  slitbr = sync_get_track(rocket, "slit.br");\
  flarebr = sync_get_track(rocket, "flare.br");\
  subzpos = sync_get_track(rocket, "sub.zpos");\
  d_blockpow = sync_get_track(rocket, "d.blockpow");\
  d_sinpow = sync_get_track(rocket, "d.sinpow");\
  d_linepow = sync_get_track(rocket, "d.linepow");\
  d_colorsep = sync_get_track(rocket, "d.colorsep");\
  pipes_br = sync_get_track(rocket, "pipes.br");\
  ball_sep = sync_get_track(rocket, "balls.sep");\
  ball_act = sync_get_track(rocket, "balls.act");\
  ball_act2 = sync_get_track(rocket, "balls.act2");\
  ball_move = sync_get_track(rocket, "balls.move");\
  
  
//////////////////////////////////////////////////////////////

double bass_get_row(HSTREAM h)
{
	QWORD pos = BASS_ChannelGetPosition(h, BASS_POS_BYTE);
	double time = BASS_ChannelBytes2Seconds(h, pos);
	return time * row_rate + 0.001;
}


Camera rocketcamera() {
  double row=bass_get_row(stream);

  Camera cam = glueDefaultcamera2(SYNC(cam_rot, row), SYNC(cam_dist, row));
  cam.pos.x += SYNC(cam_xoffs, row);
  cam.pos.y += SYNC(cam_yoffs, row);
  cam.pos.z += SYNC(cam_zoffs, row);
  cam.tgt.x = SYNC(cam_tgtx, row);
  cam.tgt.y = SYNC(cam_tgty, row);
  cam.tgt.z = SYNC(cam_tgtz, row);
  cam.per = SYNC(cam_fov, row);
  cam.tilt = SYNC(cam_tilt, row);

  return cam;
}


//#ifndef SYNC_PLAYER

void bass_pause(void *d, int flag)
{
	if (flag)
		BASS_ChannelPause((HSTREAM)d);
	else
		BASS_ChannelPlay((HSTREAM)d, FALSE);
}

void bass_set_row(void *d, int row)
{
	QWORD pos = BASS_ChannelSeconds2Bytes((HSTREAM)d, row / row_rate);
	BASS_ChannelSetPosition((HSTREAM)d, pos, BASS_POS_BYTE);
}

int bass_is_playing(void *d)
{
	return BASS_ChannelIsActive((HSTREAM)d) == BASS_ACTIVE_PLAYING;
}

float glueGettime() {
  return bass_get_row(stream)/64.0;
}

struct sync_cb bass_cb = {
	bass_pause,
	bass_set_row,
	bass_is_playing
};


//#endif /* !defined(SYNC_PLAYER) */


#include <unistd.h>


void start_bassrocket() {
//	HWND hwnd;

	if (!BASS_Init(-1, 44100, 0, hwin_fsaa, 0)) glueError("failed to init bass");
	stream = BASS_StreamCreateFile(FALSE, biisi, 0, 0, BASS_STREAM_PRESCAN);
	if (!stream) glueError("error opening tune");

	rocket = sync_create_device("data/sync");
	if (!rocket) glueError("error initializing rocket");

#ifndef SYNC_PLAYER
	sync_set_callbacks(rocket, &bass_cb, (void *)stream);
	if (sync_connect(rocket, "localhost", SYNC_DEFAULT_PORT)) {
	  glueError("failed to connect to host");
	}
#endif

  TRACKS

	BASS_Start();
	BASS_ChannelPlay(stream, FALSE);

}


void close_bassrocket() {
	BASS_StreamFree(stream);
	BASS_Free();
}


void rocket_messages() {

  switch(glueMessages.msg) {

    case WM_KEYDOWN:
      switch(glueMessages.wparam) {

        case VK_SPACE:
        	if (bass_is_playing((void *)stream)) bass_pause((void *)stream, TRUE);
        	else bass_pause((void *)stream, FALSE);
          break;
        case VK_UP:
        	bass_set_row((void *)stream, floor(bass_get_row(stream))-1);
          break;
        case VK_DOWN:
        	bass_set_row((void *)stream, floor(bass_get_row(stream))+1);
          break;
        case VK_NEXT:
        	bass_set_row((void *)stream, floor(bass_get_row(stream))+16);
          break;
        case VK_PRIOR:
        	bass_set_row((void *)stream, floor(bass_get_row(stream))-16);
          break;
        default:
          break;
      } //WM_KEYDOWN
      break;

    default:
      break;
  } // message
}



