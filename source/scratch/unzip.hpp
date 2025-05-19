#include <iostream>
#include <fstream>
#include "interpret.hpp"

bool openScratchFile(std::ifstream *file);

nlohmann::json unzipProject(std::ifstream *file);

bool openScratchProject();
