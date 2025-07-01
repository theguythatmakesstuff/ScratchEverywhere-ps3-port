#include "procedure.hpp"

Value ProcedureBlocks::stringNumber(Block& block, Sprite* sprite) {
    return findCustomValue(block.fields.at("VALUE")[0], sprite, block);
}

Value ProcedureBlocks::booleanArgument(Block& block, Sprite* sprite){
    Value value = findCustomValue(block.fields.at("VALUE")[0], sprite, block);
    return Value(value.asInt() == 1);
}

BlockResult ProcedureBlocks::call(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    
    // Initialize the custom block call if not already set up
    if(block.repeatTimes == -1){
        // Set up the custom block call (using -8 to distinguish from other blocks)
        block.repeatTimes = -8;
        block.customBlockExecuted = false;
        
        //std::cout << "doing it " << block.id << std::endl;
        
        // Run the custom block for the first time
        runCustomBlock(sprite, block, &block,withoutScreenRefresh);
        block.customBlockExecuted = true;
        
        BlockExecutor::addToRepeatQueue(sprite, &block);
    }
    
    // Check if any repeat blocks are still running inside the custom block
    if(block.customBlockPtr != nullptr && 
       !BlockExecutor::hasActiveRepeats(sprite, block.customBlockPtr->blockChainID)){
        
        //std::cout << "done with custom!" << std::endl;
        
        // Custom block execution is complete
        block.repeatTimes = -1; // Reset for next use
        block.customBlockExecuted = false;
        block.customBlockPtr = nullptr;
        
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    return BlockResult::RETURN;
}

BlockResult ProcedureBlocks::definition(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    return BlockResult::CONTINUE;
}