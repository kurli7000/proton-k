
#include "glue3dx.h"
#include "gluewin.h"
#include "gluetinyfont.h"
#include "gluegen.h"
#include <windows.h>
#include <setupapi.h>
#include <malloc.h>
#include <ddk/hidpi.h>
#include <ddk/hidsdi.h>


#ifdef GLUE_USE_3DX
 

#define MAX_DEVICES 10
int nDevices = 0;
typedef struct _HidDevice {
	PSP_DEVICE_INTERFACE_DETAIL_DATA  pDevInterfaceDetailData;
	HANDLE handle;
	PHIDP_PREPARSED_DATA pPreparsedData;
	HIDP_CAPS capabilities;
	PHIDP_BUTTON_CAPS pInButCaps,pOutButCaps,pFeatButCaps;
	PHIDP_VALUE_CAPS pInValCaps,pOutValCaps,pFeatValCaps;
	char *buf;
	BOOL bGotTranslationVector, bGotRotationVector;
	int allDOFs[6];
} HidDevice;
HidDevice Devices[MAX_DEVICES];

int listIndex, waitListIndex = 0;
HANDLE *waitListHandles;
HidDevice **waitListItems;



void init3dx(void) {
	GUID hidGuid;
  

	HidD_GetHidGuid( &hidGuid );
	HDEVINFO hDevInfo = SetupDiGetClassDevs( &hidGuid, NULL, NULL, 
		                                     DIGCF_DEVICEINTERFACE | DIGCF_PRESENT | DIGCF_PROFILE );

	int memberIndex=0;
	SP_DEVICE_INTERFACE_DATA  DevInterfaceData;
	DevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	while(SetupDiEnumDeviceInterfaces( hDevInfo, NULL, &hidGuid, memberIndex, &DevInterfaceData ))
	{
		DWORD requiredSize;

		// First call, just gets the size so we can malloc enough memory for the detail struct
		SetupDiGetDeviceInterfaceDetail( hDevInfo, &DevInterfaceData, NULL, 0, &requiredSize, NULL );

		// Second call gets the data
		Devices[nDevices].pDevInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc( requiredSize );
		Devices[nDevices].pDevInterfaceDetailData->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );
		SetupDiGetDeviceInterfaceDetail( hDevInfo, 
										 &DevInterfaceData,
										 Devices[nDevices].pDevInterfaceDetailData,
										 requiredSize,
										 NULL,
										 NULL );

		// Open this device
		Devices[nDevices].handle = CreateFile(Devices[nDevices].pDevInterfaceDetailData->DevicePath,
												GENERIC_READ | GENERIC_WRITE,
												FILE_SHARE_READ | FILE_SHARE_WRITE,
												NULL,
												OPEN_EXISTING,
												FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
												NULL);
		if ( Devices[nDevices].handle == NULL)
		{
			glueErrorf("Error opening file \"%s\"\n",Devices[nDevices].pDevInterfaceDetailData->DevicePath);
		}

		// Get the preparsedData struct
		if (HidD_GetPreparsedData( Devices[nDevices].handle, &Devices[nDevices].pPreparsedData ) != TRUE)
		{
			glueError("Error getting PreparsedData\n");
		}

		if ( HidP_GetCaps( Devices[nDevices].pPreparsedData, &Devices[nDevices].capabilities ) != HIDP_STATUS_SUCCESS )
		{
			glueError("Error getting capabilities\n");
		}

		memberIndex++;
		if (++nDevices == MAX_DEVICES) {printf("overflow\n");--nDevices;}
	} // end of while(SetupDiEnumDeviceInterfaces) loop


	// Done with devInfo list.  Release it.
	SetupDiDestroyDeviceInfoList( hDevInfo );

	// Read data from each usagePage==1, usage==8 device
	waitListHandles = (HANDLE *)malloc(nDevices * sizeof(HANDLE ));
	waitListItems  = (HidDevice **)malloc(nDevices * sizeof(HidDevice*));
	
	for (listIndex=0; listIndex < nDevices; listIndex++)
	{
		if (Devices[listIndex].capabilities.UsagePage == 1 && Devices[listIndex].capabilities.Usage == 8)
		{
			// Add its handle and a pointer to the waitList
			waitListHandles[waitListIndex] = Devices[listIndex].handle;
			HidDevice *pDev = Devices + listIndex;
			waitListItems  [waitListIndex] = pDev;
			pDev->buf = (char *)malloc(pDev->capabilities.InputReportByteLength );
			ZeroMemory( pDev->buf, pDev->capabilities.InputReportByteLength );
			pDev->bGotTranslationVector = 
 			  pDev->bGotRotationVector    = FALSE;
			waitListIndex++;
		}
	}

  if (waitListIndex<1) glueError("didn't find any suitable 3d controller");

}





