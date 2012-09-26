
#include <sal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzip.h"
#include "zip.h"
#include "menu.h"
#include "snes9x.h"
#include "memmap.h"
#include "apu.h"
#include "gfx.h"
#include "soundux.h"
#include "snapshot.h"

#define SNES_SCREEN_WIDTH  256
#define SNES_SCREEN_HEIGHT 192

static struct MENU_OPTIONS mMenuOptions;
static int mEmuScreenHeight;
static int mEmuScreenWidth;
static char mRomName[SAL_MAX_PATH]={""};
static char mSystemDir[SAL_MAX_PATH]=SNES_SYSTEM_DIR;
static u32 mLastRate=0;

static s8 mFpsDisplay[40]={""};
static u32 mFps=0;
static u32 mLastTimer=0;
static u32 mEnterMenu=0;

static int S9xCompareSDD1IndexEntries (const void *p1, const void *p2)
{
    return (*(uint32 *) p1 - *(uint32 *) p2);
}

extern "C"
{

void S9xExit ()
{
}

void S9xGenerateSound (void)
{
	S9xMessage (0,0,"generate sound");
	return;
}

void S9xSetPalette ()
{

}

void S9xExtraUsage ()
{
}
	
void S9xParseArg (char **argv, int &index, int argc)
{	
}

bool8 S9xOpenSnapshotFile (const char *fname, bool8 read_only, STREAM *file)
{
	if (read_only)
	{
		if (*file = OPEN_STREAM(fname,"rb")) 
			return(TRUE);
	}
	else
	{
		if (*file = OPEN_STREAM(fname,"w+b")) 
			return(TRUE);
	}

	return (FALSE);	
}
	
void S9xCloseSnapshotFile (STREAM file)
{
	CLOSE_STREAM(file);
}

void S9xMessage (int /* type */, int /* number */, const char *message)
{
	//MenuMessageBox("PocketSnes has encountered an error",(s8*)message,"",MENU_MESSAGE_BOX_MODE_PAUSE);
}

void erk (void)
{
      S9xMessage (0,0, "Erk!");
}

char *osd_GetPackDir(void)
{
      S9xMessage (0,0,"get pack dir");
      return ".";
}

const char *S9xGetSnapshotDirectory(void)
{
	S9xMessage (0,0,"get snapshot dir");
	return ".";
}

void S9xLoadSDD1Data (void)
{

}

   

bool8_32 S9xInitUpdate ()
{
	if(mMenuOptions.fullScreen)	GFX.Screen = (uint8 *) sal_VideoGetBuffer();
	else				GFX.Screen = (uint8 *) sal_VideoGetBuffer()+(320-SNES_WIDTH)+((240-SNES_HEIGHT)*320);

	return (TRUE);
}

bool8_32 S9xDeinitUpdate (int Width, int Height, bool8_32)
{
	u32 newTimer;
	if (mMenuOptions.showFps) 
	{
		mFps++;
		newTimer=sal_TimerRead();
		if(newTimer-mLastTimer>60)
		{
			mLastTimer=newTimer;
			sprintf(mFpsDisplay,"FPS: %d",mFps);
			mFps=0;
		}
		
		sal_VideoDrawRect(0,0,8*8,8,SAL_RGB(0,0,0));
		sal_VideoPrint(0,0,mFpsDisplay,SAL_RGB(31,31,31));
	}

	sal_VideoFlip(0);
}

const char *S9xGetFilename (const char *ex)
{
      	static char filename [SAL_MAX_PATH];
      	char dir [SAL_MAX_PATH];
      	char fname [SAL_MAX_PATH];
      	char ext [SAL_MAX_PATH];

	sal_DirectorySplitFilename(Memory.ROMFilename, dir, fname, ext);
      	strcpy (filename, S9xGetSnapshotDirectory ());
	sal_DirectoryCombine(filename,fname);
      	strcat (filename, ex);

      	return (filename);
}


uint32 S9xReadJoypad (int which1)
{
	uint32 val=0x80000000;

	if (which1 != 0) return val;

	u32 joy = sal_InputPoll();
	
	if ((joy & SAL_INPUT_SELECT)&&(joy & SAL_INPUT_START))	
	{
		mEnterMenu = 1;		
		return val;
	}

	if (joy & SAL_INPUT_Y) val |= SNES_Y_MASK;
	if (joy & SAL_INPUT_A) val |= SNES_A_MASK;
	if (joy & SAL_INPUT_B) val |= SNES_B_MASK;
	if (joy & SAL_INPUT_X) val |= SNES_X_MASK;
		
	if (joy & SAL_INPUT_UP) 	val |= SNES_UP_MASK;
	if (joy & SAL_INPUT_DOWN) 	val |= SNES_DOWN_MASK;
	if (joy & SAL_INPUT_LEFT) 	val |= SNES_LEFT_MASK;
	if (joy & SAL_INPUT_RIGHT)	val |= SNES_RIGHT_MASK;
	if (joy & SAL_INPUT_START) 	val |= SNES_START_MASK;
	if (joy & SAL_INPUT_SELECT) 	val |= SNES_SELECT_MASK;
	if (joy & SAL_INPUT_L) 		val |= SNES_TL_MASK;
	if (joy & SAL_INPUT_R) 		val |= SNES_TR_MASK;
	
	
      
	return val;
}

bool8 S9xReadMousePosition (int /* which1 */, int &/* x */, int & /* y */,
		    uint32 & /* buttons */)
{
	S9xMessage (0,0,"read mouse");
	return (FALSE);
}

bool8 S9xReadSuperScopePosition (int & /* x */, int & /* y */,
				 uint32 & /* buttons */)
{
      S9xMessage (0,0,"read scope");
      return (FALSE);
}

const char *S9xGetFilenameInc (const char *e)
{
     S9xMessage (0,0,"get filename inc");
     return e;
}

void S9xSyncSpeed(void)
{
      //S9xMessage (0,0,"sync speed");
}

const char *S9xBasename (const char *f)
{
      const char *p;

      S9xMessage (0,0,"s9x base name");

      if ((p = strrchr (f, '/')) != NULL || (p = strrchr (f, '\\')) != NULL)
         return (p + 1);

      return (f);
}



void S9xSaveSRAM (void)
{
	char path[SAL_MAX_PATH];
	
	if (CPU.SRAMModified)
	{
		strcpy(path,mSystemDir);
		sal_DirectoryCombine(path,(s8*)S9xGetFilename (".srm"));
		Memory.SaveSRAM (path);
		sync();
	}
}



}

