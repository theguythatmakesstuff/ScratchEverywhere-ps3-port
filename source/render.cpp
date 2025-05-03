#include "render.hpp"

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

C3D_RenderTarget* topScreen = nullptr;
C3D_RenderTarget* bottomScreen = nullptr;
u32 clrWhite = C2D_Color32f(1,1,1,1);
u32 clrBlack = C2D_Color32f(0,0,0,1);


void renderInit(){
   C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
    topScreen = C2D_CreateScreenTarget(GFX_TOP,GFX_LEFT);
    bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM,GFX_LEFT);
}

void renderSprites(){

    timer += 1.0 / 60.0;

    // debug stats
    printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
    printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
    printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(topScreen,clrWhite);
    C2D_TargetClear(bottomScreen,clrWhite);
    C2D_SceneBegin(topScreen);
    for(Sprite& currentSprite : sprites){
        if(currentSprite.isStage || !currentSprite.visible)continue;
        //std::cout << "Rendering: " << currentSprite.name << "at x: " << currentSprite.xPosition << "y: " << currentSprite.yPosition << "\n";
        //C2D_DrawLine(SCREEN_WIDTH / 2,0,clrBlack,SCREEN_WIDTH/2,SCREEN_HEIGHT,clrBlack,3,0);
        C2D_DrawRectSolid(currentSprite.xPosition + (SCREEN_WIDTH / 2),(currentSprite.yPosition * -1) + (SCREEN_HEIGHT/ 2),1,10,10,clrBlack);
    }
    //C2D_Flush();
    C3D_FrameEnd(0);
}

void renderDeInit(){
    C2D_Fini();
    C3D_Fini();
}