void get3dxdata(Vertex *translate, Vertex *rotate)
{
  DWORD nread, waitResult;
	BOOL bRun = TRUE;
  int i;
  static OVERLAPPED blah;
  
  translate->x = 0;
  translate->y = 0;
  translate->z = 0;
  rotate->x = 0;
  rotate->y = 0;
  rotate->z = 0;
  
	while (bRun) {
		
    // This only wakes for one of the HID devices.  Not sure why.
		//waitResult = WaitForMultipleObjects( waitListIndex, waitListHandles, FALSE, INFINITE);
		//waitResult = WaitForMultipleObjects( waitListIndex, waitListHandles, FALSE, 10);
    //if (waitResult == WAIT_TIMEOUT || waitResult == WAIT_FAILED) return;
    waitResult=0;

    // a HID device event
		if (waitResult < WAIT_OBJECT_0 + waitListIndex ) {
			int index = waitResult - WAIT_OBJECT_0;
			HidDevice *pDev = waitListItems[index];

      memset(&blah, 0, sizeof blah);
			if (ReadFile(pDev->handle, pDev->buf, pDev->capabilities.InputReportByteLength, &nread, &blah))
/*        || GetOverlappedResult(pDev->handle, &blah, &nread, FALSE))
        && nread==pDev->capabilities.InputReportByteLength)*/ {
        //GetOverlappedResult(pDev->handle, &blah, &nread, TRUE);
				
				//if (nread==0) return; // eof
				
        switch (pDev->buf[0])
				{
					case 0x01:  
						if (nread != 7) break; // something is wrong
						//printf("Device %d Got Translation vector\n", index);
						pDev->bGotTranslationVector = TRUE; 
						pDev->allDOFs[0] = (pDev->buf[1] & 0x000000ff) | ((int)pDev->buf[2]<<8 & 0xffffff00); 
						pDev->allDOFs[1] = (pDev->buf[3] & 0x000000ff) | ((int)pDev->buf[4]<<8 & 0xffffff00); 
						pDev->allDOFs[2] = (pDev->buf[5] & 0x000000ff) | ((int)pDev->buf[6]<<8 & 0xffffff00);
						break;

					case 0x02:  
						if (nread != 7) break; // something is wrong
						//printf("Device %d Got Rotation vector\n", index);
						pDev->bGotRotationVector = TRUE; 
						pDev->allDOFs[3] = (pDev->buf[1] & 0x000000ff) | ((int)pDev->buf[2]<<8 & 0xffffff00); 
						pDev->allDOFs[4] = (pDev->buf[3] & 0x000000ff) | ((int)pDev->buf[4]<<8 & 0xffffff00); 
						pDev->allDOFs[5] = (pDev->buf[5] & 0x000000ff) | ((int)pDev->buf[6]<<8 & 0xffffff00);
						break;

					case 0x03:  // Buttons (display most significant byte to least)
						//printf("Device %d Button mask: %.2x %.2x %.2x\n",index,(unsigned char)pDev->buf[3],(unsigned char)pDev->buf[2],(unsigned char)pDev->buf[1]);
						switch ( pDev->buf[1] )
						{
						case 0x01:  // Stop reading on button 1
							//bRun = FALSE;
							break;
						default:
							break;
						}
						break;

					default:
						break;
				}

				// Translation and Rotation values come in two different packets.  Wait until we have both of them.
				if ( pDev->bGotTranslationVector && pDev->bGotRotationVector ) {
					//pDev->bGotTranslationVector = pDev->bGotRotationVector = FALSE;
					//printf( "Device %d, all 6 DOF: T: (%d, %d, %d) R: (%d, %d, %d)\n", index, 
					//	pDev->allDOFs[0], pDev->allDOFs[1], pDev->allDOFs[2], pDev->allDOFs[3], pDev->allDOFs[4], pDev->allDOFs[5]);
//					glueDebugf_tiny("T: (%d, %d, %d)", pDev->allDOFs[0], pDev->allDOFs[1], pDev->allDOFs[2]);
//					glueDebugf_tiny("R: (%d, %d, %d)", pDev->allDOFs[3], pDev->allDOFs[4], pDev->allDOFs[5]);

					translate->x = pDev->allDOFs[0];
					translate->z = pDev->allDOFs[1];
					translate->y = -pDev->allDOFs[2];						
  				rotate->x = -pDev->allDOFs[3];
					rotate->z = -pDev->allDOFs[4];
					rotate->y = pDev->allDOFs[5];            						
	
					//ZeroMemory( pDev->buf, pDev->capabilities.InputReportByteLength);
					//bRun=FALSE;
				} 
			} else {
        CancelIo(pDev->handle);
        return;
      }
		}
	}  
}

