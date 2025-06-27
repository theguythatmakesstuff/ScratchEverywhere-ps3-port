#include "sensing.hpp"

BlockResult SensingBlocks::resetTimer(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    timer = 0;
    return BlockResult::CONTINUE;
}

BlockResult SensingBlocks::askAndWait(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    Keyboard kbd;
    std::string inputValue = Scratch::getInputValue(block.inputs.at("QUESTION"),&block,sprite);
    std::string output = kbd.openKeyboard(inputValue.c_str());
    answer = output;
    return BlockResult::CONTINUE;
}