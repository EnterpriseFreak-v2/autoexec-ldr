#!/bin/sh
make clean
make -j4

wine tools/dollz3.exe autoexec-ldr.dol boot.dol
wine tools/dol2gci.exe boot.dol output/savegame.gci
mv autoexec-ldr.dol output/igr.dol

make clean
rm boot.dol
clear
echo "Built savegame.gci and igr.dol! Check the output/ folder!"