bool8_32 S9xOpenSoundDevice(int a, unsigned char b, int c)
{

}

void S9xAutoSaveSRAM (void)
{
	//since I can't sync the data, there is no point in even writing the data
	//out at this point.  Instead I'm now saving the data as the users enter the menu.
	//Memory.SaveSRAM (S9xGetFilename (".srm"));
	//sync();  can't sync when emulator is running as it causes delays
}

void S9xLoadSRAM (void)
{
	char path[SAL_MAX_PATH];

	strcpy(path,mSystemDir);
	sal_DirectoryCombine(path,(s8*)S9xGetFilename (".srm"));
	Memory.LoadSRAM (path);
}

static
int RunSound()
{
  	int skip=0, done=0, doneLast=0,aim=0,i;

	sal_AudioInit(mMenuOptions.soundRate, 16, 1, 60);
	Settings.APUEnabled = 1;
	Settings.NextAPUEnabled = Settings.APUEnabled;					
	Settings.SoundPlaybackRate=mMenuOptions.soundRate;
	Settings.SixteenBitSound=true;
	Settings.Stereo=1;
	S9xInitSound (mMenuOptions.soundRate,1,sal_AudioGetBufferSize());
	S9xSetPlaybackRate(mMenuOptions.soundRate);
	S9xSetSoundMute (FALSE);
  	while(!mEnterMenu) 
  	{
		for (i=0;i<10;i++)
		{
			aim=sal_AudioGetPrevBufferIndex();
			if (done!=aim)
			{
				doneLast=done;
				done=sal_AudioGetNextBufferIndex(done);
				if(mMenuOptions.frameSkip==0) //Auto
				{
					if(done==aim) IPPU.RenderThisFrame=TRUE; // Render last frame
					else          IPPU.RenderThisFrame=FALSE; // lagged behind, so skip

				}
				else
				{
					skip--;
					if(skip<=0)
					{
						IPPU.RenderThisFrame=TRUE;
						skip=mMenuOptions.frameSkip;
					}
					else
					{
						IPPU.RenderThisFrame=FALSE;
					}
				}
		
				//Run SNES for one glorious frame
		    		S9xMainLoop ();
				S9xMixSamples((uint8*)sal_AudioGetBuffer(doneLast), sal_AudioGetSampleCount());
			}
			if (done==aim) break; // Up to date now
			if (mEnterMenu) break;
		}
		done=aim; // Make sure up to date
  	}

	sal_AudioClose();
	mEnterMenu=0;
	return mEnterMenu;

}

