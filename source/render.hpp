#ifndef RENDER_H
#define RENDER_H


#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include "interpret.hpp"

extern C3D_RenderTarget* topScreen;
extern C3D_RenderTarget* bottomScreen;
extern u32 clrWhite;
extern u32 clrBlack;

void renderInit();
void renderSprites();
void renderDeInit();

#endif