void uninit3dx(void)
{
  int i;
	for (i=0; i<nDevices; i++)
	{
		// Free the preparsedData
		if (Devices[i].pPreparsedData)
			HidD_FreePreparsedData( Devices[i].pPreparsedData );

		// Close handles
		if (Devices[i].handle != INVALID_HANDLE_VALUE) 
			CloseHandle( Devices[i].handle );

		// Free pDevInterfaceDetailData
		if (Devices[i].pDevInterfaceDetailData)
			free ( Devices[i].pDevInterfaceDetailData );

		if (Devices[i].buf)
			free ( Devices[i].buf );
	}
}




static unsigned char camname[100];
#define MAX_STEPS 5000
static Vertex xtab[MAX_STEPS];
static Vertex ytab[MAX_STEPS];
static Vertex ztab[MAX_STEPS];
static Vertex postab[MAX_STEPS];
static int ptc=0;
static int fps=1;


Camera3dx *glueCamera3dx(
  Vertex initpos, 
  Vertex inittgt, 
  float initfov,
  Vertex initrvelocity, 
  Vertex inittvelocity, 
  float rspeed, 
  float tspeed,
  unsigned char *name) {
  
  Camera3dx *c3dx=malloc(sizeof(Camera3dx));
  
  c3dx->pos = initpos;
  c3dx->z.x = inittgt.x - initpos.x;
  c3dx->z.y = inittgt.y - initpos.y;
  c3dx->z.z = inittgt.z - initpos.z;
  normalize(&c3dx->z);
  c3dx->y = new_v(0, 1, 0);
  c3dx->x = cross(&c3dx->y, &c3dx->z);
  normalize(&c3dx->x);
  c3dx->y = cross(&c3dx->x, &c3dx->z);
  normalize(&c3dx->y);
   
  c3dx->fov = initfov;
  c3dx->rvelocity = initrvelocity;
  c3dx->tvelocity = inittvelocity;
  c3dx->rspeed = rspeed;
  c3dx->tspeed = tspeed;
  
  sprintf(camname, name);
  return c3dx;
}






void saveposses(Camera3dx *c3dx) {
  static int startt;
  int t, i;
  
  ONCE (
    startt=timeGetTime();
  );
  
  t=timeGetTime()-startt;
  i=t*fps/1000;
  xtab[i]=c3dx->x;
  ytab[i]=c3dx->y;
  ztab[i]=c3dx->z;
  postab[i]=c3dx->pos;
  if (i>ptc) ptc=i;
  
  if (i<0 || i>MAX_STEPS) glueError("recordtable out of bounds thingy");
}



void glueWrite3dxfile(void) {
  int x;
  FILE *f;
  unsigned char filename[100];
  
  if (ptc>0) {
    sprintf(filename, "%s_path.inc", camname);
    f=fopen(filename, "w");
  
    fprintf(f, "int %s_pcnt=%i;\n\n", camname, ptc);
    fprintf(f, "static Splinept %s_postab[]={\n", camname);
    for (x=0; x<ptc; x++) {
      fprintf(f, "  {%5.2f, {%7.3f, %7.3f, %7.3f}},\n", (float)x/fps, postab[x].x, postab[x].y, postab[x].z);
    }
    fprintf(f, "};\n\n");
  
    fprintf(f, "static Splinept %s_xtab[]={\n", camname);
    for (x=0; x<ptc; x++) {
      fprintf(f, "  {%5.2f, {%7.3f, %7.3f, %7.3f}},\n", (float)x/fps, xtab[x].x, xtab[x].y, xtab[x].z);
    }
    fprintf(f, "};\n\n");
  
    fprintf(f, "static Splinept %s_ytab[]={\n", camname);
    for (x=0; x<ptc; x++) {
      fprintf(f, "  {%5.2f, {%7.3f, %7.3f, %7.3f}},\n", (float)x/fps, ytab[x].x, ytab[x].y, ytab[x].z);
    }
    fprintf(f, "};\n\n");
  
    fprintf(f, "static Splinept %s_ztab[]={\n", camname);
    for (x=0; x<ptc; x++) {
      fprintf(f, "  {%5.2f, {%7.3f, %7.3f, %7.3f}},\n", (float)x/fps, ztab[x].x, ztab[x].y, ztab[x].z);
    }
    fprintf(f, "};\n\n");
  
    fclose(f);
  }
}




