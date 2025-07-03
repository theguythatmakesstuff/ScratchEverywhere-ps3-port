#include "../scratch/render.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

void Render::Init(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    window = SDL_CreateWindow("Scratch Runtime",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,480,360,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

}
void Render::deInit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
void Render::renderSprites(){


    SDL_RenderPresent(renderer);
}
bool Render::appShouldRun(){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            return false;
        }
    }
    return true;
}

void LoadingScreen::init(){

}
void LoadingScreen::renderLoadingScreen(){

}
void LoadingScreen::cleanup(){

}