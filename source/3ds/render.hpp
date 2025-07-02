#ifndef RENDER_H
#define RENDER_H
#define SCREEN_WIDTH 400
#define BOTTOM_SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include "interpret.hpp"
#include "image.hpp"
#include "text.hpp"
#include <chrono>

extern C3D_RenderTarget* topScreen;
extern C3D_RenderTarget* bottomScreen;
extern u32 clrWhite;
extern u32 clrBlack;
extern std::chrono::_V2::system_clock::time_point startTime;
extern std::chrono::_V2::system_clock::time_point endTime;
extern int projectWidth;
extern int projectHeight;
extern int FPS;
extern bool bottomScreenEnabled;

double degreesToRadians(double degrees);
double getMaxSpriteLayer();
std::string getUsername();
void renderInit();
void renderImage(C2D_Image *image,Sprite* currentSprite,std::string costumeId,bool bottom = false);

void renderSprites();
void renderDeInit();

class LoadingScreen{
private:
    TextObject* text;
public:
    void init();
    void renderLoadingScreen();
    void cleanup();
};

#endif