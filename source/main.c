#include <gccore.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fat.h>
#include <sdcard/gcsd.h>

#include "error.h"

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

typedef void (*entrypoint) (void);

int main(int argc, char **argv)
{
    u8 sdCardMounted = 0;
    u8 i = 0;

    for (i = 0; i < 8; i++) {
        __io_gcsdb.startup();
        if (fatMountSimple("SD", &__io_gcsdb)) {
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
        error("\t Failed to mount the SD card - Confirm that it's properly inserted.");
    }

    FILE* targetDol = fopen("SD:/autoexec.dol", "rb");
    u32 dolSize = 0;
    char* dolBuf;

    if (!targetDol) {
        error("Can't open autoexec.dol in read mode. Check if it exists on the SD card.");
    }

    fseek(targetDol, 0, SEEK_END);
    dolSize = ftell(targetDol);
    fseek(targetDol, 0, SEEK_SET);

    dolBuf = (char*) malloc(sizeof(char)*dolSize);
    if (dolBuf == NULL) {
        error(" Error during memory allocation. The target autoexec.dol is probably too big.");
    }

    for (i = 0; i < 255; i++) {
        fseek(targetDol, 16386*i, SEEK_SET);
        fread(dolBuf + 16386*i, 16386, sizeof(char), targetDol);

        if (i*16386 >= dolSize) {
            break;
        }
    }

    fread(dolBuf, 1, dolSize, targetDol);
    fclose(targetDol);

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

    free(dolBuf);

    SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
    entrypoint entry = (entrypoint) dol -> entry_point;
    entry();

    return 0;
}
