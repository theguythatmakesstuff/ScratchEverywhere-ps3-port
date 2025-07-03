#include <chrono>
#include <thread>
#include "scratch/blockExecutor.hpp"
#include "scratch/render.hpp"
#include "scratch/input.hpp"
#include "scratch/unzip.hpp"

// arm-none-eabi-addr2line -e Scratch.elf xxx
// ^ for debug purposes

static void exitApp(){
	Render::deInit();
}

static void initApp(){
	Render::Init();
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

	if(!Unzip::load()){
		exitApp();
		return 0;
	}

	BlockExecutor::runAllBlocksByOpcode(Block::EVENT_WHENFLAGCLICKED);
	BlockExecutor::timer = std::chrono::high_resolution_clock::now();


	while (Render::appShouldRun())
	{
		
		endTime = std::chrono::high_resolution_clock::now();
		if(endTime - startTime >= std::chrono::milliseconds(1000 / Scratch::FPS)){
			startTime = std::chrono::high_resolution_clock::now();
			frameStartTime = std::chrono::high_resolution_clock::now();

			Input::getInput();
			BlockExecutor::runRepeatBlocks();
			Render::renderSprites();

			frameEndTime = std::chrono::high_resolution_clock::now();
			auto frameDuration = frameEndTime - frameStartTime;
			//std::cout << "\x1b[17;1HFrame time: " << frameDuration.count() << " ms" << std::endl;
			//std::cout << "\x1b[18;1HSprites: " << sprites.size() << std::endl;
			
		}
		if(toExit){
			break;
		}
	}
	

	exitApp();
	return 0;
}