static
int RunNoSound()
{
  	int skip=0, done=0, doneLast=0,aim=0,i;
	Settings.APUEnabled = 0;
	Settings.NextAPUEnabled = Settings.APUEnabled;					
	S9xSetSoundMute (TRUE);
	sal_TimerInit(60);
	done=sal_TimerRead()-1;
  	while(!mEnterMenu) 
  	{
		for (i=0;i<10;i++)
		{
			aim=sal_TimerRead();
			if (done!=aim)
			{
				done++;
				if(mMenuOptions.frameSkip==0) //Auto
				{
					if(done==aim) IPPU.RenderThisFrame=TRUE; // Render last frame
					else          IPPU.RenderThisFrame=FALSE; // lagged behind, so skip

				}
				else
				{
					skip--;
					if(skip<=0)
					{
						IPPU.RenderThisFrame=TRUE;
						skip=mMenuOptions.frameSkip;
					}
					else
					{
						IPPU.RenderThisFrame=FALSE;
					}
				}
		
				//Run SNES for one glorious frame
		    		S9xMainLoop ();
			}
			if (done==aim) break; // Up to date now
			if (mEnterMenu) break;
		}
		done=aim; // Make sure up to date
  	}
	mEnterMenu=0;
	return mEnterMenu;

}

static 
int SnesRomLoad()
{
	char filename[SAL_MAX_PATH+1];
	int check;
	char text[256];
	FILE *stream=NULL;
  
    	MenuMessageBox("Loading Rom...",mRomName,"",MENU_MESSAGE_BOX_MODE_MSG);

	if (!Memory.LoadROM (mRomName))
	{
		MenuMessageBox("Loading Rom...",mRomName,"FAILED!!!!",MENU_MESSAGE_BOX_MODE_PAUSE);
		return SAL_ERROR;
	}
	
	MenuMessageBox("Loading Rom...OK!",mRomName,"",MENU_MESSAGE_BOX_MODE_MSG);

	S9xReset();
	S9xResetSound(1);
	S9xLoadSRAM();
	return SAL_OK;
}

