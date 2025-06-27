#pragma once
#include "../blockExecutor.hpp"

class MotionBlocks{
public:
    static BlockResult moveSteps(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
};