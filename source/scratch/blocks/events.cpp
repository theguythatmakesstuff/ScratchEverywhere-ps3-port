#include "events.hpp"

BlockResult EventBlocks::flagClicked(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    return BlockResult::CONTINUE;
}

BlockResult EventBlocks::broadcast(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    broadcastQueue.push_back( Scratch::getInputValue(block.inputs.at("BROADCAST_INPUT"), &block, sprite));
    return BlockResult::CONTINUE;
}

BlockResult EventBlocks::whenKeyPressed(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    for (std::string button : inputButtons) {
        if (block.fields.at("KEY_OPTION")[0] == button) {
            return BlockResult::CONTINUE;
        }
    }
    return BlockResult::RETURN;
}