#include <gccore.h>
#include <stdio.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void error(const char *msg)
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

    iprintf("\x1b[2;31HAn error occurred.\n");
    iprintf("\t%s\n\n", msg);

	iprintf("\x1b[5;5HRestart the console and confirm that potential error causes are fixed.\n");
	iprintf("\tIf you performed an IGR your SD adapter might not have become ready again.");

    for(;;) {
        VIDEO_WaitVSync();
    }
}
