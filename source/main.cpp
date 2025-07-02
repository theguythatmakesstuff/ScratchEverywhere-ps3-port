#ifdef __3DS__
#include <3ds.h>
#include <chrono>
#include <thread>
#include "scratch/blockExecutor.hpp"
#include "render.hpp"
#include "input.hpp"
#include "unzip.hpp"

// arm-none-eabi-addr2line -e Scratch.elf xxx
// ^ for debug purposes

static void exitApp(){
	renderDeInit();
	romfsExit();
	ndspExit();
	romfsExit();
	gfxExit();
}

static void initApp(){
	gfxInitDefault();
	hidScanInput();
    u32 kDown = hidKeysHeld();
	if(kDown & KEY_SELECT) consoleInit(GFX_BOTTOM, NULL);
	osSetSpeedupEnable(true);
	renderInit();
	romfsInit();
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

    s32 mainPrio = 0;
    svcGetThreadPriority(&mainPrio, CUR_THREAD_HANDLE);

	Thread projectThread = threadCreate(
        openScratchProject,
        NULL,
        0x4000,
        mainPrio - 1,
        -1,
        false
    );

    if(!projectThread) {
        std::cerr << "Failed to create thread!" << std::endl;
        exitApp();
        return -1;
    }
  
	LoadingScreen loading;
	loading.init();
  
    while(!threadFinished){
		loading.renderLoadingScreen();
		gspWaitForVBlank();
    }
	threadJoin(projectThread, U64_MAX);
    threadFree(projectThread);
	if(projectOpened != 1){

		if(projectOpened == -1)
		loading.text->setText("Loading failed!\nCouldn't find a scratch project...\nis it named 'project.sb3'??\nStart to exit.");
		if(projectOpened == -2)
		loading.text->setText("Loading failed!\nproject.json is empty...\nStart to exit.");
		loading.renderLoadingScreen();

		while(aptMainLoop()){
			hidScanInput();
			if(hidKeysDown() & KEY_START){
				break;
			}
			gspWaitForVBlank();
		}
		loading.cleanup();
		exitApp();
		return 0;
	}

	loading.cleanup();

	std::cout<<"project loaded!" << std::endl;

	// disable new 3ds clock speeds for a bit cus it crashes for some reason otherwise????
	osSetSpeedupEnable(false);

	std::cout<<"Running hat blocks"<<std::endl;
	BlockExecutor::runAllBlocksByOpcode(Block::EVENT_WHENFLAGCLICKED);
	BlockExecutor::timer = std::chrono::high_resolution_clock::now();


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
			std::cout << "\x1b[17;1HFrame time: " << frameDuration.count() << " ms" << std::endl;
			std::cout << "\x1b[18;1HSprites: " << sprites.size() << std::endl;
			
		}

		gspWaitForVBlank();
		osSetSpeedupEnable(true);

		hidScanInput();

		if(toExit){
			break;
		}
	}
	

	exitApp();
	return 0;
}


#endif