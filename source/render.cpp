#include "render.hpp"

C3D_RenderTarget* topScreen = C2D_CreateScreenTarget(GFX_TOP,GFX_LEFT);
C3D_RenderTarget* bottomScreen = C2D_CreateScreenTarget(GFX_BOTTOM,GFX_LEFT);

void renderInit(){
    //gfxInitDefault(); // loads graphics
   // C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	//C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	//C2D_Prepare();
}

void renderDeInit(){
    C2D_Fini();
    C3D_Fini();
}