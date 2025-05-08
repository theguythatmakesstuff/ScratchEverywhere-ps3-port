#include "input.hpp"

std::vector<std::string> inputButtons;

void getInput(){
    inputButtons.clear();
    mousePointer.isPressed = false;
    hidScanInput();
    u32 kDown = hidKeysHeld();
    if(kDown){
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
            inputButtons.push_back("up arrow");
            mousePointer.y += 3;
        }
        if(kDown & KEY_DDOWN){
            inputButtons.push_back("down arrow");
            mousePointer.y -= 3;
        }
        if(kDown & KEY_DLEFT){
            inputButtons.push_back("left arrow");
            mousePointer.x -= 3;
        }
        if(kDown & KEY_DRIGHT){
            inputButtons.push_back("right arrow");
            mousePointer.x += 3;
        }
        if(kDown & KEY_L){
            inputButtons.push_back("l");
        }
        if(kDown & KEY_R){
            inputButtons.push_back("r");
            mousePointer.isPressed = true;
        }
        if(kDown & KEY_ZL){
            inputButtons.push_back("z");
        }
        if(kDown & KEY_ZR){
            inputButtons.push_back("x");
        }
        if(kDown & KEY_CPAD_UP){
            inputButtons.push_back("w");
        }
        if(kDown & KEY_CPAD_DOWN){
            inputButtons.push_back("s");
        }
        if(kDown & KEY_CPAD_LEFT){
            inputButtons.push_back("a");
        }
        if(kDown & KEY_CPAD_RIGHT){
            inputButtons.push_back("d");
        }
        if(kDown & KEY_CSTICK_UP){
            inputButtons.push_back("y");
        }
        if(kDown & KEY_CSTICK_DOWN){
            inputButtons.push_back("h");
        }
        if(kDown & KEY_CSTICK_LEFT){
            inputButtons.push_back("g");
        }
        if(kDown & KEY_CSTICK_RIGHT){
            inputButtons.push_back("j");
        }
        
    }

}