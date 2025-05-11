#include "render.hpp"

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

C3D_RenderTarget* topScreen = nullptr;
C3D_RenderTarget* bottomScreen = nullptr;
u32 clrWhite = C2D_Color32f(1,1,1,1);
u32 clrBlack = C2D_Color32f(0,0,0,1);
u32 clrGreen = C2D_Color32f(0,0,1,1);
std::chrono::_V2::system_clock::time_point startTime = std::chrono::high_resolution_clock::now();
std::chrono::_V2::system_clock::time_point endTime = std::chrono::high_resolution_clock::now();

int projectWidth = 480;
int projectHeight = 360;


std::string getUsername() {
    const u16* block = (const u16*)malloc(0x1C);

    cfguInit();
    CFGU_GetConfigInfoBlk2(0x1C, 0xA0000, (u8*)block);
    cfguExit();

    char* usernameBuffer = (char*)malloc(0x14);
    ssize_t length = utf16_to_utf8((u8*)usernameBuffer, block, 0x14);

    std::string username;
    if (length <= 0) {
        username = "Player";
    } else {
        username = std::string(usernameBuffer, length); // Convert char* to std::string
    }

    free((void*)block); // Free the memory allocated for block
    free(usernameBuffer); // Free the memory allocated for usernameBuffer

    return username;
}

double degreesToRadians(double degrees) {
    return degrees * (M_PI / 180.0);
}

double getMaxSpriteLayer() {
    double maxLayer = 0.0;
    for (Sprite* currentSprite : sprites) {
        if (currentSprite->layer > maxLayer) {
            maxLayer = currentSprite->layer;
        }
    }
    return maxLayer;
}

void renderInit(){
   C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
    topScreen = C2D_CreateScreenTarget(GFX_TOP,GFX_LEFT);
    bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM,GFX_LEFT);
}

void renderSprites(){

    timer += 1.0 / 60.0;


    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(topScreen,clrWhite);
    C2D_TargetClear(bottomScreen,clrWhite);
    C2D_SceneBegin(topScreen);
    int times = 1;
    for(Sprite* currentSprite : sprites){
        if(!currentSprite->visible)continue;

        // look through every costume in sprite for correct one
        int costumeIndex = 0;
        for(const auto& costume : currentSprite->costumes){
            if(costumeIndex == currentSprite->currentCostume){
                renderImage(&imageC2Ds[costume.id],currentSprite,costume.id);
                // if(imageC2Ds.find(costume.id) != imageC2Ds.end() && costume.dataFormat == "png"){
                //     // Image sprite rendering
                //     double rotation = degreesToRadians(currentSprite.rotation - 90.0f);
                //     C2D_DrawImageAtRotated(imageC2Ds[costume.id],currentSprite.xPosition + (SCREEN_WIDTH / 2),(currentSprite.yPosition * -1) + (SCREEN_HEIGHT / 2),1.0f,rotation,nullptr,0.5f,0.5f);

                   
                // }
                // else{
                //     // fallback render
                //     C2D_DrawRectSolid(currentSprite.xPosition + (SCREEN_WIDTH / 2),(currentSprite.yPosition * -1) + (SCREEN_HEIGHT/ 2),1,10,10,clrBlack);
                // }
            }
            costumeIndex++;
        }
        // if(imageC2Ds[currentSprite.costumes[currentSprite.currentCostume]]){

        // }
        //std::cout << "Rendering: " << currentSprite.name << "at x: " << currentSprite.xPosition << "y: " << currentSprite.yPosition << "\n";
        //C2D_DrawRectSolid(currentSprite.xPosition + (SCREEN_WIDTH / 2),(currentSprite.yPosition * -1) + (SCREEN_HEIGHT/ 2),1,10,10,clrBlack);
        //std::cout << "rendring sprite number " << times << std::endl;
       // std::cout<< getUsername() <<std::endl;
        times++;
    }
    //C2D_Flush();
    C3D_FrameEnd(0);
    endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = endTime - startTime;
    //int FPS = 1000.0 / std::round(duration.count());
   //std::cout << "\x1b[8;0HCPU: " <<C3D_GetProcessingTime()*6.0f<<"\nGPU: "<< C3D_GetDrawingTime()*6.0f << "\nCmdBuf: " <<C3D_GetCmdBufUsage()*100.0f << "\nFPS: " << FPS <<  std::endl;
    startTime = std::chrono::high_resolution_clock::now();
}

