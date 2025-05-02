#ifndef RENDER_H
#define RENDER_H


#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>

extern C3D_RenderTarget* topScreen;
extern C3D_RenderTarget* bottomScreen;

void renderInit();

void renderDeInit();

#endif