#include <stdio.h>
#include <gccore.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>

#include <fat.h>
#include <sdcard/gcsd.h>

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
void error(const char *errorMsg);

int main()
{
	__io_gcsdb.startup();
	
	//Check if there's a SD Gecko in memory card slot B
	if(!__io_gcsdb.isInserted())
	{
		error("There is no SD Gecko inserted into Memory Card Slot B.\nConfirm that it is properly inserted!");
	}
	
	//Try to mount the SD card
	if(!fatMountSimple("B", &__io_gcsdb))
	{
		error("Failed to mount the SD card.\nConfirm that it is formatted with FAT16 or FAT32.");
	}
	
	FILE * autoexecFile = fopen("B:/autoexec.dol", "rb");
	u32 autoexecSize;
	char * autoexecBuff;
	
	//Check if autoexec.dol actually exists
	if(!autoexecFile)
	{
		error("Can't open autoexec.dol for reading. Does it really exist?");
	}
	
	fseek(autoexecFile, 0, SEEK_END);
	autoexecSize = ftell(autoexecFile);
	fseek(autoexecFile, 0, SEEK_SET);
	
	autoexecBuff = (char*) malloc(sizeof(char)*autoexecSize);
	
	if (autoexecBuff == NULL)
	{
		error("Failed to allocate enough memory! The autoexec.dol you're trying to boot is too large!");
	}
	
	fread(autoexecBuff, 1, autoexecSize, autoexecFile);
	fclose(autoexecFile);
	
	/* Check if the loaded DOL is actually homebrew The first three bytes in every homebrew dol are always 0x00, 0x00 and 0x01. */
	if(autoexecBuff[0] != 0x00 || autoexecBuff[1] != 0x00 || autoexecBuff[2] != 0x01)
	{
		error("The autoexec.dol file on the SD card is not a valid homebrew.");
	}
	
	u8 i;
	dolheader *autoexecDol = (dolheader*)autoexecBuff;
	
	for(i = 0; i < 7; i++)
	{
		if ((!autoexecDol -> text_size[i]) || (autoexecDol -> text_start[i] < 0x100))
			continue;
		
		u32 size = autoexecDol -> text_size[i];
		u8 *buf = (u8*)autoexecDol -> text_start[i];
		memcpy(buf, autoexecBuff + autoexecDol -> text_pos[i], size);
		DCFlushRange(buf, size);
		ICInvalidateRange(buf, size);
	}
	
	for(i = 0; i < 11; i++)
	{
		if ((!autoexecDol -> data_size[i]) || (autoexecDol -> data_start[i] < 0x100))
			continue;
		
		u32 size = autoexecDol -> data_size[i];
		u8 *buf = (u8*)autoexecDol -> data_start[i];
		memcpy(buf, autoexecBuff + autoexecDol -> data_pos[i], size);
		DCFlushRange(buf, size);
		ICInvalidateRange(buf, size);
	}
	
	if (autoexecBuff != NULL)
		free(autoexecBuff);
	
	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	entrypoint entry = (entrypoint)autoexecDol -> entry_point;
	entry();
	
	return 0;
}
	
