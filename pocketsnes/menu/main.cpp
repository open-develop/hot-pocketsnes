
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
#include "savestateio.h"

#define SNES_SCREEN_WIDTH  256
#define SNES_SCREEN_HEIGHT 192

static struct MENU_OPTIONS mMenuOptions;
static int mEmuScreenHeight;
static int mEmuScreenWidth;
static char mRomName[SAL_MAX_PATH]={""};
static char mSystemDir[SAL_MAX_PATH]=SNES_SYSTEM_DIR;
static u32 mLastRate=0;

static s8 mFpsDisplay[16]={""};
static s8 mVolumeDisplay[16]={""};
static s8 mQuickStateDisplay[16]={""};
static u32 mFps=0;
static u32 mLastTimer=0;
static u32 mEnterMenu=0;
static u32 mLoadRequested=0;
static u32 mSaveRequested=0;
static u32 mQuickStateTimer=0;
static u32 mVolumeTimer=0;
static u32 mVolumeDisplayTimer=0;
static u32 mInMenu=0;

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

void S9xLoadSDD1Data (void)
{

}

   

bool8_32 S9xInitUpdate ()
{
	if(mInMenu) return (TRUE);
	if(mMenuOptions.fullScreen)	GFX.Screen = (uint8 *) sal_VideoGetBuffer();
	else				GFX.Screen = (uint8 *) sal_VideoGetBuffer()+(320-SNES_WIDTH)+((240-SNES_HEIGHT)*320);

	return (TRUE);
}

bool8_32 S9xDeinitUpdate (int Width, int Height, bool8_32)
{
	if(mInMenu) return TRUE;

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

	if(mVolumeDisplayTimer>0)
	{
		sal_VideoDrawRect(100,0,8*8,8,SAL_RGB(0,0,0));
		sal_VideoPrint(100,0,mVolumeDisplay,SAL_RGB(31,31,31));
	}

	if(mQuickStateTimer>0)
	{
		sal_VideoDrawRect(200,0,8*8,8,SAL_RGB(0,0,0));
		sal_VideoPrint(200,0,mQuickStateDisplay,SAL_RGB(31,31,31));
	}

	sal_VideoFlip(0);
}

const char *S9xGetFilename (const char *ex)
{
      	char dir [SAL_MAX_PATH];
      	char fname [SAL_MAX_PATH];
      	char ext [SAL_MAX_PATH];

	sal_DirectorySplitFilename(Memory.ROMFilename, dir, fname, ext);
	sal_DirectoryCombine(dir,fname);
      	strcat (dir, ex);

      	return (dir);
}

static u8 *mTempState=NULL;
static
void LoadStateMem()
{
	SetSaveStateIoModeMemory(&mTempState);
	S9xUnfreezeGame("blah");
}

static 
void SaveStateMem()
{
	SetSaveStateIoModeMemory(&mTempState);

	S9xFreezeGame("blah");
}

