
#include "gluempeg.h"
#include "fmodex.h"
#include <mmsystem.h>

Mpeg2 *glueMpeg2_open(char *filename, int stream_from_disk) {
  Mpeg2 *mpg=malloc(sizeof(Mpeg2));
  
  memset(mpg, 0, sizeof(Mpeg2));
  
  mpg->decoder = mpeg2_init();
  if (mpg->decoder == NULL) {
	  glueError("Could not allocate a decoder object (libmpeg2)");
  }
    
  mpg->info = mpeg2_info(mpg->decoder);
  
  if (stream_from_disk) {
    mpg->f = fopen(filename, "rb");
    mpg->stream_from_disk=1;
  } else {
    mpg->data = glueLoadfile(filename);
    mpg->stream_from_disk=0;
  }
  mpg->whereami = 0;
  mpg->framenum = 0;
  mpg->prevtime = 0;
  mpg->end = 0;
  
  glueLoading();
  
  return mpg;
}

void glueMpeg2_reset(Mpeg2 *mpg)
{
	if (mpg->stream_from_disk) {
	  fseek(mpg->f, 0, SEEK_SET);
	  mpg->framenum=0;
	  mpg->whereami=0;
	} else {
	  mpg->framenum=0;
	  mpg->whereami=0;
	}
	mpg->end=0;    
  mpg->prevtime = 0;
	mpeg2_reset (mpg->decoder, 1);
}

