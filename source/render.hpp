#ifndef RENDER_H
#define RENDER_H


#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include "interpret.hpp"
#include "image.hpp"
#include <chrono>

extern C3D_RenderTarget* topScreen;
extern C3D_RenderTarget* bottomScreen;
extern u32 clrWhite;
extern u32 clrBlack;
extern std::chrono::_V2::system_clock::time_point startTime;
extern std::chrono::_V2::system_clock::time_point endTime;

double degreesToRadians(double degrees);
std::string getUsername();
void renderInit();
void freeImage(Sprite* currentSprite, const std::string& costumeId);
void renderImage(C2D_Image *image,Sprite* currentSprite,std::string costumeId);
void renderSprites();
void renderDeInit();

#endif