void HandleQuickStateRequests()
{
	if(mVolumeTimer>0) mVolumeTimer--;
	if(mVolumeDisplayTimer>0) mVolumeDisplayTimer--;

	if(mQuickStateTimer>0)
	{
		mQuickStateTimer--;
		return;
	}

	if(mSaveRequested)
	{
		mSaveRequested=0;
		SaveStateMem();
		mQuickStateTimer = 60;
	}

	if(mLoadRequested)
	{
		mLoadRequested=0;
		LoadStateMem();
		mQuickStateTimer = 60;
	}

	
}
uint32 S9xReadJoypad (int which1)
{
	uint32 val=0x80000000;
	if (mInMenu) return val;
	if (which1 != 0) return val;

	u32 joy = sal_InputPoll();
	
	if ((joy & SAL_INPUT_SELECT)&&(joy & SAL_INPUT_START))	
	{
		mEnterMenu = 1;		
		return val;
	}

	
	if ((joy & SAL_INPUT_L)&&(joy & SAL_INPUT_R)&&(joy & SAL_INPUT_LEFT))
	{
		if (mQuickStateTimer==0)
		{
			mSaveRequested=1;
			strcpy(mQuickStateDisplay,"Saved!");
		}
		return val;
	}

	if ((joy & SAL_INPUT_L)&&(joy & SAL_INPUT_R)&&(joy & SAL_INPUT_RIGHT))
	{
		if (mQuickStateTimer==0)
		{
			mLoadRequested=1;
			strcpy(mQuickStateDisplay,"Loaded!");
		}
		return val;
	}

	if ((joy & SAL_INPUT_L)&&(joy & SAL_INPUT_R)&&(joy & SAL_INPUT_UP))
	{
		if(mVolumeTimer==0)
		{
			mMenuOptions.volume++;
			if(mMenuOptions.volume>31) mMenuOptions.volume=31;
			sal_AudioSetVolume(mMenuOptions.volume,mMenuOptions.volume);
			mVolumeTimer=5;
			mVolumeDisplayTimer=60;
			sprintf(mVolumeDisplay,"Vol: %d",mMenuOptions.volume);
		}
		return val;
	}

	if ((joy & SAL_INPUT_L)&&(joy & SAL_INPUT_R)&&(joy & SAL_INPUT_DOWN))
	{
		if(mVolumeTimer==0)
		{
			mMenuOptions.volume--;
			if(mMenuOptions.volume>31) mMenuOptions.volume=0;
			sal_AudioSetVolume(mMenuOptions.volume,mMenuOptions.volume);
			mVolumeTimer=5;
			mVolumeDisplayTimer=60;
			sprintf(mVolumeDisplay,"Vol: %d",mMenuOptions.volume);
		}
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



void S9xSaveSRAM (int showWarning)
{
	if (CPU.SRAMModified)
	{
		if(Memory.SaveSRAM ((s8*)S9xGetFilename (".srm")))
		{
			sync();
		}
		else
		{
			MenuMessageBox("Saving SRAM...Failed","SRAM Not Saved!","",MENU_MESSAGE_BOX_MODE_PAUSE);
		}
	}
	else if(showWarning)
	{
		MenuMessageBox("Saving SRAM...Ignored!","No changes have been made to SRAM","So there is nothing to save!",MENU_MESSAGE_BOX_MODE_MSG);
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
	Memory.LoadSRAM ((s8*)S9xGetFilename (".srm"));
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
				HandleQuickStateRequests();
				
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
				HandleQuickStateRequests();
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

extern "C"
{

int mainEntry(int argc, char* argv[])
{
	int ref = 0;

	s32 event=EVENT_NONE;

	sal_Init();
	sal_VideoInit(16,SAL_RGB(0,0,0),60);

	sal_DirectoryGetCurrent(mSystemDir,SAL_MAX_PATH);
	MenuInit(mSystemDir,&mMenuOptions);


	//if (argc > 0) 
	{
		//Record ROM name
 		//strcpy(mRomName, argv[0]);
	}
	//else
	{
		//ensure rom name is cleared
		mRomName[0]=0;
	}

	if(SnesInit() == SAL_ERROR)
	{
		sal_Reset();
		return 0;
	}

	while(1)
	{
		mInMenu=1;
		event=MenuRun(mRomName);
		mInMenu=0;

		if(event==EVENT_LOAD_ROM)
		{
			if(mTempState) free(mTempState);
			mTempState=NULL;
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

			event=EVENT_NONE;
		}

		if(event==EVENT_EXIT_APP) break;	
	}

	if(mTempState) free(mTempState);
	mTempState=NULL;
	
	S9xGraphicsDeinit();
	S9xDeinitAPU();
	Memory.Deinit();

	free(GFX.SubZBuffer);
	free(GFX.ZBuffer);
	free(GFX.SubScreen);
	GFX.SubZBuffer=NULL;
	GFX.ZBuffer=NULL;
	GFX.SubScreen=NULL;

	sal_Reset();
  	return 0;
}

}






