#pragma once
#include "../blockExecutor.hpp"

class ControlBlocks{
public:
    static BlockResult If(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult ifElse(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult createCloneOf(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult deleteThisClone(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult stop(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
};