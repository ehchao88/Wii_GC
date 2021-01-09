//Simple program that has an interactive cursor controlled by the joystick and changes the color of the cursor based on pressing A,B,X,Y buttons in GC controller.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;


/*
Drawing functions taken from the examples given at this page http://www.codemii.com/2008/08/31/tutorial-4-cursors/
*/
void DrawHLine (int x1, int x2, int y, int color) {
    int i;
    y = 320 * y;
    x1 >>= 1;
    x2 >>= 1;
    for (i = x1; i <= x2; i++) {
        u32 *tmpfb = xfb;
        tmpfb[y+i] = color;
    }
}

void DrawVLine (int x, int y1, int y2, int color) {
    int i;
    x >>= 1;
    for (i = y1; i <= y2; i++) {
        u32 *tmpfb = xfb;
        tmpfb[x + (640 * i) / 2] = color;
    }
}

void DrawBox (int x1, int y1, int x2, int y2, int color) {
    DrawHLine (x1, x2, y1, color);
    DrawHLine (x1, x2, y2, color);
    DrawVLine (x1, y1, y2, color);
    DrawVLine (x2, y1, y2, color);
}

void init(){
		// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	PAD_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	printf("\x1b[2;0H");
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	init();

	DrawBox (300, 250, 301, 251, COLOR_WHITE);
	int xPos = 300;
	int yPos = 250;
	int color = COLOR_WHITE;
	while(1) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		PAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = PAD_ButtonsDown(0);

		if(PAD_StickY(0) > 18){
			yPos--;
		}

		if(PAD_StickY(0) < -18){
			yPos++;
		}

		if(PAD_StickX(0) > 18){
			xPos++;
		}

		if(PAD_StickX(0) < -18){
			xPos--;
		}

		//Detect ABXY button press and change the color of the cursor accordingly
		if (pressed & PAD_BUTTON_A){
			color = COLOR_GREEN;
		}

		if (pressed & PAD_BUTTON_B){
			color = COLOR_RED;
		}

		if (pressed & PAD_BUTTON_X){
			color = COLOR_PURPLE;
		}

		if (pressed & PAD_BUTTON_Y){
			color = COLOR_WHITE;
		}

		//Exit program when start button pressed
		if ( pressed & PAD_BUTTON_START ) exit(0);
		VIDEO_ClearFrameBuffer (rmode, xfb, COLOR_BLACK);
		DrawBox(xPos, yPos, xPos+1, yPos+1, color);
		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
