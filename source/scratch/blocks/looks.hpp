#pragma once
#include "../blockExecutor.hpp"

class LooksBlocks{
public:
    static BlockResult show(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult hide(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult switchCostumeTo(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult nextCostume(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult switchBackdropTo(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult nextBackdrop(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult goForwardBackwardLayers(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult goToFrontBack(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult setSizeTo(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult changeSizeBy(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
};