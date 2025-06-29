#pragma once
#include "../blockExecutor.hpp"

class LooksBlocks{
public:
    static BlockResult show(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult hide(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult switchCostumeTo(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult nextCostume(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult switchBackdropTo(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult nextBackdrop(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult goForwardBackwardLayers(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult goToFrontBack(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult setSizeTo(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult changeSizeBy(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);

    static std::string size(const Block& block, Sprite* sprite);
    static std::string costume(const Block& block, Sprite* sprite);
    static std::string backdrops(const Block& block, Sprite* sprite);
    static std::string costumeNumberName(const Block& block, Sprite* sprite);
    static std::string backdropNumberName(const Block& block, Sprite* sprite);
};