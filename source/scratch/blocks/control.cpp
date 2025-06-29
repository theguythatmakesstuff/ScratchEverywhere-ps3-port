#include "control.hpp"

BlockResult ControlBlocks::If(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    if (block.inputs.at("CONDITION")[1].is_null()) {
        return BlockResult::CONTINUE;
    }
    if (executor.runConditionalBlock(block.inputs.at("CONDITION")[1], sprite)) {
        if (!block.inputs.at("SUBSTACK")[1].is_null()) {
            Block* subBlock = findBlock(block.inputs.at("SUBSTACK")[1]);
            executor.runBlock(*subBlock, sprite);
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::ifElse(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    if (block.inputs.at("CONDITION")[1].is_null()) {
        return BlockResult::CONTINUE;
    }
    if (executor.runConditionalBlock(block.inputs.at("CONDITION")[1], sprite)) {
        if (!block.inputs.at("SUBSTACK")[1].is_null()) {
            Block* subBlock = findBlock(block.inputs.at("SUBSTACK")[1]);
            executor.runBlock(*subBlock, sprite);
        }
    } else {
        if (!block.inputs.at("SUBSTACK2")[1].is_null()) {
            Block* subBlock = findBlock(block.inputs.at("SUBSTACK2")[1]);
            executor.runBlock(*subBlock, sprite);
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::createCloneOf(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::cout << "Trying " << std::endl;
    Block* cloneOptions = findBlock(block.inputs.at("CLONE_OPTION")[1]);
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
    if (spriteToClone != nullptr && !spriteToClone->name.empty()) {
        spriteToClone->isClone = true;
        spriteToClone->isStage = false;
        spriteToClone->toDelete = false;
        spriteToClone->id = generateRandomString(15);
        std::cout << "Created clone of " << sprite->name << std::endl;
        std::unordered_map<std::string, Block> newBlocks;
        for (auto& [id, block] : spriteToClone->blocks) {
            if (block.opcode == block.CONTROL_START_AS_CLONE || block.opcode == block.EVENT_WHENBROADCASTRECEIVED || block.opcode == block.PROCEDURES_DEFINITION || block.opcode == block.PROCEDURES_PROTOTYPE) {
                std::vector<Block*> blockChain = getBlockChain(block.id);
                for (const Block* block : blockChain) {
                    newBlocks[block->id] = *block;
                }
            }
        }
        spriteToClone->blocks.clear();
        spriteToClone->blocks = newBlocks;


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
BlockResult ControlBlocks::deleteThisClone(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    if(sprite->isClone)
    sprite->toDelete = true;
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::stop(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string stopType = block.fields.at("STOP_OPTION")[0];
    if(stopType == "all"){
        toExit = true;
        return BlockResult::RETURN;
    }
    if(stopType == "this script"){
        for (std::string repeatID : sprite->blockChains[block.blockChainID].blocksToRepeat) {
            Block* repeatBlock = findBlock(repeatID);
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
                Block* repeatBlock = findBlock(repeatID);
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

BlockResult ControlBlocks::startAsClone(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::wait(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Block* blockReference = findBlock(block.id);
    
    if(blockReference->repeatTimes == -1){
        blockReference->repeatTimes = -5;
        
        std::string duration = Scratch::getInputValue(block.inputs.at("DURATION"), &block, sprite);
        if(isNumber(duration)) {
            blockReference->waitDuration = std::stod(duration) * 1000; // convert to milliseconds
        } else {
            blockReference->waitDuration = 0;
        }
        
        blockReference->waitStartTime = std::chrono::high_resolution_clock::now();
        
        BlockExecutor::addToRepeatQueue(sprite, const_cast<Block*>(&block));
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - blockReference->waitStartTime).count();
    
    if (elapsedTime >= blockReference->waitDuration) {
        blockReference->repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }

    return BlockResult::RETURN;
}

BlockResult ControlBlocks::waitUntil(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Block* blockReference = findBlock(block.id);
    
    if(blockReference->repeatTimes == -1){
        blockReference->repeatTimes = -4;
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
        blockReference->repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    return BlockResult::RETURN;
}

BlockResult ControlBlocks::repeat(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Block* blockReference = findBlock(block.id);

    if(blockReference->repeatTimes == -1){
        std::string times = Scratch::getInputValue(block.inputs.at("TIMES"), &block, sprite);
        blockReference->repeatTimes = std::stoi(times);
        BlockExecutor::addToRepeatQueue(sprite,const_cast<Block*>(&block));
        //std::cout << "set! " << blockReference->repeatTimes << std::endl;
    }

    if (blockReference->repeatTimes > 0) {
        auto substackIt = block.inputs.find("SUBSTACK");
        if (substackIt != block.inputs.end()) {
            const auto& substack = substackIt->second;
            if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
                Block* subBlock = findBlock(substack[1]);
                if (subBlock != nullptr) {
                    //std::cout << "running inside repeat! " << blockReference->repeatTimes << std::endl;
                    executor.runBlock(*subBlock, sprite,const_cast<Block*>(&block));
                }
        }
    }
        // Countdown
        blockReference->repeatTimes -= 1;
        return BlockResult::RETURN;
} else {
        blockReference->repeatTimes = -1;
    }

    sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
    return BlockResult::CONTINUE;

}

BlockResult ControlBlocks::repeatUntil(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Block* blockReference = findBlock(block.id);

    if(blockReference->repeatTimes == -1){
        blockReference->repeatTimes = -2;
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
        blockReference->repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    auto substackIt = block.inputs.find("SUBSTACK");
    if (substackIt != block.inputs.end()) {
        const auto& substack = substackIt->second;
        if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
            Block* subBlock = findBlock(substack[1]);
            if (subBlock != nullptr) {
                executor.runBlock(*subBlock, sprite, const_cast<Block*>(&block));
            }
        }
    }
    
    // Continue the loop
    return BlockResult::RETURN;
}

BlockResult ControlBlocks::forever(const Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Block* blockReference = findBlock(block.id);

    if(blockReference->repeatTimes == -1){
        blockReference->repeatTimes = -3;
        BlockExecutor::addToRepeatQueue(sprite, const_cast<Block*>(&block));
    }

    auto substackIt = block.inputs.find("SUBSTACK");
    if (substackIt != block.inputs.end()) {
        const auto& substack = substackIt->second;
        if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
            Block* subBlock = findBlock(substack[1]);
            if (subBlock != nullptr) {
                executor.runBlock(*subBlock, sprite, const_cast<Block*>(&block));
            }
        }
    }
    return BlockResult::RETURN;
}