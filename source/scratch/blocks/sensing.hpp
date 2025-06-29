#pragma once
#include "../blockExecutor.hpp"

class SensingBlocks{
public:
    static BlockResult resetTimer(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult askAndWait(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);

    static std::string sensingTimer(const Block& block, Sprite* sprite);
    static std::string of(const Block& block, Sprite* sprite);
    static std::string mouseX(const Block& block, Sprite* sprite);
    static std::string mouseY(const Block& block, Sprite* sprite);
    static std::string distanceTo(const Block& block, Sprite* sprite);
    static std::string daysSince2000(const Block& block, Sprite* sprite);
    static std::string current(const Block& block, Sprite* sprite);
    static std::string sensingAnswer(const Block& block, Sprite* sprite);

    static bool keyPressed(const Block& block,Sprite* sprite);
    static bool touchingObject(const Block& block, Sprite* sprite);
    static bool mouseDown(const Block& block, Sprite* sprite);
};