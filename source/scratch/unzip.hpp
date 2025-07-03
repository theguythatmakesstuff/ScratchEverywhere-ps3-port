#include <iostream>
#include <fstream>
#include "interpret.hpp"

class Unzip{
public:
    static volatile int projectOpened;
    static volatile bool threadFinished;
    
    static void openScratchProject(void* arg);

    static bool openScratchFile(std::ifstream *file);

    static nlohmann::json unzipProject(std::ifstream *file);

    static bool openFile(std::ifstream *file);

    static bool load();

};