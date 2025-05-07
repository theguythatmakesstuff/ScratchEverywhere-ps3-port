#ifdef __3DS__
#include "miniz.h"
#include <3ds.h>
#include <stdio.h>
#include <citro2d.h>
#include <citro3d.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "interpret.hpp"
#include "render.hpp"
#include "input.hpp"
#include "image.hpp"


// C:/Users/Wiz/Documents/CodingProjects/Scratch


bool openScratchProject(){
	std::cout<<"Unzipping Scratch Project..."<<std::endl;

	// load Scratch project into memory
	std::cout<<"Loading SB3 into memory..."<<std::endl;
	const char* filename = "project.sb3";

	std::ifstream file("romfs:/"+std::string(filename), std::ios::binary | std::ios::ate); // loads file from romfs
	if (!file){
		std::cerr<<"No embedded Scratch project, trying SD card";
		file.open(filename, std::ios::binary | std::ios::ate); // loads file from location of executable
		if (!file){
			std::cerr<<"Couldnt find file. jinkies.";
			svcBreak(USERBREAK_PANIC);
			return false;
		}
	}



	// read the file
	std::cout<<"Reading SB3..."<<std::endl;
	std::streamsize size = file.tellg(); // gets the size of the file
	file.seekg(0,std::ios::beg); // go to the beginning of the file
	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size)){
		svcBreak(USERBREAK_PANIC);
		return false;
	}

	// open ZIP file from the thing that we just did
	std::cout<<"Opening SB3 file..."<<std::endl;
	mz_zip_archive zip;
	memset(&zip,0,sizeof(zip));
	if (!mz_zip_reader_init_mem(&zip,buffer.data(),buffer.size(),0)){
		svcBreak(USERBREAK_PANIC);
		return false;
	}

	// extract project.json
	std::cout<<"Extracting project.json..."<<std::endl;
	int file_index = mz_zip_reader_locate_file(&zip,"project.json",NULL,0);
	if (file_index < 0){
		svcBreak(USERBREAK_PANIC);
		return false;
	}

	size_t json_size;
	const char* json_data = static_cast<const char*>(mz_zip_reader_extract_to_heap(&zip, file_index, &json_size, 0));

	// Parse JSON file
	std::cout<<"Parsing project.json..."<<std::endl;
	nlohmann::json project_json = nlohmann::json::parse(std::string(json_data,json_size));
	mz_free((void*)json_data);
	


loadImages(&zip);
mz_zip_reader_end(&zip);

loadSprites(project_json);
std::cout << "DONE!" << std::endl;
return true;
}

static void exitApp(){
	// Deinit libs
	//C2D_SpriteSheetFree(spriteSheet); // delete sprites
	//freeText(); // kill text
	renderDeInit(); // from render.hpp
	romfsExit(); // unload the filesystem
	//ndspExit(); // unload audio
	romfsExit(); // unload the filesystem
	//cfguExit(); // i think kills text
	gfxExit();
}

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	renderInit();
	romfsInit();





	if(!openScratchProject()){
		std::cerr<<"Failed to open Scratch project."<<std::endl;
		exitApp();
	}
	
	std::cout<<"Running hat blocks"<<std::endl;
	runAllBlocksByOpcode(Block::EVENT_WHENFLAGCLICKED);



	while (aptMainLoop())
	{
		getInput();
		runRepeatBlocks();
		renderSprites();
		
		//gspWaitForVBlank();

		if(toExit){
			break;
		}
	}

	exitApp();
	return 0;
}


#endif