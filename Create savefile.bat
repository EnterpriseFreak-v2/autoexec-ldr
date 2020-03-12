make clean
make
.\tools\dollz3.exe autoexec-ldr.dol boot.dol
.\tools\dol2gci.exe boot.dol savegame.gci
del boot.dol