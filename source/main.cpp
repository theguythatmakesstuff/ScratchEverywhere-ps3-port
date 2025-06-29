#ifdef __3DS__
#include <3ds.h>
#include <chrono>
#include "scratch/interpret.hpp"
#include "render.hpp"
#include "input.hpp"
#include "unzip.hpp"
#include "oggAudioPlayer.hpp"



static void exitApp(){
	//freeText(); // kill text
	//cleanupSprites(); // delete sprites
	renderDeInit(); // from render.hpp
	romfsExit(); // unload the filesystem
	ndspExit(); // unload audio
	romfsExit(); // unload the filesystem
	//cfguExit(); // i think kills text
	gfxExit();
}

int main(int argc, char **argv)
{
	gfxInitDefault();
	//ndspInit();
	consoleInit(GFX_BOTTOM, NULL);
	renderInit();
	romfsInit();
	initAudioCache();

	// this is for the FPS
	std::chrono::_V2::system_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	std::chrono::_V2::system_clock::time_point endTime = std::chrono::high_resolution_clock::now();

	if(!openScratchProject()){
		exitApp();
		return 0;
	}
	std::cout<<"project loaded!" << std::endl;

	std::cout<<"Running hat blocks"<<std::endl;
	runAllBlocksByOpcode(Block::EVENT_WHENFLAGCLICKED);


	while (aptMainLoop())
	{
		
		endTime = std::chrono::high_resolution_clock::now();
		if(endTime - startTime >= std::chrono::milliseconds(1000 / FPS)){
			startTime = std::chrono::high_resolution_clock::now();
			getInput();
			BlockExecutor::runRepeatBlocks();
			renderSprites();
		}

		gspWaitForVBlank();

		hidScanInput();
    u32 kDown = hidKeysHeld();
		if(kDown & KEY_START){
			toExit = true;
		}


		if(toExit){
			break;
		}
	}
	

	exitApp();
	return 0;
}


#endif