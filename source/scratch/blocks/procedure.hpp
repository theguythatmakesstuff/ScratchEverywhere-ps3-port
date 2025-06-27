#pragma once
#include "../blockExecutor.hpp"

class ProcedureBlocks{
public:
    static std::string stringNumber(const Block& block, Sprite* sprite);

    static bool booleanArgument(const Block& block, Sprite* sprite);
};