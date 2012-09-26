
#include "sal.h"
#include "menu.h"
#include "snapshot.h"

#define MAX_DISPLAY_CHARS			40

static u16 mMenuTile[64*64];
static u16 mMenuHeader[320*48];
static u16 mHighLightBar[320*16];

static s32 mMenutileXscroll=0;
static s32 mMenutileYscroll=0;
static s32 mTileCounter=0;
static s32 mQuickSavePresent=0;

static s8 mMenuText[30][MAX_DISPLAY_CHARS];

static struct SAVE_STATE mSaveState[10];  // holds the filenames for the savestate and "inuse" flags
static s8 mSaveStateName[SAL_MAX_PATH]={""};       // holds the last filename to be scanned for save states
static s8 mRomName[SAL_MAX_PATH]={""};
static s8 mSystemDir[SAL_MAX_PATH];
static struct MENU_OPTIONS *mMenuOptions=NULL;

									
void DefaultMenuOptions(void)
{
	mMenuOptions->frameSkip=0;   //auto
	mMenuOptions->soundEnabled = 1; 
	mMenuOptions->volume=25; 
	mMenuOptions->cpuSpeed=336; 
	mMenuOptions->country=0;
	mMenuOptions->showFps=1;
	mMenuOptions->soundRate=48000;
	mMenuOptions->fullScreen=1;
	mMenuOptions->autoSaveSram=0;
}

s32 LoadMenuOptions(s8 *path, s8 *filename, s8 *ext, s8 *optionsmem, s32 maxSize, s32 showMessage)
{
	s8 fullFilename[SAL_MAX_PATH];
	s8 _filename[SAL_MAX_PATH];
	s8 _ext[SAL_MAX_PATH];
	s8 _path[SAL_MAX_PATH];
	s32 size=0;

    	sal_DirectorySplitFilename(filename, _path, _filename, _ext);
	sprintf(fullFilename,"%s%s%s.%s",path,SAL_DIR_SEP,_filename,ext);
	return sal_FileLoad(fullFilename,(u8*)optionsmem,maxSize,(u32*)&size);
}

s32 SaveMenuOptions(s8 *path, s8 *filename, s8 *ext, s8 *optionsmem, s32 maxSize, s32 showMessage)
{
	s8 fullFilename[SAL_MAX_PATH];
	s8 _filename[SAL_MAX_PATH];
	s8 _ext[SAL_MAX_PATH];
	s8 _path[SAL_MAX_PATH];
	
	if (showMessage)
	{
		PrintTile();
		PrintTitle("");
		sal_VideoPrint(8,120,"Saving...",SAL_RGB(31,31,31));
		sal_VideoFlip(1);
	}

	sal_DirectorySplitFilename(filename, _path, _filename, _ext);
	sprintf(fullFilename,"%s%s%s.%s",path,SAL_DIR_SEP,_filename,ext);
	return sal_FileSave(fullFilename,(u8*)optionsmem,maxSize);
}

s32 DeleteMenuOptions(s8 *path, s8 *filename, s8 *ext, s32 showMessage)
{
	s8 fullFilename[SAL_MAX_PATH];
	s8 _filename[SAL_MAX_PATH];
	s8 _ext[SAL_MAX_PATH];
	s8 _path[SAL_MAX_PATH];
	
	if (showMessage)
	{
		PrintTile();
		PrintTitle("");
		sal_VideoPrint(8,120,"Deleting...",SAL_RGB(31,31,31));
		sal_VideoFlip(1);
	}

	sal_DirectorySplitFilename(filename, _path, _filename, _ext);
	sprintf(fullFilename,"%s%s%s.%s",path,SAL_DIR_SEP,_filename,ext);
	sal_FileDelete(fullFilename);
	return SAL_OK;
}

void MenuPause()
{
	sal_InputWaitForPress();
	sal_InputWaitForRelease();
}