void freeImage(Sprite* currentSprite, const std::string& costumeId) {
    // Check if the costume exists in imageC2Ds
    auto it = imageC2Ds.find(costumeId);
    if (it != imageC2Ds.end()) {
        // Free the texture if it exists
        if (it->second.tex) {
            C3D_TexDelete(it->second.tex);
            free(it->second.tex);
           // imageC2Ds.erase(it);
        }

        // Free the subtexture if it exists
        if (it->second.subtex) {
            free((Tex3DS_SubTexture*)it->second.subtex);
        }

        // Erase the costume from imageC2Ds
        imageC2Ds.erase(it);
        std::cout << "Freed image for costume: " << costumeId << std::endl;
    }
}



void renderImage(C2D_Image *image, Sprite* currentSprite, std::string costumeId) {
    //freeImage(currentSprite,costumeId);
    bool legacyDrawing = false;
    if(imageC2Ds.find(costumeId) == imageC2Ds.end() || image->tex == nullptr || image->subtex == nullptr){
        for(ImageRGBA rgba : imageRBGAs){
            if(rgba.name == costumeId){
                //C3D_TexDelete(image->tex);
                //free(image->tex);
                currentSprite->spriteWidth = rgba.width / 2;
                currentSprite->spriteHeight = rgba.height / 2;
                imageC2Ds[costumeId] = get_C2D_Image(rgba);
                break;

            }

        }
        legacyDrawing = true;
    }

    double maxLayer = getMaxSpriteLayer();
    double scaleX = static_cast<double>(SCREEN_WIDTH) / projectWidth;
    double scaleY = static_cast<double>(SCREEN_HEIGHT) / projectHeight;
    double scale = std::min(scaleX, scaleY); // smaller scale is to maintain aspect ratio 
if (!legacyDrawing) {
    double rotation = degreesToRadians(currentSprite->rotation - 90.0f);
    C2D_DrawImageAtRotated(
        *image,
        (currentSprite->xPosition * scale) + (SCREEN_WIDTH / 2),
        (currentSprite->yPosition * -1 * scale) + (SCREEN_HEIGHT / 2),
        currentSprite->layer / maxLayer,
        rotation,
        nullptr,
        (currentSprite->size * 0.01) * scale / 2.0f,
        (currentSprite->size * 0.01) * scale / 2.0f 
    );
} else {
    C2D_DrawRectSolid(
        (currentSprite->xPosition * scale) + (SCREEN_WIDTH / 2),
        (currentSprite->yPosition * -1 * scale) + (SCREEN_HEIGHT / 2),
        currentSprite->layer / maxLayer,
        10 * scale,
        10 * scale, 
        clrBlack
    );
}

    // Draw collision points
    // auto collisionPoints = getCollisionPoints(currentSprite);
    // for (const auto& point : collisionPoints) {
    //     C2D_DrawRectSolid(point.first + (SCREEN_WIDTH / 2), (point.second * -1) + (SCREEN_HEIGHT / 2), 1, 5, 5, clrBlack);
    // }
    // Draw mouse pointer
    C2D_DrawRectSolid(mousePointer.x + (SCREEN_WIDTH / 2), (mousePointer.y * -1) + (SCREEN_HEIGHT / 2), 1, 5, 5, clrGreen);
}

void renderDeInit(){
    C2D_Fini();
    C3D_Fini();
    for(auto &[id,data] : imageC2Ds){
        if(data.tex){
        C3D_TexDelete(data.tex);
        free(data.tex);
        }
    
        if(data.subtex){
            free((Tex3DS_SubTexture*)data.subtex);
        }
    }

}
