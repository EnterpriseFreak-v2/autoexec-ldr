#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"

#ifndef IGR
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void initVideo()
{
	VIDEO_Init();
	PAD_Init();

	rmode = &TVNtsc240Ds;
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();	
}
#endif

void error(const char *msg)
{
	#ifdef IGR
	SYS_ResetSystem(SYS_RESTART, 0,0);
	exit(0);
	#endif

	#ifndef IGR
    initVideo();

    iprintf("\x1b[2;31HAn error occurred.\n");
    iprintf("\t%s\n\n", msg);

	iprintf("\x1b[5;5HRestart the console and confirm that potential error causes are fixed.\n");
	iprintf("\tIf you performed an IGR your SD adapter might not have become ready again.\n\n");
	iprintf("\x1b[10;21H- Press RESET to restart the console -");
	iprintf("\x1b[12;3HBuild: %s", builddate);

    for(;;) {
        VIDEO_WaitVSync();

		if (SYS_ResetButtonDown()) {
			SYS_ResetSystem(SYS_RESTART, 0, 0);
			exit(0);
		}
    }
	#endif
}
