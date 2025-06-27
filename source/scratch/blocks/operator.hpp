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

    static bool equals(const Block& block, Sprite* sprite);
    static bool greaterThan(const Block& block, Sprite* sprite);
    static bool lessThan(const Block& block, Sprite* sprite);
    static bool and_(const Block& block, Sprite* sprite);
    static bool or_(const Block& block, Sprite* sprite);
    static bool not_(const Block& block, Sprite* sprite);
    static bool contains(const Block& block, Sprite* sprite);
};