int SnesInit()
{
	ZeroMemory (&Settings, sizeof (Settings));

	Settings.JoystickEnabled = FALSE;
	Settings.SoundPlaybackRate = 22050;
	Settings.Stereo = FALSE;
	Settings.SoundBufferSize = 0;
	Settings.CyclesPercentage = 100;
	Settings.DisableSoundEcho = FALSE;
	Settings.APUEnabled = FALSE;
	Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
	Settings.SkipFrames = AUTO_FRAMERATE;
	Settings.Shutdown = Settings.ShutdownMaster = TRUE;
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.FrameTime = Settings.FrameTimeNTSC;
	Settings.DisableSampleCaching = FALSE;
	Settings.DisableMasterVolume = FALSE;
	Settings.Mouse = FALSE;
	Settings.SuperScope = FALSE;
	Settings.MultiPlayer5 = FALSE;
	//	Settings.ControllerOption = SNES_MULTIPLAYER5;
	Settings.ControllerOption = 0;
	
	Settings.ForceTransparency = FALSE;
	Settings.Transparency = FALSE;
	Settings.SixteenBit = TRUE;
	
	Settings.SupportHiRes = FALSE;
	Settings.NetPlay = FALSE;
	Settings.ServerName [0] = 0;
	Settings.AutoSaveDelay = 30;
	Settings.ApplyCheats = FALSE;
	Settings.TurboMode = FALSE;
	Settings.TurboSkipFrames = 15;
	Settings.ThreadSound = FALSE;
	Settings.SoundSync = FALSE;
	//Settings.NoPatch = true;		

	GFX.Pitch = 320 * 2;
	GFX.RealPitch = 320 * 2;
	GFX.Screen = (uint8 *) sal_VideoGetBuffer();
	
	GFX.SubScreen = (uint8 *)malloc(GFX.RealPitch * 480 * 2); 
	GFX.ZBuffer =  (uint8 *)malloc(GFX.RealPitch * 480 * 2); 
	GFX.SubZBuffer = (uint8 *)malloc(GFX.RealPitch * 480 * 2);
	GFX.Delta = (GFX.SubScreen - GFX.Screen) >> 1;
	GFX.PPL = GFX.Pitch >> 1;
	GFX.PPLx2 = GFX.Pitch;
	GFX.ZPitch = GFX.Pitch >> 1;
	
	if (Settings.ForceNoTransparency)
         Settings.Transparency = FALSE;

	if (Settings.Transparency)
         Settings.SixteenBit = TRUE;

	Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;

	if (!Memory.Init () || !S9xInitAPU())
	{
		S9xMessage (0,0,"Failed to init memory");
		return SAL_ERROR;
	}

	//S9xInitSound ();
	
	//S9xSetRenderPixelFormat (RGB565);
	S9xSetSoundMute (TRUE);

	if (!S9xGraphicsInit ())
	{
         	S9xMessage (0,0,"Failed to init graphics");
		return SAL_ERROR;
	}

	return SAL_OK;
}

void _makepath (char *path, const char *, const char *dir,
	const char *fname, const char *ext)
{
	if (dir && *dir)
	{
		strcpy (path, dir);
		strcat (path, "/");
	}
	else
	*path = 0;
	strcat (path, fname);
	if (ext && *ext)
	{
		strcat (path, ".");
		strcat (path, ext);
	}
}

void _splitpath (const char *path, char *drive, char *dir, char *fname,
	char *ext)
{
	*drive = 0;

	char *slash = strrchr ((char*)path, '/');
	if (!slash)
		slash = strrchr ((char*)path, '\\');

	char *dot = strrchr ((char*)path, '.');

	if (dot && slash && dot < slash)
		dot = NULL;

	if (!slash)
	{
		strcpy (dir, "");
		strcpy (fname, path);
		if (dot)
		{
			*(fname + (dot - path)) = 0;
			strcpy (ext, dot + 1);
		}
		else
			strcpy (ext, "");
	}
	else
	{
		strcpy (dir, path);
		*(dir + (slash - path)) = 0;
		strcpy (fname, slash + 1);
		if (dot)
		{
			*(fname + (dot - slash) - 1) = 0;
			strcpy (ext, dot + 1);
		}
		else
			strcpy (ext, "");
	}
} 

// save state file I/O
int  (*statef_open)(const char *fname, const char *mode);
int  (*statef_read)(void *p, int l);
int  (*statef_write)(void *p, int l);
void (*statef_close)();
static FILE  *state_file = 0;
static char state_filename[SAL_MAX_PATH];
static char *state_mem = NULL;
static int state_mem_pos = 0;
static int state_mem_size=0;
static int state_mode = 0;
static int open_mode = 0;

int check_zip(char *filename)
{
    uint8 buf[2];
    FILE *fd = NULL;
    fd = (FILE*)fopen(filename, "rb");
    if(!fd) return (0);
    fread(buf, 1, 2, fd);
    fclose(fd);
    if(memcmp(buf, "PK", 2) == 0) return (1);
    return (0);
}

