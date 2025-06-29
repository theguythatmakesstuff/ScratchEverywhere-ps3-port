#include "procedure.hpp"

std::string ProcedureBlocks::stringNumber(const Block& block, Sprite* sprite) {
    return findCustomValue(block.fields.at("VALUE")[0], sprite, block);
}

bool ProcedureBlocks::booleanArgument(const Block& block, Sprite* sprite){
    std::string value = findCustomValue(block.fields.at("VALUE")[0], sprite, block);
    return value == "1";
}

BlockResult ProcedureBlocks::call(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh) {
    // if (sprite->conditionals.find(block.id) == sprite->conditionals.end()) {
    //     Conditional newConditional;
    //     newConditional.id = block.id;
    //     newConditional.hostSprite = sprite;
    //     newConditional.isTrue = false;
    //     newConditional.times = -1;
    //     newConditional.waitingConditional = getParentConditional(sprite,block.id);
    //     if(newConditional.waitingConditional != nullptr) newConditional.waitingConditional->isActive = false;
    //     // Block* nextBlockptr = findBlock(block.next);
    //     // if(nextBlockptr != nullptr) newConditional.waitingBlock = *nextBlockptr;
    //     //newConditional.waitingBlock = block;
    //     sprite->conditionals[block.id] = newConditional;
    // }
    std::cout << "doing it " << block.id << std::endl;
    //waitingBlock = findBlock(block.next);
    


    if(!sprite->conditionals[block.id].isTrue){
    std::cout << "about to run " << block.id << std::endl;
    runCustomBlock(sprite, block);
    sprite->conditionals[block.id].isTrue = true;
    sprite->conditionals[block.id].isActive = true;
    if(sprite->conditionals[block.id].waitingBlock != nullptr && 
        !sprite->conditionals[block.id].waitingBlock->id.empty()) return BlockResult::RETURN;
    }


    // check if any repeat blocks are running in the custom block
    if(!hasActiveConditionalsInside(sprite,sprite->conditionals[block.id].customBlock->id)){
    std::cout << "done with custom!" << std::endl;
        sprite->conditionals[block.id].isTrue = false;
        //sprite->conditionals.erase(block.id);
        return BlockResult::CONTINUE;
    }
    else return BlockResult::RETURN;

}

BlockResult ProcedureBlocks::definition(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh) {
    return BlockResult::CONTINUE;
}