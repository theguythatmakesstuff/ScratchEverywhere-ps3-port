#include "unzip.hpp"

volatile int projectOpened = 0;
volatile bool threadFinished = false;

bool openFile(std::ifstream *file){
    std::cout<<"Unzipping Scratch Project..."<<std::endl;

    // load Scratch project into memory
    std::cout<<"Loading SB3 into memory..."<<std::endl;
    const char* filename = "project.sb3";
    const char* unzippedPath = "romfs:/project/project.json";

    //first try embedded unzipped project
    file->open(unzippedPath, std::ios::binary | std::ios::ate);
    projectType = UNZIPPED;
    if(!(*file)){
        std::cerr<<"No unzipped project, trying embedded."<<std::endl;

        // try embedded zipped sb3
        file->open("romfs:/"+std::string(filename), std::ios::binary | std::ios::ate); // loads file from romfs
        projectType = EMBEDDED;
        if (!(*file)){
            std::cerr<<"No embedded Scratch project, trying SD card"<<std::endl;

            // then try SD card location
            file->open(filename, std::ios::binary | std::ios::ate); // loads file from location of executable
            projectType = UNEMBEDDED;
            if (!(*file)){
                std::cerr<<"Couldnt find file. jinkies.";
                svcBreak(USERBREAK_PANIC);
                return false;
            }
        }
    }
    return true;
}

nlohmann::json unzipProject(std::ifstream *file){

    nlohmann::json project_json;

    if(!projectType == UNZIPPED){
        // read the file
        std::cout<<"Reading SB3..."<<std::endl;
        std::streamsize size = file->tellg(); // gets the size of the file
        file->seekg(0,std::ios::beg); // go to the beginning of the file
        std::vector<char> buffer(size);
        if (!file->read(buffer.data(), size)){
            svcBreak(USERBREAK_PANIC);
            return project_json;
        }

        // open ZIP file from the thing that we just did
        std::cout<<"Opening SB3 file..."<<std::endl;
        mz_zip_archive zip;
        memset(&zip,0,sizeof(zip));
        if (!mz_zip_reader_init_mem(&zip,buffer.data(),buffer.size(),0)){
            svcBreak(USERBREAK_PANIC);
            return project_json;
        }

        // extract project.json
        std::cout<<"Extracting project.json..."<<std::endl;
        int file_index = mz_zip_reader_locate_file(&zip,"project.json",NULL,0);
        if (file_index < 0){
            svcBreak(USERBREAK_PANIC);
            return project_json;
        }

        size_t json_size;
        const char* json_data = static_cast<const char*>(mz_zip_reader_extract_to_heap(&zip, file_index, &json_size, 0));

        // Parse JSON file
        std::cout<<"Parsing project.json..."<<std::endl;
        project_json = nlohmann::json::parse(std::string(json_data,json_size));
        mz_free((void*)json_data);

        loadImages(&zip);
        mz_zip_reader_end(&zip);
    }
    else {
        // if project is unzipped
    file->clear(); // Clear any EOF flags
    file->seekg(0, std::ios::beg); // Go to the start of the file
    (*file) >> project_json;
}

    return project_json;
}

void openScratchProject(void* arg){
    	std::ifstream file;
	if(!openFile(&file)){
		std::cerr<<"Failed to open Scratch project."<<std::endl;
        projectOpened = -1;
        threadFinished = true;
		return;
	}
	nlohmann::json project_json = unzipProject(&file);
    if(project_json.empty()){
        std::cerr<<"Project.json is empty."<<std::endl;
        projectOpened = -2;
        threadFinished = true;
        return;
    }
	loadSprites(project_json);
    projectOpened = 1;
    threadFinished = true;
    return;
}