#pragma once
#include "../blockExecutor.hpp"

class ControlBlocks{
public:
    static BlockResult If(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult ifElse(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult createCloneOf(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult deleteThisClone(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult stop(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult startAsClone(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult wait(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult waitUntil(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult repeat(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult repeatUntil(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult forever(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
};