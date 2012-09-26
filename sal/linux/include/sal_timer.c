
static u32 mTimerFreq=1000000/60;

u32 sal_TimerRead()
{
	struct timeval tval; // timing
  
  	gettimeofday(&tval, 0);
  	//tval.tv_usec
  	//tval.tv_sec
  	return ((tval.tv_sec*1000000)+tval.tv_usec)/mTimerFreq;
}

s32 sal_TimerInit(s32 freq)
{
	mTimerFreq=(1000000)/freq;
   	return SAL_OK;
}

void sal_TimerClose(void) 
{

}