static char *load_archive(char *filename, int *file_size)
{
    int size = 0;
    char *buf = NULL;
    char text[128];	

    unzFile fd = NULL;
    unz_file_info info;
    int ret = 0;
         
	/* Attempt to open the archive */
	fd = unzOpen(filename);
	if(!fd)
	{
		printf("Failed to open archive\r\n");
		return NULL;
	}

	/* Go to first file in archive */
	ret = unzGoToFirstFile(fd);
	if(ret != UNZ_OK)
	{
		printf("Failed to find first file in zip\r\n");
		unzClose(fd);
		return NULL;
	}

	ret = unzGetCurrentFileInfo(fd, &info, NULL, 0, NULL, 0, NULL, 0);
	if(ret != UNZ_OK)
	{
		printf("Failed to zip info\r\n");
        unzClose(fd);
        return NULL;
	}

	/* Open the file for reading */
	ret = unzOpenCurrentFile(fd);
	if(ret != UNZ_OK)
	{
	    printf("Failed to read file\r\n");
		unzClose(fd);
		return NULL;
	}

	/* Allocate file data buffer */
	size = info.uncompressed_size;
	buf=(char*)malloc(size);
	if(!buf)
	{
		printf("Failed to malloc zip buffer\r\n");
		unzClose(fd);
		return NULL;
	}
	
	/* Read (decompress) the file */
	ret = unzReadCurrentFile(fd, buf, info.uncompressed_size);
	if(ret != info.uncompressed_size)
	{
		free(buf);
	    printf("File failed to uncompress fully\r\n");
	    unzCloseCurrentFile(fd);
		unzClose(fd);
		return NULL;
	}

	/* Close the current file */
	ret = unzCloseCurrentFile(fd);
	if(ret != UNZ_OK)
	{
		free(buf);
	    printf("Failed to close file in zip\r\n");
	    unzClose(fd);
		return NULL;
	}

	/* Close the archive */
	ret = unzClose(fd);
	if(ret != UNZ_OK)
	{
		free(buf);
	    printf("Failed to close zip\r\n");
	    return NULL;
	}

	/* Update file size and return pointer to file data */
	*file_size = size;
	return buf;
}

static int save_archive(char *filename, char *buffer, int size)
{
    uint8 *buf = NULL;
    char text[128]="";	
    zipFile fd = NULL;
    int ret = 0;
    fd=zipOpen(filename,0);
    if(!fd)
    {
       printf("Failed to create zip\r\n");
       return (0);
    }

    ret=zipOpenNewFileInZip(fd,"SNAPSHOT",
			    NULL,
				NULL,0,
			    NULL,0,
			    NULL,
			    Z_DEFLATED,
			    Z_BEST_COMPRESSION);
			    
    if(ret != ZIP_OK)
    {
       zipClose(fd,NULL);
       printf("Failed to create file in zip\r\n");
       return (0);    
    }

    ret=zipWriteInFileInZip(fd,buffer,size);
    if(ret != ZIP_OK)
    {
      zipCloseFileInZip(fd);
      zipClose(fd,NULL);
      printf("Failed to write file in zip\r\n");
      return (0);
    }

    ret=zipCloseFileInZip(fd);
    if(ret != ZIP_OK)
    {
      zipClose(fd,NULL);
      printf("Failed to close file in zip\r\n");
      return (0);
    }

    ret=zipClose(fd,NULL);
    if(ret != ZIP_OK)
    {
      printf("Failed to close zip\r\n");
      return (0);
    }
	
    return(1);
}

int state_unc_open(const char *fname, const char *mode)
{
	//mode = "wb"  or "rb"
	//If mode is write then create a new buffer to hold written data
	//when file is closed buffer will be compressed to zip file and then freed
	if(mode[0]=='r')
	{
		//Read mode requested
		if(check_zip((char*)fname))
		{
			//File is a zip, so uncompress
			state_mode = 1; //zip mode
			open_mode = 0;
			state_mem=load_archive((char*)fname,&state_mem_size);
			if(!state_mem) return 0;
			state_mem_pos=0;
			strcpy(state_filename,fname);
			return 1;
		}
		else
		{
			state_mode = 0; //normal file mode
			state_file = fopen(fname, mode);
			return (int) state_file;
		}
	}
	else
	{
		//Write mode requested. Zip only option
		open_mode = 1;
		state_mode = 1; //normal file mode
		state_mem=(char*)malloc(200);
		state_mem_size=200;
		state_mem_pos = 0;
		strcpy(state_filename,fname);
		return 1;
	}
}