s32 MenuMessageBox(s8 *message1,s8 *message2,s8 *message3, enum MENU_MESSAGE_BOX_MODE mode)
{
  s32 select=0;
  s32 subaction=-1;
  u32 keysRepeat=0,keysHeld=0;
  
  sal_InputIgnore();
  while(subaction==-1)
  {
     keysHeld=sal_InputPoll();
	 keysRepeat=sal_InputRepeat();
     if (keysRepeat&SAL_INPUT_UP) 
     {
       select=SAL_OK; // Up
     }
     if (keysRepeat&SAL_INPUT_DOWN) 
     {
       select=SAL_ERROR; // Down
     }
     if ((keysHeld&INP_BUTTON_MENU_SELECT) || (keysHeld&INP_BUTTON_MENU_CANCEL))
     {
        subaction=select;
     }
     PrintTile();
     PrintTitle("Message Box");
     sal_VideoPrint(8,50,message1,SAL_RGB(31,31,31));
     sal_VideoPrint(8,60,message2,SAL_RGB(31,31,31));
     sal_VideoPrint(8,70,message3,SAL_RGB(31,31,31));
     switch(mode)
     {
        case MENU_MESSAGE_BOX_MODE_YESNO: // yes no input
	       if(select==SAL_OK)
	       {
			  PrintBar(120-4);
	          sal_VideoPrint(8,120,"YES",SAL_RGB(0,0,0));
	          sal_VideoPrint(8,140,"NO",SAL_RGB(31,31,31));
	       }
	       else
	       {
			  PrintBar(140-4);
	          sal_VideoPrint(8,120,"YES",SAL_RGB(31,31,31));
	          sal_VideoPrint(8,140,"NO",SAL_RGB(0,0,0));
	       }
	       break;
	case MENU_MESSAGE_BOX_MODE_PAUSE: 
			PrintBar(120-4);
			sal_VideoPrint(8,120,"Press button to continue",SAL_RGB(0,0,0));
			break;
	case MENU_MESSAGE_BOX_MODE_MSG:
			subaction=SAL_OK;
			break;
     }
     sal_VideoFlip(1);
  }
  sal_InputIgnore();
  return(subaction);
}

void PrintTile()
{
	sal_ImageDrawTiled(mMenuTile, MENU_TILE_WIDTH, MENU_TILE_HEIGHT, mMenutileXscroll, mMenutileYscroll, 0, 48);	

	mTileCounter++;
	if (mTileCounter > 1)
	{
		mTileCounter=0;
		mMenutileXscroll++;
		if(mMenutileXscroll>=MENU_TILE_WIDTH) mMenutileXscroll=0;
	  
		mMenutileYscroll++;
		if(mMenutileYscroll>=MENU_TILE_HEIGHT) mMenutileYscroll=0;
	}  
	return; 
}

void PrintTitle(s8 *title)
{	
	sal_ImageDraw(mMenuHeader,MENU_HEADER_WIDTH, MENU_HEADER_HEIGHT,0,0);	
	//sal_VideoPrint(6,38,title,SAL_RGB(0,0,0)); 
	//sal_VideoPrint(130,38,DRSMS_VERSION,SAL_RGB(0,0,0));

}

void PrintBar(u32 givenY)
{
	sal_ImageDraw(mHighLightBar,HIGHLIGHT_BAR_WIDTH, HIGHLIGHT_BAR_HEIGHT,0,givenY);
}

static void ScanSaveStates(s8 *romname)
{
	s32 i=0;
	s8 savename[SAL_MAX_PATH];
	s8 filename[SAL_MAX_PATH];
	s8 ext[SAL_MAX_PATH];
	s8 path[SAL_MAX_PATH];

	if(!strcmp(romname,mSaveStateName)) return; // is current save state rom so exit
	
	sal_DirectorySplitFilename(romname,path,filename,ext);

	sprintf(savename,"%s.%s",filename,SAVESTATE_EXT);
  
	for(i=0;i<10;i++)
	{
		/*
		need to build a save state filename
		all saves are held in current working directory (lynxSaveStateDir)
		save filename has following format
		shortname(minus file ext) + SV + saveno ( 0 to 9 )
		*/
		sprintf(mSaveState[i].filename,"%s%d",savename,i);
		sprintf(mSaveState[i].fullFilename,"%s%s%s",mSystemDir,SAL_DIR_SEP,mSaveState[i].filename);
		if (sal_FileExists(mSaveState[i].fullFilename)==SAL_TRUE)
		{
			// we have a savestate
			mSaveState[i].inUse = 1;
		}
		else
		{
			// no save state
			mSaveState[i].inUse = 0;
		}
	}
	strcpy(mSaveStateName,romname);  // save the last scanned romname
}

void LoadStateFile(s8 *filename)
{
	S9xUnfreezeGame(filename);
}

static void SaveStateFile(s8 *filename)
{
	S9xFreezeGame(filename);
}

