#pragma once
#include "../blockExecutor.hpp"

class SensingBlocks{
public:
    static BlockResult resetTimer(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult askAndWait(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
};