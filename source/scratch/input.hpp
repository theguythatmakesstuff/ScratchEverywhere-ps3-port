#pragma once
#include <vector>
#include <string>

class Input{
public:
    struct Mouse{
        int x;
        int y;
        bool isPressed;
        bool isMoving;
    };
    static Mouse mousePointer;


    static std::vector<std::string> inputButtons;
    static void getInput();

};