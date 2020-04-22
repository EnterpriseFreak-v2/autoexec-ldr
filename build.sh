#!/bin/sh
rm -rf tmp/
rm -rf output/
rm  include/defines.h
make clean

mkdir tmp/
mkdir tmp/disc

# First, let's do the default build that will later be packed into the savegame.gci
echo "" >> include/defines.h
make -j3

cp autoexec-ldr.dol tmp/
make clean
rm include/defines.h

# Now let's build the version that is designed to be booted of a burned mini DVD.
echo "#define BOOTDISK" >> include/defines.h
make -j3

cp autoexec-ldr.dol tmp/disc/boot.dol
make clean
rm include/defines.h

# Finally burn a version that is intended to be used as a IGR.dol for swiss. This one will do no printing to produce a smaller dol.
echo "#define IGR" >> include/defines.h
make -j3

cp autoexec-ldr.dol tmp/igr.dol
make clean
rm include/defines.h

# Process the builds (compress them, create a bootable ISO image etc.)
mkdir output/
cp tmp/igr.dol output/igr.dol


wine tools/dollz3.exe tmp/autoexec-ldr.dol tmp/boot.dol
wine tools/dol2gci.exe tmp/boot.dol tmp/savegame.gci
cp tmp/savegame.gci output/savegame.gci

mkisofs -R -J -G data/gbi.hdr -no-emul-boot -b boot.dol -o tmp/autoexec-ldr.iso tmp/disc/
cp tmp/autoexec-ldr.iso output/autoexec-ldr.iso


