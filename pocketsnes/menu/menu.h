#ifndef _MENU_H_
#define _MENU_H_

#define SYSTEM_DIR		"pocketsnes"

#define ROM_LIST_FILENAME		"romlist.bin"
#define SRAM_FILE_EXT			"srm"
#define SAVESTATE_EXT			"sv"
#define MENU_OPTIONS_FILENAME		"pocketsnes_options"
#define MENU_OPTIONS_EXT		"opt"
#define DEFAULT_ROM_DIR_FILENAME	"romdir"
#define DEFAULT_ROM_DIR_EXT		"opt"

#define SAVESTATE_MODE_SAVE			0
#define SAVESTATE_MODE_LOAD			1
#define SAVESTATE_MODE_DELETE			2

#define POCKETSNES_VERSION			"version 2"

#define MENU_NORMAL_CPU_SPEED 			336
#define MENU_FAST_CPU_SPEED			336

enum  MENU_ENUM
{
	MENU_RETURN = 0,
	MENU_ROM_SELECT,	
	MENU_STATE,
	MENU_RESET_GAME,
	MENU_SAVE_SRAM,
	MENU_AUTO_SAVE_SRAM,
	MENU_SOUND_ON,
	MENU_SOUND_VOL,
	MENU_SOUND_RATE,
	MENU_FRAMESKIP,
	MENU_FULLSCREEN,
	MENU_QUICKKEYS,
	MENU_TRANSPARENCY,
	MENU_FPS,
	MENU_CPU_SPEED,
	MENU_LOAD_GLOBAL_SETTINGS,
	MENU_SAVE_GLOBAL_SETTINGS,
	MENU_LOAD_CURRENT_SETTINGS,
	MENU_SAVE_CURRENT_SETTINGS,
	MENU_DELETE_CURRENT_SETTINGS,
	MENU_CREDITS,
	MENU_EXIT_APP,
	MENU_COUNT
};

enum SAVESTATE_MENU_ENUM
{
	SAVESTATE_MENU_LOAD = 0,
	SAVESTATE_MENU_SAVE,
	SAVESTATE_MENU_DELETE,
	SAVESTATE_MENU_RETURN,
	SAVESTATE_MENU_COUNT
};

enum SRAM_MENU_ENUM
{
	SRAM_MENU_LOAD = 0,
	SRAM_MENU_SAVE,
	SRAM_MENU_DELETE,
	SRAM_MENU_RETURN,
	SRAM_MENU_COUNT,
};

enum EVENT_TYPES
{
	EVENT_NONE = 0,
	EVENT_EXIT_APP,
	EVENT_LOAD_ROM,
	EVENT_RUN_ROM,
	EVENT_RESET_ROM,
};

enum MENU_MESSAGE_BOX_MODE
{
	MENU_MESSAGE_BOX_MODE_MSG = 0,
	MENU_MESSAGE_BOX_MODE_PAUSE,
	MENU_MESSAGE_BOX_MODE_YESNO,
};

//Graphic size definitions
#define MENU_TILE_WIDTH      64
#define MENU_TILE_HEIGHT     64
#define MENU_HEADER_WIDTH    320
#define MENU_HEADER_HEIGHT   48
#define HIGHLIGHT_BAR_WIDTH  320
#define HIGHLIGHT_BAR_HEIGHT 16

#define INP_BUTTON_MENU_SELECT			SAL_INPUT_A
#define INP_BUTTON_MENU_CANCEL			SAL_INPUT_B
#define INP_BUTTON_MENU_ENTER			SAL_INPUT_SELECT
#define INP_BUTTON_MENU_DELETE			SAL_INPUT_SELECT
#define INP_BUTTON_MENU_PREVIEW_SAVESTATE	SAL_INPUT_Y
#define INP_BUTTON_MENU_QUICKSAVE1		SAL_INPUT_R
#define INP_BUTTON_MENU_QUICKSAVE2		SAL_INPUT_SELECT
#define INP_BUTTON_MENU_QUICKLOAD1		SAL_INPUT_L
#define INP_BUTTON_MENU_QUICKLOAD2		SAL_INPUT_SELECT
#define MENU_TEXT_LOAD_SAVESTATE 		"Press A to load"
#define MENU_TEXT_OVERWRITE_SAVESTATE		"Press A to overwrite"
#define MENU_TEXT_DELETE_SAVESTATE 		"Press A to delete"
#define MENU_TEXT_PREVIEW_SAVESTATE 		"Press Y to preview"


struct MENU_OPTIONS
{
  unsigned int optionsVersion;
  unsigned int frameSkip;
  unsigned int soundEnabled;
  unsigned int transparency;
  unsigned int volume;
  unsigned int pad_config[32];
  unsigned int country;
  unsigned int showFps;
  unsigned int stereo;
  unsigned int fullScreen;
  unsigned int autoSaveSram;
  unsigned int cpuSpeed;
  unsigned int soundRate;
#define POCKET_SOUND_EMULATED_ONLY 0
#define POCKET_SOUND_ON           1
#define POCKET_SOUND_NOT_EMULATED 2

  unsigned int quickkeys;
  unsigned int spare01;
  unsigned int spare02;
  unsigned int spare03;
  unsigned int spare04;
  unsigned int spare05;
  unsigned int spare06;
  unsigned int spare07;
  unsigned int spare08;

};

struct SAVE_STATE
{
  s8 filename[SAL_MAX_PATH];
  s8 fullFilename[SAL_MAX_PATH];
  u32 inUse;
};

//####################################
//# Functions
//####################################
void MenuInit(s8 *systemDir,struct MENU_OPTIONS *menuOptions);
s32 MenuRun(s8 *romName);
void LoadSram(s8 *path,s8 *romname,s8 *ext,s8 *srammem);
void SaveSram(s8 *path,s8 *romname,s8 *ext,s8 *srammem);
void DeleteSram(s8 *path,s8 *romname,s8 *ext);
s32 SaveMenuOptions(s8 *path, s8 *filename, s8 *ext, s8 *optionsmem, s32 maxSize, s32 showMessage);
s32 LoadMenuOptions(s8 *path, s8 *filename, s8 *ext, s8 *optionsmem, s32 maxSize, s32 showMessage);
s32 DeleteMenuOptions(s8 *path, s8 *filename, s8 *ext, s32 showMessage);
void DefaultMenuOptions(void);
void MenuPause(void);
void PrintTitle(s8 *title);
void PrintTile();
void PrintBar(u32 givenY);
s32 MenuMessageBox(s8 *message1,s8 *message2,s8 *message3,enum MENU_MESSAGE_BOX_MODE mode);


#endif /* _MENU_H_ */





