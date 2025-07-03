#pragma once
#include <vector>
#include <cmath>

class Render{
public:
    
    static void Init();
    static void deInit();
    static void renderSprites();

};

class LoadingScreen{
private:
    struct squareObject{
        float x,y;
        float size;
    };
    std::vector<squareObject> squares;
    void createSquares(int count) {
        for (int i = 0; i < count; i++) {
            squareObject square;
            square.x = rand() % 400;
            square.y = 260;
            square.size = rand() % 20;
            squares.push_back(square);
        }
    }
public:
    //TextObject* text;
    void init();
    void renderLoadingScreen();
    void cleanup();
};