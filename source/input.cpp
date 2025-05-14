#include "input.hpp"

std::vector<std::string> inputButtons;

void getInput(){
    inputButtons.clear();
    mousePointer.isPressed = false;
    mousePointer.isMoving = false;
    hidScanInput();
    u32 kDown = hidKeysHeld();

    touchPosition touch;

    //Read the touch screen coordinates
    hidTouchRead(&touch);

    if(kDown){
        inputButtons.push_back("any");
        if(kDown & KEY_A){
            inputButtons.push_back("a");
        }
        if(kDown & KEY_B){
            inputButtons.push_back("b");
        }
        if(kDown & KEY_X){
            inputButtons.push_back("x");
        }
        if(kDown & KEY_Y){
            inputButtons.push_back("y");
        }
        if(kDown & KEY_SELECT){
            inputButtons.push_back("0");
        }
        if(kDown & KEY_START){
            inputButtons.push_back("1");
        }
        if(kDown & KEY_DUP){
            if(kDown & KEY_L)
            mousePointer.y += 3;
            else
            inputButtons.push_back("u");
        }
        if(kDown & KEY_DDOWN){
            if(kDown & KEY_L)
            mousePointer.y -= 3;
            else
            inputButtons.push_back("h");
        }
        if(kDown & KEY_DLEFT){
            if(kDown & KEY_L)
            mousePointer.x -= 3;
            else
            inputButtons.push_back("g");
        }
        if(kDown & KEY_DRIGHT){
            if(kDown & KEY_L)
            mousePointer.x += 3;
            else
            inputButtons.push_back("j");
        }
        if(kDown & KEY_L){
            inputButtons.push_back("l");
            mousePointer.isMoving = true;
        }
        if(kDown & KEY_R){
            if(kDown & KEY_L)
            mousePointer.isPressed = true;
            else
            inputButtons.push_back("r");
        }
        if(kDown & KEY_ZL){
            inputButtons.push_back("z");
        }
        if(kDown & KEY_ZR){
            inputButtons.push_back("x");
        }
        if(kDown & KEY_CPAD_UP){
            inputButtons.push_back("up arrow");
        }
        if(kDown & KEY_CPAD_DOWN){
            inputButtons.push_back("down arrow");
        }
        if(kDown & KEY_CPAD_LEFT){
            inputButtons.push_back("left arrow");
        }
        if(kDown & KEY_CPAD_RIGHT){
            inputButtons.push_back("right arrow");
        }
        if(kDown & KEY_CSTICK_UP){
            inputButtons.push_back("2");
        }
        if(kDown & KEY_CSTICK_DOWN){
            inputButtons.push_back("3");
        }
        if(kDown & KEY_CSTICK_LEFT){
            inputButtons.push_back("4");
        }
        if(kDown & KEY_CSTICK_RIGHT){
            inputButtons.push_back("5");
        }
        if(kDown & KEY_TOUCH){
            mousePointer.isPressed = true;
            mousePointer.x = touch.px - (SCREEN_WIDTH / 2);
            mousePointer.y = (-touch.py + (SCREEN_HEIGHT / 2)) -SCREEN_HEIGHT;
        }
        
    }

}