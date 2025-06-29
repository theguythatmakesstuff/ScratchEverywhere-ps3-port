#pragma once
#include "../blockExecutor.hpp"

class EventBlocks{
public:
    static BlockResult flagClicked(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult broadcast(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult whenKeyPressed(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
};