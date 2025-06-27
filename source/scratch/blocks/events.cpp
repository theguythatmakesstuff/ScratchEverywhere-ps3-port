#include "events.hpp"

BlockResult EventBlocks::flagClicked(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    return BlockResult::CONTINUE;
}

BlockResult EventBlocks::broadcast(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    broadcastQueue.push_back( Scratch::getInputValue(block.inputs.at("BROADCAST_INPUT"), &block, sprite));
    return BlockResult::CONTINUE;
}