#pragma once
#include "../blockExecutor.hpp"

class DataBlocks{
public:
    static BlockResult setVariable(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult changeVariable(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult addToList(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult deleteFromList(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult deleteAllOfList(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult insertAtList(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult replaceItemOfList(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);

    static std::string itemOfList(const Block& block, Sprite* sprite);
    static std::string itemNumOfList(const Block& block, Sprite* sprite);
    static std::string lengthOfList(const Block& block, Sprite* sprite);

    static bool listContainsItem(const Block& block, Sprite* sprite);
};