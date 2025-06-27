#pragma once
#include "../blockExecutor.hpp"

class SensingBlocks{
public:
    static BlockResult resetTimer(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
    static BlockResult askAndWait(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);

    static std::string sensingTimer(const Block& block, Sprite* sprite);
    static std::string of(const Block& block, Sprite* sprite);
    static std::string mouseX(const Block& block, Sprite* sprite);
    static std::string mouseY(const Block& block, Sprite* sprite);
    static std::string distanceTo(const Block& block, Sprite* sprite);
    static std::string daysSince2000(const Block& block, Sprite* sprite);
    static std::string current(const Block& block, Sprite* sprite);
    static std::string sensingAnswer(const Block& block, Sprite* sprite);
};