
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

