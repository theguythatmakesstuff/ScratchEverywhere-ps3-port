#include "../scratch/render.hpp"
#include "render.hpp"
#include "interpret.hpp"
int windowWidth = 480;
int windowHeight = 360;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;


void Render::Init(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    window = SDL_CreateWindow("Scratch Runtime",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,windowWidth,windowHeight,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

}
void Render::deInit(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
void Render::renderSprites(){
    SDL_GetWindowSizeInPixels(window,&windowWidth,&windowHeight);
    SDL_SetWindowSize(window,Scratch::projectWidth,Scratch::projectHeight);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    double scaleX = static_cast<double>(windowWidth) / Scratch::projectWidth;
    double scaleY = static_cast<double>(windowHeight) / Scratch::projectHeight;
    double scale;
    scale = std::min(scaleX, scaleY);

    // Sort sprites by layer first
    std::vector<Sprite*> spritesByLayer = sprites;
    std::sort(spritesByLayer.begin(), spritesByLayer.end(), 
    [](const Sprite* a, const Sprite* b) {
        return a->layer < b->layer;
    });


    for(Sprite* currentSprite : spritesByLayer) {
        if(!currentSprite->visible) continue;

        bool legacyDrawing = false;
        auto imgFind = images.find(currentSprite->costumes[currentSprite->currentCostume].id); // long ahh line
        if(imgFind == images.end()){
            legacyDrawing = true;
        }
        if(!legacyDrawing){
            SDL_Image* image = &imgFind->second;
            SDL_RendererFlip flip = SDL_FLIP_NONE;

            currentSprite->spriteWidth = image->width;
            currentSprite->spriteHeight = image->height;

            image->setScale(currentSprite->size * 0.01);
            image->renderRect.x = currentSprite->xPosition;
            image->renderRect.y = currentSprite->yPosition;
            image->setRotation(Math::degreesToRadians(currentSprite->rotation - 90.0f));

            if(currentSprite->rotationStyle == currentSprite->LEFT_RIGHT){
                if(image->rotation < 0){
                    flip = SDL_FLIP_HORIZONTAL;
                }
                image->setRotation(0);
            }
            if(currentSprite->rotationStyle == currentSprite->NONE){
                image->setRotation(0);
            }
            
            image->renderRect.x = image->renderRect.x + windowWidth;
            image->renderRect.y = image->renderRect.y + windowHeight;

            SDL_RenderCopyEx(renderer,image->spriteTexture,&image->textureRect,&image->renderRect,image->rotation,nullptr,flip);

        }
        else{
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect rect;
            rect.x = (currentSprite->xPosition * scale) + (windowWidth / 2);
            rect.y = (currentSprite->yPosition * -1 * scale) + (windowHeight * 0.5);
            SDL_RenderDrawRect(renderer,&rect);
        }


    }


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


// i dont think we need these tbh
void LoadingScreen::init(){

}
void LoadingScreen::renderLoadingScreen(){

}
void LoadingScreen::cleanup(){

}