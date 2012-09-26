
#include <stdio.h>
#include <dirent.h>
#include <SDL/SDL.h>
#include <sys/time.h>
#include <png.h>
#include "sal.h"
#include "unzip.h"
#include "zip.h"

#define SAL_FRAME_BUFFER_COUNT	4
#define SOUND_BUFFER_COUNT 	4
#define CPU_SPEED_COUNT		0
#define AUDIO_RATE_COUNT	5
#define PALETTE_BUFFER_LENGTH	256*2*4
#define MAX_SOUND_LEN 	((48000/60)*2)

static SDL_Surface *mScreen = NULL;
static u32 mSoundSampleCount=0;
static u32 mSoundBufferSize=0;
static u32 mSoundThreadFlag=0;
static u32 mSoundLastCpuSpeed=0;
static u32 mPaletteBuffer[PALETTE_BUFFER_LENGTH];
static u32 *mPaletteCurr=(u32*)&mPaletteBuffer[0];
static u32 *mPaletteLast=(u32*)&mPaletteBuffer[0];
static u32 *mPaletteEnd=(u32*)&mPaletteBuffer[PALETTE_BUFFER_LENGTH];
static u32 mInputFirst=0;

s32 mAudioRateLookup[AUDIO_RATE_COUNT]={8250,11025,16500,22050,44100};
s32 mCpuSpeedLookup[1]={0};

#include "sal_common.c"

u32 sal_InputPoll()
{
	int i=0;
	u32 inputHeld=0;
	u32 timer=0;
	u8 *keystate;

	SDL_PumpEvents();

	keystate = SDL_GetKeyState(NULL);
	
	if ( keystate[SDLK_z] ) inputHeld|=SAL_INPUT_A;
	if ( keystate[SDLK_x] ) inputHeld|=SAL_INPUT_B;
	if ( keystate[SDLK_a] ) inputHeld|=SAL_INPUT_X;
	if ( keystate[SDLK_s] ) inputHeld|=SAL_INPUT_X;
	if ( keystate[SDLK_q] ) inputHeld|=SAL_INPUT_L;
	if ( keystate[SDLK_w] ) inputHeld|=SAL_INPUT_R;
	if ( keystate[SDLK_RETURN] ) inputHeld|=SAL_INPUT_START;
	if ( keystate[SDLK_SPACE] ) inputHeld|=SAL_INPUT_SELECT;
	if ( keystate[SDLK_UP] ) inputHeld|=SAL_INPUT_UP;
	if ( keystate[SDLK_DOWN] ) inputHeld|=SAL_INPUT_DOWN;
	if ( keystate[SDLK_LEFT] ) inputHeld|=SAL_INPUT_LEFT;
	if ( keystate[SDLK_RIGHT] ) inputHeld|=SAL_INPUT_RIGHT;

	// Process key repeats
	timer=sal_TimerRead();
	for (i=0;i<32;i++)
	{
		if (inputHeld&(1<<i)) 
		{
			if(mInputFirst&(1<<i))
			{
				if (mInputRepeatTimer[i]<timer)
				{
					mInputRepeat|=1<<i;
					mInputRepeatTimer[i]=timer+10;
				}
				else
				{
					mInputRepeat&=~(1<<i);
				}
			}
			else
			{
				//First press of button
				//set timer to expire later than usual
				mInputFirst|=(1<<i);
				mInputRepeat|=1<<i;
				mInputRepeatTimer[i]=timer+50;
			}
		}
		else			
		{
			mInputRepeatTimer[i]=timer-10;
			mInputRepeat&=~(1<<i);
			mInputFirst&=~(1<<i);
		}
		
	}

	if(mInputIgnore)
	{
		//A request to ignore all key presses until all keys have been released has been made
		//check for release and clear flag, otherwise clear inputHeld and mInputRepeat
		if (inputHeld == 0)
		{
			mInputIgnore=0;
		}
		inputHeld=0;
		mInputRepeat=0;
	}

	return inputHeld;
}

void sal_CpuSpeedSet(u32 mhz)
{

}

s32 sal_Init(void)
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		return SAL_ERROR;
	}
	sal_TimerInit(60);

	memset(mInputRepeatTimer,0,sizeof(mInputRepeatTimer));

	return SAL_OK;
}

u32 sal_VideoInit(u32 bpp, u32 color, u32 refreshRate)
{
	

	if (mScreen)
	{
		if (mBpp == bpp)
		{
			return SAL_OK;
		}
		SDL_VideoQuit();
		mScreen=NULL;
	}
	
	mBpp=bpp;
	mRefreshRate=refreshRate;

	//Set up the screen
	mScreen = SDL_SetVideoMode( SAL_SCREEN_WIDTH, SAL_SCREEN_HEIGHT, bpp, SDL_SWSURFACE );

    	//If there was an error in setting up the screen
    	if( mScreen == NULL )
    	{
		sal_LastErrorSet("SDL_SetVideoMode failed");        	
		return SAL_ERROR;
    	}

    	// lock surface if needed 
	if (SDL_MUSTLOCK(mScreen)) 
	{ 
		if (SDL_LockSurface(mScreen) < 0) 
		{ 
			sal_LastErrorSet("unable to lock surface"); 
			return SAL_ERROR;
		} 
	}

	sal_VideoClear(color);
   
	sal_VideoFlip(1);
   
	return SAL_OK;
}

void sal_VideoFlip(s32 vsync)
{
	if (SDL_MUSTLOCK(mScreen)) 
	{ 
		SDL_UnlockSurface(mScreen); 
	}

	//Update the screen
    	if( SDL_Flip( mScreen ) == -1 )
    	{
        	return;
    	}

	// lock surface if needed 
	if (SDL_MUSTLOCK(mScreen)) 
	{ 
		if (SDL_LockSurface(mScreen) < 0) 
		{ 
			sal_LastErrorSet("unable to lock surface"); 
			return;
		} 
	}

	return;
}

void *sal_VideoGetBuffer()
{
	return (void*)mScreen->pixels;
}

u32 sal_VideoSetScaling(s32 width, s32 height)
{
	return SAL_ERROR;
}

void sal_VideoPaletteSync() 
{ 	
	
} 

void sal_VideoPaletteSet(u32 index, u32 color)
{
	*mPaletteCurr++=index;
	*mPaletteCurr++=color;
	if(mPaletteCurr>mPaletteEnd) mPaletteCurr=&mPaletteBuffer[0];
}


void sal_Reset(void)
{
	sal_AudioClose();
}


#include "sal_sound.c"
#include "sal_filesys.c"
#include "sal_timer.c"

int mainEntry(int argc, char *argv[]);

// Prove entry point wrapper
int main(int argc, char *argv[])
{	
	return mainEntry(argc-1,&argv[1]);
}