/*
void glueMpeg2_getframe(Mpeg2 *mpg, Texture *dst) {
  size_t size;
  //mpeg2_info_t *info = &mpg->info;
  int write_frame=0;
  size_t blocksize=MPEG_BUFFER_SIZE;
  unsigned char *tmpbuf=tmpmalloc(dst->xres*dst->yres*4);

  do {
    mpg->state = mpeg2_parse(mpg->decoder);
  
    switch (mpg->state) {
      case STATE_BUFFER:
        blocksize = MPEG_BUFFER_SIZE;
        if (mpg->whereami+blocksize > mpg->datasize-1) blocksize =  mpg->datasize-mpg->whereami-1;
	      if (blocksize>0) mpeg2_buffer(mpg->decoder, mpg->data+mpg->whereami, mpg->data+mpg->whereami+blocksize);
	      mpg->whereami += blocksize;	    
  	    break;
	    case STATE_SEQUENCE:
	      mpeg2_convert(mpg->decoder, mpeg2convert_bgr32, NULL);
	      break;
	    case STATE_SLICE:
	    case STATE_END:
	    case STATE_INVALID_END:
        write_frame++;
        break;
      default:
        break;
    }
  } while ((!write_frame) && blocksize>0);

  if (mpg->info->display_fbuf && write_frame) {
  
    if (dst->xres==mpg->info->sequence->width && dst->yres==mpg->info->sequence->height) glueReloadtexture(dst, mpg->info->display_fbuf->buf[0]);
    else {
      int x;
      
      if (dst->xres < mpg->info->sequence->width || dst->yres < mpg->info->sequence->height) 
        glueErrorf("erreur: ur texture too small for ur videoz (%ix%i vs %ix%i", mpg->info->sequence->width, mpg->info->sequence->height, dst->xres, dst->yres);    
      for (x=0; x<mpg->info->sequence->height; x++) memcpy(tmpbuf+x*dst->xres*4, mpg->info->display_fbuf->buf[0]+x*mpg->info->sequence->width*4, dst->xres*4);
      glueReloadtexture(dst, tmpbuf);
      if (mpg->state==STATE_END) mpg->framenum=0, mpg->whereami=0;
    }
    //memcpy(dst, mpg->info->display_fbuf->buf[0], mpg->info->sequence->width*mpg->info->sequence->height*4);
    mpg->framenum++;
  }
  
  //if (mpg->whereami >= mpg->datasize) mpg->whereami = 0; 
  tmpfree(tmpbuf);
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <io.h>

void getframe(Mpeg2 *mpg, Texture *dst) {
  int frame_ready=0;
  size_t blocksize=MPEG_BUFFER_SIZE;
  static unsigned char bufz[MPEG_BUFFER_SIZE+1];
  int invalid=0;
  
  do {
    mpg->state = mpeg2_parse(mpg->decoder);
  
    switch (mpg->state) {
      case STATE_BUFFER:
        blocksize = MPEG_BUFFER_SIZE;
        //if (mpg->whereami+blocksize > mpg->datasize-1) blocksize =  mpg->datasize-mpg->whereami-1;
	      //if (blocksize>0) 
	      if (mpg->stream_from_disk) {
          blocksize=fread(bufz, 1, MPEG_BUFFER_SIZE, mpg->f);
          mpeg2_buffer(mpg->decoder, bufz, bufz+blocksize);
        } else {
          mpeg2_buffer(mpg->decoder, mpg->data+mpg->whereami, mpg->data+mpg->whereami+blocksize);
	        mpg->whereami += blocksize;
        }	    
  	    break;
	    case STATE_SEQUENCE:
	      mpeg2_convert(mpg->decoder, mpeg2convert_bgr32, NULL);
	      break;
	    case STATE_INVALID_END: {
        //FILE *foo;
        //foo=fopen("moi.txt", "a");
        //fprintf(foo, "STATE_INVALID_END: %d, %i, %i\n", blocksize, (int)ftell(mpg->f), (int)filelength(fileno(mpg->f)));
        //fclose(foo);
        invalid=2;
        frame_ready++;
        break;      
        }
	    case STATE_SLICE:
	    case STATE_END:
        frame_ready++;
        break;

      case STATE_INVALID: {
        //FILE *foo;
        //foo=fopen("moi.txt", "a");
        //fprintf(foo, "STATE_INVALID: %d, %i, %i\n", blocksize, (int)ftell(mpg->f), (int)filelength(fileno(mpg->f)));
        //fclose(foo);
        invalid=3;
        frame_ready++;
        break;      
        }
      case STATE_SEQUENCE_REPEATED:
      case STATE_GOP:
      case STATE_PICTURE:
      case STATE_SLICE_1ST:
      case STATE_PICTURE_2ND:
        break;      
      default:
        break;
    }
  } while (!frame_ready);


  mpg->framenum++;
  if (invalid) mpg->end=invalid;
//  if (mpg->state==STATE_END || mpg->state==STATE_INVALID_END) mpg->end=1; //mpg->framenum=0, mpg->whereami=0;
  if (mpg->state==STATE_END) mpg->end=1;

}


#define blask(buf, index) buf[(index)*4+0]=buf[(index)*4+1]=buf[(index)*4+2]=buf[(index)*4+3]=0 

void glueMpeg2_get(Mpeg2 *mpg, Texture *dst, int sync_channel, float fps, int border, int skip_frames, int reset_on_error) {
  unsigned char *tmpbuf;
  int framecnt=0;
	float curtime;
	
#ifdef USE_FMODEX
	if(sync_channel==-1) // no audio sync
		curtime = timeGetTime();
	else // get time from audio channel
		curtime = fmod_gettick_channel(sync_channel)+(1000.0/fps);
#else
  curtime = timeGetTime();
#endif

  if (mpg->end==1) {
    glueMpeg2_reset(mpg);
  }
  
  if (mpg->prevtime==0) { 
    getframe(mpg, dst); 
    if (mpg->end==2) { glueMpeg2_reset(mpg); return; }
    if (mpg->end==3) { if (reset_on_error) glueMpeg2_reset(mpg); return; }    
  } 
  else if (curtime-mpg->prevtime < 1000.0/fps) return;
  else {
    int x;
    
    framecnt=(curtime-mpg->prevtime)/(1000.0/fps);
    if (skip_frames==0 && framecnt>1) framecnt=1;
    for (x=0; x<framecnt; x++) {
      getframe(mpg, dst);
      if (mpg->end==2) { glueMpeg2_reset(mpg); return; }
      if (mpg->end==3) { if (reset_on_error) glueMpeg2_reset(mpg); return; }
    }
  }
  
  if (mpg->prevtime==0) mpg->prevtime=curtime;
  else mpg->prevtime+=1000.0/fps*framecnt;
  //mpg->prevtime=timeGetTime();
  tmpbuf=tmpmalloc(dst->xres*dst->yres*4);
  
  if (mpg->info->display_fbuf) {
    int w, h;
      
    w=mpg->info->sequence->width;
    h=mpg->info->sequence->height;  
  
    if (dst->xres==w && dst->yres==h && (!border)) {
      glueReloadtexture(dst, mpg->info->display_fbuf->buf[0]);
    } else {
      int x;
      
      memset(tmpbuf, 0, dst->xres*dst->yres*4);
     
      if (dst->xres < w || dst->yres < h) 
        glueErrorf("erreur: ur texture too small for ur videoz (%ix%i vs %ix%i)", w, h, dst->xres, dst->yres);    

      for (x=0; x<h; x++) 
        memcpy(tmpbuf+x*dst->xres*4, mpg->info->display_fbuf->buf[0]+x*w*4, dst->xres*4);

      if (border) {
        for (x=0; x<w; x++) { blask(tmpbuf, x); blask(tmpbuf, x+(h-1)*dst->xres); }
        for (x=0; x<h; x++) { blask(tmpbuf, x*dst->xres); blask(tmpbuf, x*dst->xres+w-1); }
      }
 
      dst->scale.x=(float)w/dst->xres;
      dst->scale.y=(float)h/dst->yres;

      glueReloadtexture(dst, tmpbuf);
    }
  }
  
  tmpfree(tmpbuf);

}



void glueMpeg2_close(Mpeg2 *mpg) {
  if (mpg->stream_from_disk) fclose(mpg->f);
  mpeg2_close(mpg->decoder);
}




