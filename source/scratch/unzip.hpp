#include <iostream>
#include <fstream>
#include "interpret.hpp"

bool openScratchFile(std::ifstream *file);

nlohmann::json unzipProject(std::ifstream *file);

void openScratchProject(void* arg);

extern volatile int projectOpened;
extern volatile bool threadFinished;