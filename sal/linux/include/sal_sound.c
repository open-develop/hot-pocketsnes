/*
** Linux sound for SAL.
**  SDL sound
*/

#ifndef SAL_NO_SDL_SOUND
/* use SDL sound */
static u32 mStereo;
static volatile u32 mVolume=15;
static u32 mSoundRate;

void SoundFill(void *userdata, Uint8 *stream, int len)
{
	memcpy(stream,mSoundBuffer[mSb],len);
	if (mSb+1 >= SOUND_BUFFER_COUNT) 
	{
		mSb=0;
	}
	else
	{
		mSb++;
	}
	
}

s32 sal_AudioInit(s32 rate, s32 bits, s32 stereo, s32 Hz)
{
	SDL_AudioSpec in;
	SDL_AudioSpec out;
	int x=0,i=0,j=0;
	
	mSoundRate=rate;
	mStereo=stereo+1;
	mSoundSampleCount=(rate/Hz) * mStereo;
	mSoundBufferSize=mSoundSampleCount*((bits==16)?2:1);

	in.freq=rate;
	if(bits==16)
	{
		in.format=AUDIO_S16;
	}
	else
	{
		in.format=AUDIO_S8;
	}
	in.channels = stereo?2:1;
	in.silence =0;
	in.samples=mSoundSampleCount;
	in.size=mSoundBufferSize;
	in.callback = SoundFill;
	in.userdata = NULL;

	mSb=0;

	//Clear all sound banks
	for(i=0;i<SOUND_BUFFER_COUNT;i++)
	{
		for(j=0;j<mSoundSampleCount;j++)
		{
			mSoundBuffer[i][j] = 0;
		}
	}

	SDL_OpenAudio(&in, &out);
	SDL_PauseAudio(0);

   	return SAL_OK;
}


void sal_AudioClose(void)
{
	SDL_CloseAudio();
}

void sal_AudioSetVolume(s32 l, s32 r) 
{ 
	// Deerrh! obviously not supported
	// just spin the wheel baby
} 

#else /* SAL_NO_SDL_SOUND */
/* so use libao */

#include <ao/ao.h>

static ao_device *ao_dev = NULL;

s32 sal_AudioInit(s32 rate, s32 bits, s32 stereo, s32 Hz)
{
        stereo = stereo ? 2 : 1;
        ao_initialize();
        ao_sample_format ao = {bits, rate, stereo, AO_FMT_LITTLE, NULL};
        ao_dev = ao_open_live(ao_default_driver_id(), &ao, NULL);
        if (!ao_dev) {
                perror("Failed to open sound.\n");
                printf("Failed to open sound.\n");
                return SAL_ERROR;
        }
        mSoundSampleCount = (rate/Hz) * (stereo);
        mSoundBufferSize = mSoundSampleCount * (bits >> 3);
        return SAL_OK;
}

void sal_AudioClose(void)
{
        if (ao_dev)
          ao_close(ao_dev);
        ao_dev = NULL;
        ao_shutdown();
}

void sal_SubmitSamples(void *buff, int len)
{
        ao_play(ao_dev, buff, len);
}

void sal_AudioSetVolume(s32 l, s32 r) 
{ 
        // Deerrh! obviously not supported
        // just spin the wheel baby
} 
#endif /* SAL_NO_SDL_SOUND using liabo */
