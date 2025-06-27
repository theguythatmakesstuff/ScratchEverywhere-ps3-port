#pragma once
#include "../blockExecutor.hpp"

class OperatorBlocks{
public:
    static std::string add(const Block& block, Sprite* sprite);
    static std::string subtract(const Block& block, Sprite* sprite);
    static std::string multiply(const Block& block, Sprite* sprite);
    static std::string divide(const Block& block, Sprite* sprite);
    static std::string random(const Block& block, Sprite* sprite);
    static std::string join(const Block& block, Sprite* sprite);
    static std::string letterOf(const Block& block, Sprite* sprite);
    static std::string length(const Block& block, Sprite* sprite);
    static std::string mod(const Block& block, Sprite* sprite);
    static std::string round(const Block& block, Sprite* sprite);
    static std::string mathOp(const Block& block, Sprite* sprite);
};