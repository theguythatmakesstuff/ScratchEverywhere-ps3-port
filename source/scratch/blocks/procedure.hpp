#pragma once
#include "../blockExecutor.hpp"

class ProcedureBlocks{
public:
    static BlockResult call(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult definition(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);

    static std::string stringNumber(const Block& block, Sprite* sprite);

    static bool booleanArgument(const Block& block, Sprite* sprite);
};