#pragma once
#include "../blockExecutor.hpp"

class EventBlocks{
public:
    static BlockResult flagClicked(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
};