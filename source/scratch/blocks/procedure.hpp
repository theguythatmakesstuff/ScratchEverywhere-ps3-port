#pragma once
#include "../blockExecutor.hpp"

class ProcedureBlocks{
public:
    static BlockResult call(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);
    static BlockResult definition(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh);

    static std::string stringNumber(const Block& block, Sprite* sprite);

    static bool booleanArgument(const Block& block, Sprite* sprite);
};