void glue3dxcam_exact(Camera3dx *c3dx) {
  static int prevt=0, t;
  float td;
  Vertex rot, trans;
  float tp;
  
  t = timeGetTime();
  if (prevt==0) td=0;
  else td = (t-prevt)/1000.0;
  prevt = t;
  
  get3dxdata(&trans, &rot);
  tp=c3dx->tspeed * td;
 
  c3dx->pos.x += -(c3dx->x.x * trans.x + c3dx->y.x * trans.y + c3dx->z.x * trans.z) * tp;
  c3dx->pos.y += -(c3dx->x.y * trans.x + c3dx->y.y * trans.y + c3dx->z.y * trans.z) * tp;
  c3dx->pos.z += -(c3dx->x.z * trans.x + c3dx->y.z * trans.y + c3dx->z.z * trans.z) * tp;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(rot.x * c3dx->rspeed * td, c3dx->x.x, c3dx->x.y, c3dx->x.z);
  glRotatef(rot.y * c3dx->rspeed * td, c3dx->y.x, c3dx->y.y, c3dx->y.z);
  glRotatef(rot.z * c3dx->rspeed * td, c3dx->z.x, c3dx->z.y, c3dx->z.z); 
  c3dx->x=glueMultmatrix(&c3dx->x);
  c3dx->y=glueMultmatrix(&c3dx->y);
  c3dx->z=glueMultmatrix(&c3dx->z);
  glPopMatrix();
  
  normalize(&c3dx->x);
  normalize(&c3dx->y);
  normalize(&c3dx->z);

  saveposses(c3dx);
}

void glue3dxcam_smooth(Camera3dx *c3dx, float change) {
  static int prevt=0, t;
  float td;
  Vertex rot, trans;
  float tp;
  Vertex xx, yy, zz;
  
  t = timeGetTime();
  if (prevt==0) td=0;
  else td = (t-prevt)/1000.0;
  prevt = t;
  
  get3dxdata(&trans, &rot);
  tp=c3dx->tspeed * td;
  
 
  c3dx->tvelocity.x = c3dx->tvelocity.x * (1-change) -(c3dx->x.x * trans.x + c3dx->y.x * trans.y + c3dx->z.x * trans.z) * tp * change;
  c3dx->tvelocity.y = c3dx->tvelocity.y * (1-change) -(c3dx->x.y * trans.x + c3dx->y.y * trans.y + c3dx->z.y * trans.z) * tp * change;
  c3dx->tvelocity.z = c3dx->tvelocity.z * (1-change) -(c3dx->x.z * trans.x + c3dx->y.z * trans.y + c3dx->z.z * trans.z) * tp * change;

  c3dx->pos.x += c3dx->tvelocity.x;
  c3dx->pos.y += c3dx->tvelocity.y;
  c3dx->pos.z += c3dx->tvelocity.z;

  c3dx->rvelocity.x = c3dx->rvelocity.x * (1-change) + rot.x * change;
  c3dx->rvelocity.y = c3dx->rvelocity.y * (1-change) + rot.y * change;
  c3dx->rvelocity.z = c3dx->rvelocity.z * (1-change) + rot.z * change;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(c3dx->rvelocity.x * c3dx->rspeed * td, c3dx->x.x, c3dx->x.y, c3dx->x.z);
  glRotatef(c3dx->rvelocity.y * c3dx->rspeed * td, c3dx->y.x, c3dx->y.y, c3dx->y.z);
  glRotatef(c3dx->rvelocity.z * c3dx->rspeed * td, c3dx->z.x, c3dx->z.y, c3dx->z.z); 
  c3dx->x=glueMultmatrix(&c3dx->x);
  c3dx->y=glueMultmatrix(&c3dx->y);
  c3dx->z=glueMultmatrix(&c3dx->z);
  glPopMatrix();
  
  normalize(&c3dx->x);
  normalize(&c3dx->y);
  normalize(&c3dx->z);
  
  saveposses(c3dx);
}







#endif


void glueC3dx2cam(Camera3dx *c3dx, float t, Splinept *postab, Splinept *xtab, Splinept *ytab, Splinept *ztab, int pcnt) {
  float m[16];
  float fov;
  Vertex xx, yy, zz, pos;
  
  if (pcnt>0) {
    xx=glueSpline(t, xtab, pcnt);
    yy=glueSpline(t, ytab, pcnt);
    zz=glueSpline(t, ztab, pcnt);
    pos=glueSpline(t, postab, pcnt);
    fov=60;
  } else {
    xx=c3dx->x;
    yy=c3dx->y;
    zz=c3dx->z;
    pos=c3dx->pos;
    fov=c3dx->fov;
  }
  
  glueSetmatrices3d(fov, new_v(0, 0, 0), 0.1, 1000.0, 0, 0);
  
  m[0]=xx.x;  m[1]=yy.x;  m[2]=zz.x;  m[3]=0;
  m[4]=xx.y;  m[5]=yy.y;  m[6]=zz.y;  m[7]=0;
  m[8]=xx.z;  m[9]=yy.z;  m[10]=zz.z; m[11]=0;
  m[12]=0;    m[13]=0;    m[14]=0;    m[15]=1;
  
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(m);
  glTranslatef(pos.x, pos.y, pos.z);
}


