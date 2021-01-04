#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

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


	printf("Hello World!\n");

	while(1) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		PAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = PAD_ButtonsDown(0);

		// Print "A pressed" when A button is pressed down
		if (pressed & PAD_BUTTON_A){
			printf("A pressed\n");
		}

		//Check if A is being held
		u32 held = PAD_ButtonsHeld(0);
		if (held & PAD_BUTTON_A){
			printf("A held\n");
		}

		//Indicate A was released
		u32 buttonUp = PAD_ButtonsUp(0);
		if(buttonUp & PAD_BUTTON_A){
			printf("A released\n");
		}

		if(PAD_StickY(0) > 18){
			printf("Joystick moved up\n");
		}

		if(PAD_StickY(0) < -18){
			printf("Joystick moved down\n");
		}

		if(PAD_StickX(0) > 18){
			printf("Joystick moved right\n");
		}

		if(PAD_StickX(0) < -18){
			printf("Joystick moved left\n");
		}

		//Exit program when start button pressed
		if ( pressed & PAD_BUTTON_START ) exit(0);

		// Wait for the next frame
		VIDEO_WaitVSync();
	}

	return 0;
}
