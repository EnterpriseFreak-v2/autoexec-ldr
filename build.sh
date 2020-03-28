#!/bin/sh

make clean
make -j3

wine tools/dollz3.exe autoexec-ldr.dol boot.dol
wine tools/dol2gci.exe boot.dol savegame.gci

rm boot.dol
clear
echo "Built savegame.gci - Inject it with a memory card manager of your choice."
