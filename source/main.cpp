#ifdef __3DS__
#include <3ds.h>
#include <chrono>
#include "scratch/interpret.hpp"
#include "render.hpp"
#include "input.hpp"
#include "unzip.hpp"
#include "oggAudioPlayer.hpp"

// arm-none-eabi-addr2line -e Scratch.elf xxx
// ^ for debug perposes

static void exitApp(){
	//freeText(); // kill text
	//cleanupSprites(); // delete sprites
	renderDeInit(); // from render.hpp
	romfsExit(); // unload the filesystem
	ndspExit(); // unload audio
	romfsExit(); // unload the filesystem
	gfxExit();
}

static void initApp(){
	gfxInitDefault();
	//ndspInit();

	hidScanInput();
    u32 kDown = hidKeysHeld();
	if(kDown & KEY_SELECT) consoleInit(GFX_BOTTOM, NULL);

	renderInit();
	romfsInit();
	initAudioCache();
}

int main(int argc, char **argv)
{
	initApp();
	
	// this is for the FPS
	std::chrono::_V2::system_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	std::chrono::_V2::system_clock::time_point endTime = std::chrono::high_resolution_clock::now();
	//this is for frametime check
	std::chrono::_V2::system_clock::time_point frameStartTime = std::chrono::high_resolution_clock::now();
	std::chrono::_V2::system_clock::time_point frameEndTime = std::chrono::high_resolution_clock::now();

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
			frameStartTime = std::chrono::high_resolution_clock::now();
			getInput();
			BlockExecutor::runRepeatBlocks();
			renderSprites();
			frameEndTime = std::chrono::high_resolution_clock::now();
			auto frameDuration = frameEndTime - frameStartTime;
			std::cout << "\x1b[8;1HFrame time: " << frameDuration.count() << " ns" << std::endl;
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