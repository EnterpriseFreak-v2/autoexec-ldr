# autoexec-ldr
A minimal homebrew loader that can load and execute a autoexec.dol file from either a SD Gecko or SD2SP2.

If you don't have a GameCube that is able to run homebrew you can also use a Wii to get the files onto your Memory Card.
## Prerequisites (GameCube)
- A GameCube that is already able to run homebrew software 
- A SD Gecko or SD2SP2 adapter.
- A (micro) SD (HC/XC) card formatted to use FAT16 or FAT32 as filesystem.
- A GameCube Memory Card with atleast 16 blocks available.
- A Memory Card Manager (for example [this](https://github.com/seewood/gcmm) fork of GCMM)

## Prerequisites (Wii)
- A RVL-001 Wii (the ones with GameCube slots) that has the Homebrew Channel installed.
- A SD card or an USB drive formatted with FAT32.
- A GameCube Memory Card with atleast 16 blocks available.
- A Memory Card Manager (for example [this](https://github.com/seewood/gcmm) fork of GCMM)

## Installation
(This assumes that you already have a save game exploit on your Memory Card)
- Create a folder named ```MCBACKUP``` on your SD card / USB drive.
- Copy the latest ```savegame.gci``` from the releases page into the MCBACKUP folder.
- (GameCube): Download the fork of GCMM and place the .DOL file found in the ```gamecube``` folder into the SD root. 
- (Wii): Download the fork of GCMM and copy the ```apps``` folder onto your SD card / USB drive.
- (GameCube): Plug the now prepared SD card into the SD Gecko / SD2SP2
- (Wii): Plug the now prepared SD / USB into the SD card slot / USB port.
- Run GCMM using your current method of homebrew loading. (eg.: the Homebrew Channel on Wii)
- In GCMM restore the savegame.gci to the Memory Card.
- ???
- Profit.
