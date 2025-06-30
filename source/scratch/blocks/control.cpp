#include "control.hpp"

BlockResult ControlBlocks::If(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    if (block.inputs.at("CONDITION")[1].is_null()) {
        return BlockResult::CONTINUE;
    }
    if (executor.runConditionalBlock(block.inputs.at("CONDITION")[1], sprite)) {
        if (!block.inputs.at("SUBSTACK")[1].is_null()) {
            Block* subBlock = &sprite->blocks[block.inputs.at("SUBSTACK")[1]];
            executor.runBlock(*subBlock, sprite);
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::ifElse(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    if (block.inputs.at("CONDITION")[1].is_null()) {
        return BlockResult::CONTINUE;
    }
    if (executor.runConditionalBlock(block.inputs.at("CONDITION")[1], sprite)) {
        if (!block.inputs.at("SUBSTACK")[1].is_null()) {
            Block* subBlock = &sprite->blocks[block.inputs.at("SUBSTACK")[1]];
            executor.runBlock(*subBlock, sprite);
        }
    } else {
        if (!block.inputs.at("SUBSTACK2")[1].is_null()) {
            Block* subBlock = &sprite->blocks[block.inputs.at("SUBSTACK2")[1]];
            executor.runBlock(*subBlock, sprite);
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::createCloneOf(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::cout << "Trying " << std::endl;
    Block* cloneOptions = &sprite->blocks[block.inputs.at("CLONE_OPTION")[1]];
    Sprite* spriteToClone = getAvailableSprite();
    if(!spriteToClone) return BlockResult::CONTINUE;
    if (cloneOptions->fields["CLONE_OPTION"][0] == "_myself_") {
        *spriteToClone = *sprite;
    } else {
        for (Sprite* currentSprite : sprites) {
            if (currentSprite->name == removeQuotations(cloneOptions->fields["CLONE_OPTION"][0]) && !currentSprite->isClone) {
                *spriteToClone = *currentSprite;
            }
        }
    }
    spriteToClone->blockChains.clear();

    if (spriteToClone != nullptr && !spriteToClone->name.empty()) {
        spriteToClone->isClone = true;
        spriteToClone->isStage = false;
        spriteToClone->toDelete = false;
        spriteToClone->id = generateRandomString(15);
        std::cout << "Created clone of " << sprite->name << std::endl;
        // std::unordered_map<std::string, Block> newBlocks;
        // for (auto& [id, block] : spriteToClone->blocks) {
        //     if (block.opcode == block.CONTROL_START_AS_CLONE || block.opcode == block.EVENT_WHENBROADCASTRECEIVED || block.opcode == block.PROCEDURES_DEFINITION || block.opcode == block.PROCEDURES_PROTOTYPE) {
        //         std::vector<Block*> blockChain = getBlockChain(block.id);
        //         for (const Block* block : blockChain) {
        //             newBlocks[block->id] = *block;
        //         }
        //     }
        // }
        // spriteToClone->blocks.clear();
        // spriteToClone->blocks = newBlocks;

        // add clone to sprite list
        sprites.push_back(spriteToClone);
        Sprite* addedSprite = sprites.back();
        // Run "when I start as a clone" scripts for the clone
        for (Sprite* currentSprite : sprites) {
            if (currentSprite == addedSprite) {
                for (auto& [id, block] : currentSprite->blocks) {
                    if (block.opcode == block.CONTROL_START_AS_CLONE) {
                        // std::cout << "Running clone block " << block.id << std::endl;
                        executor.runBlock(block, currentSprite);
                    }
                }
            }
        }
    }
    return BlockResult::CONTINUE;
}
BlockResult ControlBlocks::deleteThisClone(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    if(sprite->isClone)
    sprite->toDelete = true;
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::stop(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string stopType = block.fields.at("STOP_OPTION")[0];
    if(stopType == "all"){
        toExit = true;
        return BlockResult::RETURN;
    }
    if(stopType == "this script"){
        for (std::string repeatID : sprite->blockChains[block.blockChainID].blocksToRepeat) {
            Block* repeatBlock = &sprite->blocks[repeatID];
            if (repeatBlock) {
                repeatBlock->repeatTimes = -1;
            }
        }
        sprite->blockChains[block.blockChainID].blocksToRepeat.clear();
        return BlockResult::CONTINUE;
    }

    if(stopType == "other scripts in sprite"){
        for(auto& [id,chain] : sprite->blockChains){
            if(id == block.blockChainID) continue;
            for (std::string repeatID : chain.blocksToRepeat) {
                Block* repeatBlock = &sprite->blocks[repeatID];
                if (repeatBlock) {
                    repeatBlock->repeatTimes = -1;
                }
            }
            chain.blocksToRepeat.clear();
        }
        return BlockResult::CONTINUE;
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::startAsClone(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::wait(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    
    if(block.repeatTimes == -1){
        block.repeatTimes = -5;
        
        std::string duration = Scratch::getInputValue(block.inputs.at("DURATION"), &block, sprite);
        if(Math::isNumber(duration)) {
            block.waitDuration = std::stod(duration) * 1000; // convert to milliseconds
        } else {
            block.waitDuration = 0;
        }
        
        block.waitStartTime = std::chrono::high_resolution_clock::now();
        
        BlockExecutor::addToRepeatQueue(sprite, const_cast<Block*>(&block));
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - block.waitStartTime).count();
    
    if (elapsedTime >= block.waitDuration) {
        block.repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }

    return BlockResult::RETURN;
}

BlockResult ControlBlocks::waitUntil(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    
    if(block.repeatTimes == -1){
        block.repeatTimes = -4;
        BlockExecutor::addToRepeatQueue(sprite, const_cast<Block*>(&block));
    }
    
    bool conditionMet = false;
    auto conditionIt = block.inputs.find("CONDITION");
    if (conditionIt != block.inputs.end()) {
        const auto& condition = conditionIt->second;
        if (condition.is_array() && condition.size() > 1 && !condition[1].is_null()) {
            conditionMet = executor.runConditionalBlock(condition[1], sprite);
        }
    }
    
    if (conditionMet) {
        block.repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    return BlockResult::RETURN;
}

BlockResult ControlBlocks::repeat(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){

    if(block.repeatTimes == -1){
        std::string times = Scratch::getInputValue(block.inputs.at("TIMES"), &block, sprite);
        block.repeatTimes = std::stoi(times);
        BlockExecutor::addToRepeatQueue(sprite,const_cast<Block*>(&block));
        //std::cout << "set! " << blockReference->repeatTimes << std::endl;
    }

    if (block.repeatTimes > 0) {
        auto substackIt = block.inputs.find("SUBSTACK");
        if (substackIt != block.inputs.end()) {
            const auto& substack = substackIt->second;
            if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
                Block* subBlock = &sprite->blocks[substack[1]];
                if (subBlock != nullptr) {
                    //std::cout << "running inside repeat! " << blockReference->repeatTimes << std::endl;
                    executor.runBlock(*subBlock, sprite,const_cast<Block*>(&block));
                }
        }
    }
        // Countdown
        block.repeatTimes -= 1;
        return BlockResult::RETURN;
} else {
        block.repeatTimes = -1;
    }

    sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
    return BlockResult::CONTINUE;

}

BlockResult ControlBlocks::repeatUntil(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){

    if(block.repeatTimes == -1){
        block.repeatTimes = -2;
        BlockExecutor::addToRepeatQueue(sprite, &block);
        std::cout << "added to repeat queue!" << std::endl;
    }
    

    bool conditionMet = false;
    auto conditionIt = block.inputs.find("CONDITION");
    if (conditionIt != block.inputs.end()) {
        const auto& condition = conditionIt->second;
        if (condition.is_array() && condition.size() > 1 && !condition[1].is_null()) {
            
            conditionMet = executor.runConditionalBlock(condition[1], sprite);

        }
    }
    
    if (conditionMet) {
        block.repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    auto substackIt = block.inputs.find("SUBSTACK");
    if (substackIt != block.inputs.end()) {
        const auto& substack = substackIt->second;
        if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
            Block* subBlock = &sprite->blocks[substack[1]];
            if (subBlock != nullptr) {
                executor.runBlock(*subBlock, sprite, &block);
            }
        }
    }
    
    // Continue the loop
    return BlockResult::RETURN;
}

BlockResult ControlBlocks::forever(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){

    if(block.repeatTimes == -1){
        block.repeatTimes = -3;
        BlockExecutor::addToRepeatQueue(sprite, &block);
    }

    auto substackIt = block.inputs.find("SUBSTACK");
    if (substackIt != block.inputs.end()) {
        const auto& substack = substackIt->second;
        if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
            Block* subBlock = &sprite->blocks[substack[1]];
            if (subBlock != nullptr) {
                executor.runBlock(*subBlock, sprite, &block);
            }
        }
    }
    return BlockResult::RETURN;
}