#include "../scratch/unzip.hpp"

volatile int Unzip::projectOpened;
volatile bool Unzip::threadFinished;

void Unzip::openScratchProject(void* arg){

}

bool Unzip::openScratchFile(std::ifstream *file){
    return true;
}

nlohmann::json Unzip::unzipProject(std::ifstream *file){
    nlohmann::json poo;
    return poo;
}

bool Unzip::openFile(std::ifstream *file){
    return true;
}

bool Unzip::load(){
    return true;
}