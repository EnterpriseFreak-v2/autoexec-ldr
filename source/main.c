#include <stdio.h>
#include <gccore.h>
#include <string.h>
#include <malloc.h>
#include <ogc/dvd.h>
#include <ogc/card.h>

#include <fat.h>
#include <sdcard/gcsd.h>
#include <sdcard/card_io.h>

#include "error.h"
#include "defines.h"

typedef struct _dolheader {
	u32 text_pos[7];
	u32 data_pos[11];
	u32 text_start[7];
	u32 data_start[11];
	u32 text_size[7];
	u32 data_size[11];
	u32 bss_start;
	u32 bss_size;
	u32 entry_point;
} dolheader;

u8 DVD_StopFinished = 0;

typedef void (*entrypoint) (void);

void dvdStopFinished()
{
    DVD_StopFinished = 1;
}

void deinitFAT(void)
{
    fatUnmount("SD");
    __io_gcsda.shutdown();
    __io_gcsd2.shutdown();
    __io_gcsdb.shutdown();
}

int main(int argc, char **argv)
{
    u8 sdCardMounted = 0;
    u8 i = 0;

    for (i = 0; i < 0x0F; i++) {
        __io_gcsda.startup();
        if (fatMountSimple("SD", &__io_gcsda)) {
            sdgecko_setSpeed(EXI_SPEED32MHZ); //Enable the 32MHz mode for slightly faster load speeds.
            sdCardMounted = 1;
            break;    
        }

        __io_gcsdb.startup();
        if (fatMountSimple("SD", &__io_gcsdb)) {
            sdgecko_setSpeed(EXI_SPEED32MHZ); //Enable the 32MHz mode for slightly faster load speeds.
            sdCardMounted = 1;
            break;    
        }

        __io_gcsd2.startup();
        if (fatMountSimple("SD", &__io_gcsd2)) {
            sdCardMounted = 1;
            break;
        }
    }

    if (!sdCardMounted) {
        deinitFAT();
        error("\t Failed to mount the SD card - Confirm that it's properly inserted.");
    }

    //If we're running the bootdisk version of autoexec-ldr stop the DVD from spinning now.
    //We don't need to do this for the savegame.gci version as dollz3 already stops the dvd drive.
    #ifdef BOOTDISK
    dvdcmdblk drv;
    DVD_Init();
    DVD_ControlDriveAsync(&drv, DVD_SPINMOTOR_DOWN, &dvdStopFinished);
    #endif

    #ifndef IGR
    FILE *userVmodeCfg = fopen("SD:/autoexec-ldr/videomode.txt", "rb");

    //If there is a videomode.txt in the autoexec-ldr directory set the video mode to what the user wants.
    if (userVmodeCfg) {
        u8 mode = fgetc(userVmodeCfg);
        fclose(userVmodeCfg);

        switch (mode) {
            case '0': //240p 60 Hz NTSC
                initVideo(&TVNtsc240Ds);
                break;

            case '1': //480i 60 Hz NTSC
                initVideo(&TVNtsc480Int);
                break;

            case '2': //480p 60 Hz NTSC /!\ REQUIRES COMPONENT CABLE TO WORK !!!!
                initVideo(&TVNtsc480Prog);
                break;

            case '3': //528i 50 Hz PAL50 
                initVideo(&TVPal528Int);
                break;

            case '4': //480i 60 Hz RGB Scart PAL60
                initVideo(&TVEurgb60Hz480Int);
                break;

            default: //240p NTSC 60 Hz fallback
                initVideo(NULL);
                break;
        }
    }
    else {
        initVideo(NULL);
    }
    #endif

    FILE* targetDol = fopen("SD:/autoexec-ldr/autoexec.dol", "rb");
    u32 dolSize = 0;
    char* dolBuf;

    if (!targetDol) {
        deinitFAT();
        error("Can't open autoexec.dol in read mode. Check if it exists on the SD card.");
    }

    fseek(targetDol, 0, SEEK_END);
    dolSize = ftell(targetDol);
    fseek(targetDol, 0, SEEK_SET);

    dolBuf = (char*) malloc(sizeof(char)*dolSize);

    if (dolBuf == NULL) {
        deinitFAT();
        error(" Error during memory allocation. The target autoexec.dol is probably too big.");
    }

    //Read the autoexec.dol file into the buffer
    for (i = 0; i <= 255; i++) {
        if (i * 32768 >= dolSize)
            break;

        #ifdef BOOTDISK
        iprintf("\x1b[7;15HLoading autoexec.dol, please wait! %04d / %04d KB.", (32768 * i) / 1024, dolSize / 1024);
        VIDEO_WaitVSync();
        #endif

        
        fseek(targetDol, i * 32768, SEEK_SET);
        fread(dolBuf + (i * 32768), 1, 32768, targetDol);
    }

    fclose(targetDol); //Close the autoexec.dol file.
    deinitFAT(); //Unmount the SD card and shutdown all storage devices

    #ifdef BOOTDISK
    if (DVD_StopFinished == 0) {
        iprintf("\x1b[8;24HWaiting for DVD drive to stop...");
    }

    while (DVD_StopFinished == 0) {
        VIDEO_WaitVSync();
    }
    #endif

    u8* buf;
    u32 size = 0;
    dolheader *dol = (dolheader*)dolBuf;

    for (i = 0; i < 11; i++) {
        if (i < 7) {
            if ((!dol -> text_size[i]) || (dol -> text_start[i] < 0x100)) {
                continue;
            }

            size = dol -> text_size[i];
            buf = (u8*)dol -> text_start[i];
            memcpy(buf, dolBuf + dol -> text_pos[i], size);
            DCFlushRange(buf, size);
            ICInvalidateRange(buf, size);
        }

        if ((!dol -> data_size[i]) || (dol -> data_start[i] < 0x100)) {
                continue;
        }

        size = dol -> data_size[i];
        buf = (u8*)dol -> data_start[i];
        memcpy(buf, dolBuf + dol -> data_pos[i], size);
        DCFlushRange(buf, size);
        ICInvalidateRange(buf, size);
    }

    if (dolBuf != NULL)
        free(dolBuf);

    SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
    entrypoint entry = (entrypoint) dol -> entry_point;
    entry();

    return 0;
}
