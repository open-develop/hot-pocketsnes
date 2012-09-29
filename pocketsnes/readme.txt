Installation
------------

Extract contents of the zip file and copy everything to the GAME
folder on your DINGOO.

APP - Usage
----------
Go to the Games menu option on your Dingoo
Select the 3D Game option ( or Dingoo Games depending on the version of firmware you have )
Select the pocketsnes folder
Select the pocketsnes.app item
PocketSnes will now start

SIM -Usage
----------
Go to the Games menu option on your Dingoo
Select the "Interesting Game" option ( or Roms option depending on the version of firmware you have )
Browse to where your Snes roms are located
Select a rom
PocketSnes will now start and run your selected game

Menu
----
Return to Game
Exits menu and returns to Snes emulation ( if a game was previously loaded )
Select Rom
Enters the rom browser interface
Press UP or DOWN to change your selection	
You can page through the items using RIGHT or LEFT

Save Default Directory
Sets the current directory as the default directory for PocketSnes
Main Menu
Returns you to the main menu
<..> 
will take you up a directory

Press A to select a ROM which will then be loaded 
Save States
Save States are basically dumps of the emulation state which
can be reloaded at any point.  This effectively allows you 
to save you game at any point.

The Save State management screen allows you to load,save or delete save states
Save States are saved to the pocketsnes directory on your dingoo
Reset Game
Resets the current game if one is loaded
Save SRAM
Some Snes games allowed you to save your game to the special memory on the Snes cartridge.  
This memory was called SRAM.  PocketSNES emulates this type of memory which means you can
play your Snes games exactly as you would on Snes.

The save SRAM option allows you to save the contents of SRAM to disk
so it can be loaded again later when you load the Snes Game. 
Auto Save SRAM
Press RIGHT or LEFT to change the option
ON - Means that PocketSnes will save the contents of SRAM everytime you enter the menu.
OFF - Means that PocketSnes will never automatically save SRAM to disk.  This means you
have to use the Save SRAM menu option or your SRAM saves will be lost.
Sound
Turns sound on or off
Volume
Allows you to set the sound volume
Sound Rate
Allows you to set the sound rate of the emulated sound stream.  The lower the value
the lower the quality.  The higher the value the better the quality but it has more
impact on the performance of the emulator
Frameskip
Frameskip allows you to control how many times the emulated Snes screen is drawn
per second.  
AUTO - means that the system will monitor the emulator performance and skip
frames when it believes the emulator is running too slowly
0    = 60 fps
The system will never skip a frame, this could cause some
games to run very slowly
1    = 30 fps
2    = 20 fps
and so on..
Full Screen Mode
Allows you set the screen to fill the entire Dingoo screen or render
the screen at its current resolution which will mean having black borders
around the emulated Snes screen.
Transparency
This is a hack which allows you to increase the performance of the emulator.
Not all games use Transparency effects, so for those games you can disable
transparency and the game will run faster.  
NOTE: Enabling this option on games that do use transparency can cause
the emulated Snes display to be drawn incorrectly.
Show FPS
FPS = Frames Per Second
This is the number of times the emulated Snes screen has been updated over the
last second.  These stats give you can idea of how the emulator is performing

60 FPS - nice
30 FPS - ok
20 FPS - bareable
10 FPS - yuck! try lowering the sound rate or increasing the cpu speed
CPU Speed
This option allows you to configure the CPU speed on your Dingoo when
running the Snes emulation.  The higher the value, the better the FPS
but it will cause your battery to drain quickly.

RIGHT or LEFT = increase/decrease value by 1Mhz
(RIGHT or LEFT) + A = increase/decrease value by 10Mhz

336 Mhz - default Dingoo CPU speed

NOTE: Playing with the CPU speed will not cause permanent problems on
your dingoo but it could cause the emulator to crash all over
the place.  So just bare that in mind when trying to set the
CPU speed too high or too low. 
Load Global Settings
This will reload the global default menu settings from disk
Note: All settings are held in the pocketsnes directory
Save Global Settings
This will save the current menu settings to disk.  When PocketSnes
next runs, these settings will be automatically loaded.
Load Settings For Current Game
This will reload the settings for the Snes game currently loaded
Save Settings For Current Game
This will save the current menu settings to disk.  PocketSnes will
then reload these settings the next time this particular Snes game
is loaded.
Delete Settings For Current Game
Deletes any saved settings for the currently Snes game.
Credits
Displays credit information for PocketSnes
Exit Application
Exits PocketSnes and returns you to the Dingoo menu.


In Game - QuickSave/QuickLoad
-----------------------------
While running a Snes game you can save your current position to memory by pressing
the following button combination

L + R + LEFT

To reload this position press

L + R + RIGHT

Messages will be displayed at the top of the screen to confirm when your state has
been saved or loaded.

In Game Volume Control
----------------------
You can change the sound volume in game by pressing the following button combinations

The increase the volume press

L + R + UP

To decrease the volume press

L + R + DOWN

A message will be display at the top of the screen to inform you of the new
volume setting.

Credits
-------
PocketSNES created by Scott Ramsby
Ported to the Dingoo by Reesy

Special thanks to Flatmush and Hartex
who created the Dingoo SDK without
which this port would not have been
possible!

Thanks to clach04 for adding dirent.h functionality
to the Dingoo SDK which made adding the rom browsing
very simple.

Change Log
----------
v2.1    09/01/2011
Rom Browser modified to allow browsing of the SD card ( b:\ )
Emulator now released as SIM and APP, since some wierd people
actually like the SIM method.

v2	08/01/2011
Application moved from SIM to APP
Rom browser added
ZIP support now enabled
Additional CPU speeds added
Quick Save States added
L + R + LEFT - saves current state to memory
L + R + RIGHT - loads saved state from memory
NOTE: Quick states are lost if you load a new rom or exit PocketSnes
Use the normal save state menu option to create states which
are saved to disk.

v1.2	04/01/2011
Fixed issue with SRAM saves.  
SRAM saves saved in same directory as Snes rom
PNG menu skin now moved into pocketsnes sub directory of GAME directory

v1.1	02/01/2011
Fixed issue with sound rates menu option
Linux port also added to source code which include SDL port details

v1 01/01/2011
Clean port of Pocketsnes 
SIM interface + menu