static s32 SaveStateSelect(s32 mode)
{
	s8 text[128];
	s32 action=11;
	s32 saveno=0;
	u32 keysRepeat=0,keysHeld=0;
	if(mRomName[0]==0)
	{
		// no rom loaded
		// display error message and exit
		return(0);
	}

	ScanSaveStates(mRomName);
	sal_InputIgnore();
	while (action!=0&&action!=100)
	{
		keysHeld=sal_InputPoll();
		keysRepeat=sal_InputRepeat();

		if(keysRepeat&SAL_INPUT_UP) {saveno--; action=1;}
		if(keysRepeat&SAL_INPUT_DOWN) {saveno++; action=1;}
		if(saveno<-1) saveno=9;
		if(saveno>9) saveno=-1;
	      
		if(keysHeld&INP_BUTTON_MENU_CANCEL) action=0; // exit
		else if((keysHeld&INP_BUTTON_MENU_SELECT)&&(saveno==-1)) action=0; // exit
		else if((keysHeld&INP_BUTTON_MENU_SELECT)&&(mode==0)&&((action==2)||(action==5))) action=6;  // pre-save mode
		else if((keysHeld&INP_BUTTON_MENU_SELECT)&&(mode==1)&&(action==5)) action=8;  // pre-load mode
		else if((keysHeld&INP_BUTTON_MENU_SELECT)&&(mode==2)&&(action==5))
		{
			if(MenuMessageBox("Are you sure you want to delete","this save?","",MENU_MESSAGE_BOX_MODE_YESNO)==SAL_OK) action=13;  //delete slot with no preview
		}
		//else if((Inp.held[INP_BUTTON_R]==1)&&(action==12)) action=3;  // preview slot mode
		else if((keysHeld&INP_BUTTON_MENU_SELECT)&&(mode==1)&&(action==12)) action=8;  //load slot with no preview
		else if((keysHeld&INP_BUTTON_MENU_SELECT)&&(mode==0)&&(action==12)) action=6;  //save slot with no preview
		else if((keysHeld&INP_BUTTON_MENU_SELECT)&&(mode==2)&&(action==12))
		{
			if(MenuMessageBox("Are you sure you want to delete","this save?","",MENU_MESSAGE_BOX_MODE_YESNO)==SAL_OK) action=13;  //delete slot with no preview
		}

		PrintTile();
		PrintTitle("Save States");
		sal_VideoPrint(12,230,"Press UP and DOWN to change save slot",SAL_RGB(31,15,5));
      
		if(saveno==-1) 
		{
			if(action!=10&&action!=0) 
			{
				action=10;
			}
		}
		else
		{
			PrintBar(60-4);
			sprintf(text,"SLOT %d",saveno);
			sal_VideoPrint(136,60,text,SAL_RGB(0,0,0));
		}
      
		switch(action)
		{
			case 1:
				//sal_VideoPrint(112,145,14,"Checking....",(unsigned short)SAL_RGB(31,31,31));
				break;
			case 2:
				sal_VideoPrint(144,145,"FREE",SAL_RGB(31,31,31));
				break;
			case 3:
				sal_VideoPrint(104,145,"Previewing....",SAL_RGB(31,31,31));
				break;
			case 4:
				sal_VideoPrint(88,145,"Previewing....fail",SAL_RGB(31,31,31));
				break;
			case 5: 
				sal_VideoPrint(112,145,"Not gonna happen!",SAL_RGB(31,31,31));
				if(mode==1) sal_VideoPrint((320-(strlen(MENU_TEXT_LOAD_SAVESTATE)<<3))>>1,210,MENU_TEXT_LOAD_SAVESTATE,SAL_RGB(31,31,31));
				else if(mode==0) sal_VideoPrint((320-(strlen(MENU_TEXT_OVERWRITE_SAVESTATE)<<3))>>1,210,MENU_TEXT_OVERWRITE_SAVESTATE,SAL_RGB(31,31,31));
				else if(mode==2) sal_VideoPrint((320-(strlen(MENU_TEXT_DELETE_SAVESTATE)<<3))>>1,210,MENU_TEXT_DELETE_SAVESTATE,SAL_RGB(31,31,31));
				break;
			case 6:
				sal_VideoPrint(124,145,"Saving...",SAL_RGB(31,31,31));
				break;
			case 7:
				sal_VideoPrint(124,145,"Saving...Fail!",SAL_RGB(31,31,31));
				break;
			case 8:
				sal_VideoPrint(116,145,"loading....",SAL_RGB(31,31,31));
				break;
				case 9:
				sal_VideoPrint(116,145,"loading....Fail",SAL_RGB(31,31,31));
				break;
			case 10:	
				PrintBar(145-4);
				sal_VideoPrint(104,145,"Return To Menu",SAL_RGB(0,0,0));
				break;
			case 12:
				sal_VideoPrint(124,145,"Slot used",SAL_RGB(31,31,31));
				//sal_VideoPrint((320-(strlen(MENU_TEXT_PREVIEW_SAVESTATE)<<3))>>1,165,strlen(MENU_TEXT_PREVIEW_SAVESTATE),MENU_TEXT_PREVIEW_SAVESTATE,(unsigned short)SAL_RGB(31,31,31));
				if(mode==1) sal_VideoPrint((320-(strlen(MENU_TEXT_LOAD_SAVESTATE)<<3))>>1,175,MENU_TEXT_LOAD_SAVESTATE,SAL_RGB(31,31,31));
				else if(mode==0) sal_VideoPrint((320-(strlen(MENU_TEXT_OVERWRITE_SAVESTATE)<<3))>>1,175,MENU_TEXT_OVERWRITE_SAVESTATE,SAL_RGB(31,31,31));
				else if(mode==2) sal_VideoPrint((320-(strlen(MENU_TEXT_DELETE_SAVESTATE)<<3))>>1,175,MENU_TEXT_DELETE_SAVESTATE,SAL_RGB(31,31,31));
				break;
			case 13:
				sal_VideoPrint(116,145,"Deleting....",SAL_RGB(31,31,31));
				break;
		}
      
		sal_VideoFlip(1);
      
		switch(action)
		{
			case 1:
				if(mSaveState[saveno].inUse) 
				{
					action=12;
				}
				else 
				{
					action=2;
				}
				break;
			case 3:
				LoadStateFile(mSaveState[saveno].fullFilename);
				action=5;
				break;
			case 6:
				SaveStateFile(mSaveState[saveno].fullFilename);
				mSaveState[saveno].inUse=1;
				action=1;
				break;
			case 7:
				action=1;
				break;
			case 8:
				LoadStateFile(mSaveState[saveno].fullFilename);
				action=100;  // loaded ok so exit
				break;
			case 9:
				action=1;
				break;
			case 11:
				action=1;
				break;
			case 13:
				sal_FileDelete(mSaveState[saveno].fullFilename);
				mSaveState[saveno].inUse = 0;
				action=1;
				break;
		}
	}

	sal_InputIgnore();
	return(action);
}