int state_unc_read(void *p, int l)
{
	if(state_mode==0)
	{
		return fread(p, 1, l, state_file);
	}
	else
	{
		
		if((state_mem_pos+l)>state_mem_size)
		{
			//Read requested that exceeded memory limits
			return 0;
		}
		else
		{
			memcpy(p,state_mem+state_mem_pos,l);
			state_mem_pos+=l;
		}
		return l;
	}
}

int state_unc_write(void *p, int l)
{
	if(state_mode==0)
	{
		return fwrite(p, 1, l, state_file);
	}
	else
	{
		if((state_mem_pos+l)>state_mem_size)
		{
			//Write will exceed current buffer, re-alloc buffer and continue
			state_mem=(char*)realloc(state_mem,state_mem_pos+l);
			state_mem_size=state_mem_pos+l;
		}
		//Now do write
		memcpy(state_mem+state_mem_pos,p,l);
		state_mem_pos+=l;
		return l;
	}
}

void state_unc_close()
{
	if(state_mode==0)
	{
		fclose(state_file);
	}
	else
	{
		if (open_mode == 1)
			save_archive(state_filename,state_mem,state_mem_size);
		free(state_mem);
		state_mem=NULL;
		state_mem_size=0;
		state_mem_pos=0;
		state_filename[0]=0;
	}
}

extern "C"
{

int mainEntry(int argc, char* argv[])
{
	int ref = 0;

	s32 event=EVENT_NONE;

	sal_Init();
	sal_VideoInit(16,SAL_RGB(0,0,0),60);

	MenuInit(mSystemDir);

	if (argc <= 0) 
	{
		MenuMessageBox("No filename paramter supplied","POCKETSNES will now exit","Press any button to continue", MENU_MESSAGE_BOX_MODE_PAUSE);
		sal_Reset();
		return 0;
	}

	//Record ROM name
 	strcpy(mRomName, argv[0]);

	if(SnesInit() == SAL_ERROR)
	{
		sal_Reset();
		return 0;
	}

	while(1)
	{
		event=MenuRun(&mMenuOptions, mRomName, mSystemDir);

		if(event==EVENT_LOAD_ROM)
		{
			if(SnesRomLoad() == SAL_ERROR) 
			{
				MenuMessageBox("Failed to load rom",mRomName,"Press any button to continue", MENU_MESSAGE_BOX_MODE_PAUSE);
				sal_Reset();
		    		return 0;
			}
			else
			{
				event=EVENT_RUN_ROM;
		  	}
		}

		if(event==EVENT_RESET_ROM)
		{
			S9xReset();
			event=EVENT_RUN_ROM;
		}

		if(event==EVENT_RUN_ROM)
		{
			if(mMenuOptions.fullScreen)
			{
				sal_VideoSetScaling(SNES_WIDTH,SNES_HEIGHT);
			}

			if(mMenuOptions.transparency)	Settings.Transparency = TRUE;
			else Settings.Transparency = FALSE;

			sal_AudioSetVolume(mMenuOptions.volume,mMenuOptions.volume);
			sal_CpuSpeedSet(mMenuOptions.cpuSpeed);	
			sal_VideoClear(0);
			if(mMenuOptions.soundEnabled) 	
				RunSound();
			else	RunNoSound();

			if(mMenuOptions.autoSaveSram)
			{
				MenuMessageBox("","","Saving SRAM...",MENU_MESSAGE_BOX_MODE_MSG);
				S9xSaveSRAM();
			}
			
			event=EVENT_NONE;
		}

		if(event==EVENT_EXIT_APP) break;	
	}

	sal_Reset();
  	return 0;
}

}






