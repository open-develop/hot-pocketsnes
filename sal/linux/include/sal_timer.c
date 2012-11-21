/*
** Linux timer for SAL.
**  posix timer
*/

static u32 mTimerFreq=1000000/60;  /* 60 Hz default */

u32 sal_TimerRead()
{
	struct timeval tval; // timing
  
  	gettimeofday(&tval, 0);
  	return ((tval.tv_sec*1000000)+tval.tv_usec)/mTimerFreq;
}

s32 sal_TimerInit(s32 freq)
{
	mTimerFreq=(1000000)/freq; /* This could be updated with Settings.FrameTime instead of Memory.ROMFramesPerSecond */
   	return SAL_OK;
}

void sal_TimerClose(void) 
{

}



