#pragma once
#include "../blockExecutor.hpp"

class DataBlocks{
public:
    static BlockResult setVariable(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult changeVariable(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult addToList(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult deleteFromList(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult deleteAllOfList(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult insertAtList(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);
    static BlockResult replaceItemOfList(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh);

    static std::string itemOfList(const Block& block, Sprite* sprite);
    static std::string itemNumOfList(const Block& block, Sprite* sprite);
    static std::string lengthOfList(const Block& block, Sprite* sprite);

    static bool listContainsItem(const Block& block, Sprite* sprite);
};