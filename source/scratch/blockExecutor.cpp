#include "blockExecutor.hpp"
#include "blocks/motion.hpp"
#include "blocks/events.hpp"

BlockExecutor::BlockExecutor(){
    registerHandlers();
}

void BlockExecutor::registerHandlers(){

    // motion
    handlers[Block::MOTION_MOVE_STEPS] = MotionBlocks::moveSteps;

    // events
    handlers[Block::EVENT_WHENFLAGCLICKED] = EventBlocks::flagClicked;

}

void BlockExecutor::runBlock(Block block, Sprite* sprite, Block waitingBlock, bool withoutScreenRefresh){
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!sprite || sprite->toDelete) {
        return;
    }
    
    while (block.id != "null") {

        BlockResult result = executeBlock(block, sprite, waitingBlock, withoutScreenRefresh);
        
        if (result == BlockResult::RETURN) {
            return;
        } else if (result == BlockResult::BREAK) {
            break;
        }
        
        // Timing measurement
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        if (duration.count() > 0) {
            // std::cout << block.opcode << " took " << duration.count() << " milliseconds!" << std::endl;
        }
        
        // Move to next block
        if (!block.next.empty()) {
            block = *blockLookup[block.next];
        } else {
            runBroadcasts();
            if (!waitingBlock.id.empty() && blockLookup.find(waitingBlock.id) != blockLookup.end()) {
                block = *blockLookup[waitingBlock.id];
                std::cout << "block is now " << block.id << " from waiting." << std::endl;
                withoutScreenRefresh = false;
                waitingBlock = Block(); // reset waiting block
            } else {
                break;
            }
        }
    }
}


BlockResult BlockExecutor::executeBlock(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    auto iterator = handlers.find(block.opcode);
    if (iterator != handlers.end()) {
        return iterator->second(block, sprite, waitingBlock, withoutScreenRefresh);
    }

    return BlockResult::CONTINUE;
}