static
void RenderMenu(s8 *menuName, s32 menuCount, s32 menuSmooth, s32 menufocus)
{
	
	s32 i=0;
	u16 color=0;
	PrintTile();
	PrintTitle(menuName);

    	for (i=0;i<menuCount;i++)
    	{
      		int x=0,y=0;

      		y=(i<<4)-(menuSmooth>>4);
		x=8;
      		y+=112;

      		if (y<=48 || y>=232) continue;
      
      		if (i==menufocus)
      		{
        		color=SAL_RGB(0,0,0);
			PrintBar(y-4);
      		}
      		else
      		{
        		color=SAL_RGB(31,31,31);
      		}

      		sal_VideoPrint(x,y,mMenuText[i],color);
    	}
}

static
s32 SaveStateMenu(void)
{
	s32 menuExit=0,menuCount=SAVESTATE_MENU_COUNT,menufocus=0,menuSmooth=0;
	s32 action=0;
	s32 subaction=0;
	u32 keysRepeat=0,keysHeld=0;

	//Update
	strcpy(mMenuText[SAVESTATE_MENU_LOAD],"Load State");
	strcpy(mMenuText[SAVESTATE_MENU_SAVE],"Save State");
	strcpy(mMenuText[SAVESTATE_MENU_DELETE],"Delete State");
	strcpy(mMenuText[SAVESTATE_MENU_RETURN],"Back");
	sal_InputIgnore();
	while (!menuExit)
	{
		keysHeld=sal_InputPoll();
		keysRepeat=sal_InputRepeat();

		// Change which rom is focused on:
		if (keysRepeat&SAL_INPUT_UP) menufocus--; // Up
		if (keysRepeat&SAL_INPUT_DOWN) menufocus++; // Down
    
		if (keysHeld&INP_BUTTON_MENU_CANCEL) menuExit=1;
    
		if (menufocus>menuCount-1)
		{
			menufocus=0;
			menuSmooth=(menufocus<<8)-1;
		}   
		else if (menufocus<0) 
		{
			menufocus=menuCount-1;
			menuSmooth=(menufocus<<8)-1;
		}

		if (keysHeld&INP_BUTTON_MENU_SELECT)
		{
			switch(menufocus)
			{
				case SAVESTATE_MENU_LOAD:
					subaction=SaveStateSelect(SAVESTATE_MODE_LOAD);
					if(subaction==100)
					{
						menuExit=1;
						action=100;
					}
					break;
				case SAVESTATE_MENU_SAVE:
					SaveStateSelect(SAVESTATE_MODE_SAVE);
					break;
				case SAVESTATE_MENU_DELETE:
					SaveStateSelect(SAVESTATE_MODE_DELETE);
					break;
				case SAVESTATE_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menufocus<<8); menuSmooth>>=3;
		RenderMenu("Save States", menuCount,menuSmooth,menufocus);
		sal_VideoFlip(1);
       
	}
  sal_InputIgnore();
  return action;
}

void ShowCredits()
{
	s32 menuExit=0,menuCount=9,menufocus=0,menuSmooth=0;
	u32 keysRepeat=0,keysHeld=0;

	strcpy(mMenuText[0],"Credits");
	strcpy(mMenuText[1],"-------------------------------------");
	strcpy(mMenuText[2],"PocketSNES created by Scott Ramsby");
	strcpy(mMenuText[3],"Ported to the Dingoo by Reesy");
	strcpy(mMenuText[4],"-------------------------------------");
	strcpy(mMenuText[5],"Special thanks to Flatmush and Hartex");
	strcpy(mMenuText[6],"who created the Dingoo SDK without ");
	strcpy(mMenuText[7],"which this port would not have been ");
	strcpy(mMenuText[8],"possible!");

	sal_InputIgnore();
	while (!menuExit)
	{
		keysHeld=sal_InputPoll();
		keysRepeat=sal_InputRepeat();

		if (keysRepeat&SAL_INPUT_UP) menufocus--; // Up
		if (keysRepeat&SAL_INPUT_DOWN) menufocus++; // Down

    
		if (keysHeld&INP_BUTTON_MENU_CANCEL) menuExit=1;
    
		if (menufocus>menuCount-1)
		{
			menufocus=0;
			menuSmooth=(menufocus<<8)-1;
		}   
		else if (menufocus<0) 
		{
			menufocus=menuCount-1;
			menuSmooth=(menufocus<<8)-1;
		}

		// Draw screen:
		menuSmooth=menuSmooth*7+(menufocus<<8); menuSmooth>>=3;
		RenderMenu("Credits", menuCount,menuSmooth,menufocus);
		sal_VideoFlip(1);
	}
  	sal_InputIgnore();
}

static 
void MainMenuUpdateText(s32 menu_index)
{
	switch(menu_index)
	{
		case MENU_STATE:
			strcpy(mMenuText[MENU_STATE],"Save States");
			break;

		case MENU_RESET_GAME:
			strcpy(mMenuText[MENU_RESET_GAME],"Reset Game");
			break;

		case MENU_EXIT_APP:
			strcpy(mMenuText[MENU_EXIT_APP],"Exit Application");
			break;

		case MENU_CREDITS:
			strcpy(mMenuText[MENU_CREDITS],"Credits");
			break;

		case MENU_RETURN:
			strcpy(mMenuText[MENU_RETURN],"Return To Game");
			break;

		case MENU_AUTO_SAVE_SRAM:
			switch(mMenuOptions->autoSaveSram)
			{
			case 0:
				strcpy(mMenuText[MENU_AUTO_SAVE_SRAM],"Auto Save SRAM:             OFF");
				break;
			case 1:
				strcpy(mMenuText[MENU_AUTO_SAVE_SRAM],"Auto Save SRAM:             ON");
				break;
			}
			break;

		case MENU_TRANSPARENCY:
			switch(mMenuOptions->transparency)
			{
			case 0:
				strcpy(mMenuText[MENU_TRANSPARENCY],"Transparency:               OFF");
				break;
			case 1:
				strcpy(mMenuText[MENU_TRANSPARENCY],"Transparency:               ON");
				break;
			}
			break;

		case MENU_SOUND_ON:
			switch(mMenuOptions->soundEnabled)
			{
			case 0:
				strcpy(mMenuText[MENU_SOUND_ON],"Sound:                      OFF");
				break;
			case 1:
				strcpy(mMenuText[MENU_SOUND_ON],"Sound:                      ON");
				break;
			}
			break;
		
		case MENU_SOUND_RATE:		
			sprintf(mMenuText[MENU_SOUND_RATE],"Sound Rate:                 %d",mMenuOptions->soundRate);
			break;

		case MENU_CPU_SPEED:		
			sprintf(mMenuText[MENU_CPU_SPEED],"Cpu Speed:                  %d",mMenuOptions->cpuSpeed);
			break;
		
		case MENU_SOUND_VOL:
			sprintf(mMenuText[MENU_SOUND_VOL],"Volume:                     %d",mMenuOptions->volume);
			break;
		
		case MENU_FRAMESKIP: 
			switch(mMenuOptions->frameSkip)
			{
				case 0:
					strcpy(mMenuText[MENU_FRAMESKIP],"Frameskip:                  AUTO");
					break;
				default:
					sprintf(mMenuText[MENU_FRAMESKIP],"Frameskip:                  %d",mMenuOptions->frameSkip-1);
					break;
			}
			break;

		case MENU_FPS:
			switch(mMenuOptions->showFps)
			{
				case 0:
					strcpy(mMenuText[MENU_FPS],"Show FPS:                   OFF");

					break;
				case 1:
					strcpy(mMenuText[MENU_FPS],"Show FPS:                   ON");
					break;
			}
			break;

		case MENU_FULLSCREEN:
			switch(mMenuOptions->fullScreen)
			{
				case 0:
					strcpy(mMenuText[MENU_FULLSCREEN],"Full Screen Mode:           OFF");
					break;
				case 1:
					strcpy(mMenuText[MENU_FULLSCREEN],"Full Screen Mode:           ON");
					break;  
			}
			break;
			
		case MENU_LOAD_GLOBAL_SETTINGS:
			strcpy(mMenuText[MENU_LOAD_GLOBAL_SETTINGS],"Load Global Settings");
			break;
			
		case MENU_SAVE_GLOBAL_SETTINGS:
			strcpy(mMenuText[MENU_SAVE_GLOBAL_SETTINGS],"Save Global Settings");
			break;
			
		case MENU_LOAD_CURRENT_SETTINGS:
			strcpy(mMenuText[MENU_LOAD_CURRENT_SETTINGS],"Load Settings For Current Game");
			break;
		
		case MENU_SAVE_CURRENT_SETTINGS:
			strcpy(mMenuText[MENU_SAVE_CURRENT_SETTINGS],"Save Settings For Current Game");
			break;

		case MENU_DELETE_CURRENT_SETTINGS:
			strcpy(mMenuText[MENU_DELETE_CURRENT_SETTINGS],"Delete Settings For Current Game");
			break;

		case MENU_SAVE_SRAM:
			strcpy(mMenuText[MENU_SAVE_SRAM],"Save SRAM");
			break;
	}
}

static
void MainMenuUpdateTextAll(void)
{
	MainMenuUpdateText(MENU_STATE);
	MainMenuUpdateText(MENU_RESET_GAME);
	MainMenuUpdateText(MENU_EXIT_APP);
	MainMenuUpdateText(MENU_RETURN);
	MainMenuUpdateText(MENU_TRANSPARENCY);
	MainMenuUpdateText(MENU_CPU_SPEED);
	MainMenuUpdateText(MENU_SOUND_ON);
	MainMenuUpdateText(MENU_SOUND_RATE);
	MainMenuUpdateText(MENU_SOUND_VOL);
	MainMenuUpdateText(MENU_FRAMESKIP);
	MainMenuUpdateText(MENU_FPS);
	MainMenuUpdateText(MENU_FULLSCREEN);
	MainMenuUpdateText(MENU_LOAD_GLOBAL_SETTINGS);
	MainMenuUpdateText(MENU_SAVE_GLOBAL_SETTINGS);
	MainMenuUpdateText(MENU_LOAD_CURRENT_SETTINGS);
	MainMenuUpdateText(MENU_SAVE_CURRENT_SETTINGS);
	MainMenuUpdateText(MENU_DELETE_CURRENT_SETTINGS);
	MainMenuUpdateText(MENU_RETURN);
	MainMenuUpdateText(MENU_CREDITS);
	MainMenuUpdateText(MENU_AUTO_SAVE_SRAM);
	MainMenuUpdateText(MENU_SAVE_SRAM);
}

void MenuInit(s8 *systemDir)
{
	s8 filename[SAL_MAX_PATH];
	u16 *pix;
	s32 x;

	pix=&mMenuTile[0];
	for(x=0;x<64*64;x++) *pix++=0;
	pix=&mMenuHeader[0];
	for(x=0;x<320*48;x++) *pix++=0;
	pix=&mHighLightBar[0];
	for(x=0;x<320*16;x++) *pix++=0xFFFF;

	strcpy(filename,systemDir);
	sal_DirectoryCombine(filename,"pocketsnes_tile.png");	
	if (sal_ImageLoad(filename, &mMenuTile, 64, 64) == SAL_ERROR)
	{
		MenuMessageBox("Failed to load image",filename,sal_LastErrorGet(),MENU_MESSAGE_BOX_MODE_PAUSE);
	}
	
	strcpy(filename,systemDir);
	sal_DirectoryCombine(filename,"pocketsnes_header.png");	
	if (sal_ImageLoad(filename, &mMenuHeader, 320, 48) == SAL_ERROR)
	{
		MenuMessageBox("Failed to load image",filename,sal_LastErrorGet(),MENU_MESSAGE_BOX_MODE_PAUSE);
	}
	
	strcpy(filename,systemDir);
	sal_DirectoryCombine(filename,"pocketsnes_bar.png");
	if (sal_ImageLoad(filename, &mHighLightBar, 320, 16) == SAL_ERROR)
	{
		MenuMessageBox("Failed to load image",filename,sal_LastErrorGet(),MENU_MESSAGE_BOX_MODE_PAUSE);
	}
}

void MenuReloadOptions()
{
	s8 _path[SAL_MAX_PATH];
	s8 _filename[SAL_MAX_PATH];
	s8 _ext[SAL_MAX_PATH];

	sal_DirectorySplitFilename(mRomName, _path, _filename, _ext);

	//Load settings for game
	if (LoadMenuOptions(_path, mRomName, MENU_OPTIONS_EXT, (s8*)mMenuOptions, sizeof(struct MENU_OPTIONS), 0) != SAL_OK)
	{
		if(LoadMenuOptions(mSystemDir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (s8*)mMenuOptions, sizeof(struct MENU_OPTIONS), 0) != SAL_OK)
		{
			DefaultMenuOptions();
		}
	}
}

extern "C" void S9xSaveSRAM(void);

s32 MenuRun(struct MENU_OPTIONS *options, s8 *romName, s8 *systemDir)
{
	s32 menuExit=0,menuCount=MENU_COUNT,menufocus=0,menuSmooth=0;
	s32 action=EVENT_NONE;
	s32 subaction=0;
	u32 keysRepeat=0,keysHeld=0;

	mMenuOptions=options;

	sal_CpuSpeedSet(MENU_NORMAL_CPU_SPEED);

	if((sal_StringCompare(mRomName,romName)!=0) || sal_StringCompare(mSystemDir,systemDir)!=0)
	{
		//Romname or systemDir are different
		//Need to reload options
		
		strcpy(mRomName,romName);
		strcpy(mSystemDir,systemDir);
		MenuReloadOptions();
		
		return EVENT_LOAD_ROM;
	}

	sal_VideoInit(16,0,60);
	sal_VideoSetScaling(320,240);

	MainMenuUpdateTextAll();
	sal_InputIgnore();

	while (!menuExit)
	{
		keysHeld=sal_InputPoll();
		keysRepeat=sal_InputRepeat();

		if (keysRepeat&SAL_INPUT_UP) menufocus--; // Up
		if (keysRepeat&SAL_INPUT_DOWN) menufocus++; // Down
    
		if (keysHeld&INP_BUTTON_MENU_CANCEL) 
		{
			if(mRomName[0]!=0)
			{
				action=EVENT_RUN_ROM;
				menuExit=1;
			}
		}
    
		if (menufocus>menuCount-1)
		{
			menufocus=0;
			menuSmooth=(menufocus<<8)-1;
		}   
		else if (menufocus<0) 
		{
			menufocus=menuCount-1;
			menuSmooth=(menufocus<<8)-1;
		}

		if (keysRepeat&SAL_INPUT_LEFT||
			keysRepeat&SAL_INPUT_RIGHT)
		{
			switch(menufocus)
			{
				case MENU_SOUND_ON:
					mMenuOptions->soundEnabled^=1;
					MainMenuUpdateText(MENU_SOUND_ON);
					break;

				case MENU_TRANSPARENCY:
					mMenuOptions->transparency^=1;
					MainMenuUpdateText(MENU_TRANSPARENCY);
					break;

				case MENU_AUTO_SAVE_SRAM:
					mMenuOptions->autoSaveSram^=1;
					MainMenuUpdateText(MENU_AUTO_SAVE_SRAM);
					break;

				case MENU_CPU_SPEED:
					if (keysRepeat&SAL_INPUT_RIGHT)
					{
						mMenuOptions->cpuSpeed=sal_CpuSpeedNext(mMenuOptions->cpuSpeed);	
					}
					else
					{
						mMenuOptions->cpuSpeed=sal_CpuSpeedPrevious(mMenuOptions->cpuSpeed);
					}
					MainMenuUpdateText(MENU_CPU_SPEED);
					break;

				case MENU_SOUND_RATE:
					if (keysRepeat&SAL_INPUT_RIGHT)
					{
						mMenuOptions->soundRate=sal_AudioRateNext(mMenuOptions->soundRate);	
					}
					else
					{
						mMenuOptions->soundRate=sal_AudioRatePrevious(mMenuOptions->soundRate);
					}
					MainMenuUpdateText(MENU_SOUND_RATE);
					break;

				case MENU_SOUND_VOL:
					if (keysRepeat&SAL_INPUT_RIGHT)
					{
						mMenuOptions->volume+=1;
						if(mMenuOptions->volume>31) mMenuOptions->volume=0;
					}
					else
					{
						mMenuOptions->volume-=1;
						if(mMenuOptions->volume>31) mMenuOptions->volume=31;

					}
					MainMenuUpdateText(MENU_SOUND_VOL);
					break;

				case MENU_FRAMESKIP:
					if (keysRepeat&SAL_INPUT_RIGHT)
					{
						mMenuOptions->frameSkip++;
						if(mMenuOptions->frameSkip>6) mMenuOptions->frameSkip=0;
					}
					else
					{
						mMenuOptions->frameSkip--;
						if(mMenuOptions->frameSkip>6) mMenuOptions->frameSkip=6;
					}
					MainMenuUpdateText(MENU_FRAMESKIP);
					break;

				case MENU_FPS:
					mMenuOptions->showFps^=1;
					MainMenuUpdateText(MENU_FPS);
					break;

				case MENU_FULLSCREEN:
					mMenuOptions->fullScreen^=1;
					MainMenuUpdateText(MENU_FULLSCREEN);
					break;
			}
		}

		if (keysHeld&INP_BUTTON_MENU_SELECT)
		{
			switch(menufocus)
			{
				
				case MENU_LOAD_GLOBAL_SETTINGS:
					LoadMenuOptions(mSystemDir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)mMenuOptions, sizeof(struct MENU_OPTIONS), 1);
					MainMenuUpdateTextAll();
					break;
				case MENU_SAVE_GLOBAL_SETTINGS:
					SaveMenuOptions(mSystemDir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)mMenuOptions, sizeof(struct MENU_OPTIONS), 1);
					break;

				case MENU_LOAD_CURRENT_SETTINGS:
					if(mRomName[0]!=0)
					{
						LoadMenuOptions(mSystemDir, mRomName, MENU_OPTIONS_EXT, (char*)mMenuOptions, sizeof(struct MENU_OPTIONS), 1);

						MainMenuUpdateTextAll();
					}
					break;
				case MENU_SAVE_CURRENT_SETTINGS:
					if(mRomName[0]!=0)
					{
						SaveMenuOptions(mSystemDir, mRomName, MENU_OPTIONS_EXT, (char*)mMenuOptions, sizeof(struct MENU_OPTIONS), 1);
					}
					break;

				case MENU_DELETE_CURRENT_SETTINGS:
					if(mRomName[0]!=0)
					{
						DeleteMenuOptions(mSystemDir, mRomName, MENU_OPTIONS_EXT, 1);
					}
					break;

				case MENU_STATE:
					if(mRomName[0]!=0)
					{
						subaction=SaveStateMenu();
						if (subaction==100)
						{
							action=EVENT_RUN_ROM;
							menuExit=1;
						}
					}
					MainMenuUpdateTextAll();
					break;

				case MENU_SAVE_SRAM:
					if(mRomName[0]!=0)
					{
						MenuMessageBox("","","Saving SRAM...",MENU_MESSAGE_BOX_MODE_MSG);
						S9xSaveSRAM();
					}
					break;

				case MENU_CREDITS:
					ShowCredits();
					MainMenuUpdateTextAll();
					break;

				case MENU_RESET_GAME:
					if(mRomName[0]!=0)
					{
						action=EVENT_RESET_ROM;
						menuExit=1;
					}
					break;
				case MENU_RETURN:
					if(mRomName[0]!=0)
					{
						action=EVENT_RUN_ROM;
						menuExit=1;
					}
					break;
				case MENU_EXIT_APP:
					action=EVENT_EXIT_APP;
					menuExit=1;
					break;
				
			}	
		}

		// Draw screen:
		menuSmooth=menuSmooth*7+(menufocus<<8); menuSmooth>>=3;
		RenderMenu("Main Menu", menuCount,menuSmooth,menufocus);
		sal_VideoFlip(1);
	}
	
  sal_InputIgnore();
  
  return action;
}



