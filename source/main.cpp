#ifdef __3DS__
#include <3ds.h>
#include <stdio.h>
#include <citro2d.h>
#include "miniz.h"
#include <iostream>
#include <fstream>
// C:/Users/Wiz/Documents/CodingProjects/Scratch


int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on top screen.
	consoleInit(GFX_TOP, NULL);

	//printf("\x1b[16;20 yo press the fucking A button you twerp. NOW.");

	//printf("\x1b[30;16HPress Start to exit.");


	// load Scratch project into memory
	const char* filename = "project.sb3";
	std::ifstream file(filename, std::ios::binary | std::ios::ate); // loads file from root(?) of SD card
	if (!file){
		printf("\x1b[16;20 Errrm well this is awkward... couldnt find the file... jinkies...");
		goto exit;
	}
	else{
		printf("\x1b[16;20 BAAAAAAAAAAANG FOUND THAT SHIT BAAAAAAAANG ANOTHER DAY IN THE OFFICE BABY");
	}

	// open ZIP file from the thing that we just did



	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		if (kDown & KEY_A)
		{
			consoleInit(GFX_TOP, NULL);
			printf("\x1b[16;20 good boy.");
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}
	exit:

	gfxExit();
	return 0;
}


#endif