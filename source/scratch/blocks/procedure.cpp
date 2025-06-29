#include "procedure.hpp"

std::string ProcedureBlocks::stringNumber(const Block& block, Sprite* sprite) {
    return findCustomValue(block.fields.at("VALUE")[0], sprite, block);
}

bool ProcedureBlocks::booleanArgument(const Block& block, Sprite* sprite){
    std::string value = findCustomValue(block.fields.at("VALUE")[0], sprite, block);
    return value == "1";
}

BlockResult ProcedureBlocks::call(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    Block* blockReference = findBlock(block.id);
    
    // Initialize the custom block call if not already set up
    if(blockReference->repeatTimes == -1){
        // Set up the custom block call (using -8 to distinguish from other blocks)
        blockReference->repeatTimes = -8;
        blockReference->customBlockExecuted = false;
        
        //std::cout << "doing it " << block.id << std::endl;
        
        // Run the custom block for the first time
        runCustomBlock(sprite, block, blockReference,withoutScreenRefresh);
        blockReference->customBlockExecuted = true;
        
        BlockExecutor::addToRepeatQueue(sprite, const_cast<Block*>(&block));
    }
    
    // Check if any repeat blocks are still running inside the custom block
    if(blockReference->customBlockPtr != nullptr && 
       !BlockExecutor::hasActiveRepeats(sprite, blockReference->customBlockPtr->blockChainID)){
        
        //std::cout << "done with custom!" << std::endl;
        
        // Custom block execution is complete
        blockReference->repeatTimes = -1; // Reset for next use
        blockReference->customBlockExecuted = false;
        blockReference->customBlockPtr = nullptr;
        
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    return BlockResult::RETURN;
}

BlockResult ProcedureBlocks::definition(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    return BlockResult::CONTINUE;
}