#include "control.hpp"

BlockResult ControlBlocks::If(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
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

BlockResult ControlBlocks::ifElse(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
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

BlockResult ControlBlocks::createCloneOf(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
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
        spriteToClone->conditionals.clear();
        // for (auto& [id, conditional] : spriteToClone->conditionals) {
        //     conditional.hostSprite = spriteToClone;
        //     conditional.isTrue = false;
        // }
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
BlockResult ControlBlocks::deleteThisClone(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
    if(sprite->isClone)
    sprite->toDelete = true;
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::stop(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
    std::string stopType = block.fields.at("STOP_OPTION")[0];
    if(stopType == "all"){
        toExit = true;
        return BlockResult::BREAK;
    }
    if(stopType == "this script"){
        Block* parent = getBlockParent(&block);
        //std::cout << "Stopping script " << parent.id << std::endl;
        for(auto& [id,block] : sprite->blocks){
            if(block.topLevelParentBlock == parent->id){
                if(sprite->conditionals.find(id) != sprite->conditionals.end()){
                    sprite->conditionals.erase(id);
                }
            }
        }
        return BlockResult::CONTINUE;
    }

    if(stopType == "other scripts in sprite"){
        std::string topLevelParentBlock = getBlockParent(&block)->id;
        //std::cout << "Stopping other scripts in sprite " << sprite->id << std::endl;
        for(auto& [id,block] : sprite->blocks){
            if(block.topLevelParentBlock != topLevelParentBlock){
                if(sprite->conditionals.find(id) != sprite->conditionals.end()){
                    sprite->conditionals.erase(id);
                }
            }
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::startAsClone(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::wait(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
//     if (sprite->conditionals.find(block.id) == sprite->conditionals.end()) {
//         Conditional newConditional;
//         newConditional.id = block.id;
//         newConditional.hostSprite = sprite;
//         newConditional.isTrue = false;
//         newConditional.times = -1;
//         newConditional.time = std::chrono::high_resolution_clock::now();
//         std::string duration = Scratch::getInputValue(block.inputs.at("DURATION"), &block, sprite);
//         if(isNumber(duration)) {
//             newConditional.endTime = std::stod(duration) * 1000; // convert to milliseconds
//         } else {
//             newConditional.endTime = 0;
//         }
//         newConditional.waitingBlock = *waitingBlock;
//         newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
//         newConditional.waitingConditional = getParentConditional(sprite,block.id);
//         //std::cout << "block = " << block.id << ". cond = " << newConditional.waitingConditional->id << std::endl;
//         if(newConditional.waitingBlock != nullptr){
//             sprite->conditionals[newConditional.waitingBlock->id].isActive = false;
//             newConditional.waitingBlockId = newConditional.waitingBlock->id;
//         }

//         sprite->conditionals[newConditional.id] = newConditional;
// }

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - sprite->conditionals[block.id].time).count();
    if (elapsedTime < sprite->conditionals[block.id].endTime) {
        sprite->conditionals[block.id].isTrue = true;
        
        return BlockResult::RETURN;
    } else {
        std::cout << "Dione! " << block.id << std::endl;
        sprite->conditionals[block.id].isTrue = false;
        *waitingBlock = sprite->conditionals[block.id].waitingBlock;
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::waitUntil(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
    // if (sprite->conditionals.find(block.id) == sprite->conditionals.end()) {
    //     Conditional newConditional;
    //     newConditional.id = block.id;
    //     newConditional.hostSprite = sprite;
    //     newConditional.isTrue = false;
    //     newConditional.times = -1;
    //     newConditional.waitingBlock = *waitingBlock;
    //     newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
    //     newConditional.waitingConditional = getParentConditional(sprite,block.id);
    //     if(newConditional.waitingConditional != nullptr) newConditional.waitingConditional->isActive = false;

    //     if(newConditional.waitingBlock != nullptr){
    //         sprite->conditionals[newConditional.waitingBlock->id].isActive = false;
    //         newConditional.waitingBlockId = newConditional.waitingBlock->id;
    //     }
    //     sprite->conditionals[newConditional.id] = newConditional;
    // }
    if (block.inputs.at("CONDITION")[1].is_null() || !executor.runConditionalBlock(block.inputs.at("CONDITION")[1], sprite)) {
        sprite->conditionals[block.id].isTrue = true;
        return BlockResult::RETURN;
    } else {
        sprite->conditionals[block.id].isTrue = false;
        *waitingBlock = sprite->conditionals[block.id].waitingBlock;
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::repeat(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
    //std::cout << "repeat " << block.id << std::endl;
    // if (sprite->conditionals.find(block.id) == sprite->conditionals.end()) {
    //     Conditional newConditional;
    //     newConditional.id = block.id;
    //     newConditional.hostSprite = sprite;
    //     newConditional.isTrue = false;
    //     std::string times = Scratch::getInputValue(block.inputs.at("TIMES"), &block, sprite);
    //     newConditional.times = std::stoi(times);
    //     newConditional.waitingBlock = *waitingBlock;
    //     newConditional.waitingConditional = getParentConditional(sprite,block.id);
    //     newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
    //     if(newConditional.waitingBlock != nullptr){
    //         sprite->conditionals[newConditional.waitingBlock->id].isActive = false;
    //         newConditional.waitingBlockId = newConditional.waitingBlock->id;
    //         //waitingBlock = nullptr;
    //     }

    //     sprite->conditionals[newConditional.id] = newConditional;
    // }

    Block* blockReference = findBlock(block.id);

    if(blockReference->repeatTimes == -1){
        std::string times = Scratch::getInputValue(block.inputs.at("TIMES"), &block, sprite);
        blockReference->repeatTimes = std::stoi(times);
        BlockExecutor::addToRepeatQueue(sprite,const_cast<Block*>(&block));
        std::cout << "set! " << blockReference->repeatTimes << std::endl;
    }

    if (blockReference->repeatTimes > 0) {
        auto substackIt = block.inputs.find("SUBSTACK");
        if (substackIt != block.inputs.end()) {
            const auto& substack = substackIt->second;
            if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
                Block* subBlock = findBlock(substack[1]);
                if (subBlock != nullptr) {
                    std::cout << "running inside repeat! " << blockReference->repeatTimes << std::endl;
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

BlockResult ControlBlocks::repeatUntil(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
    // if (sprite->conditionals.find(block.id) == sprite->conditionals.end()) {
    //     Conditional newConditional;
    //     newConditional.id = block.id;
    //     newConditional.hostSprite = sprite;
    //     newConditional.isTrue = false;
    //     newConditional.times = -1;
    //     newConditional.waitingBlock = *waitingBlock;
    //     newConditional.waitingConditional = getParentConditional(sprite,block.id);
    //     newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
    //     if(newConditional.waitingBlock != nullptr){
    //         sprite->conditionals[newConditional.waitingBlock->id].isActive = false;
    //         newConditional.waitingBlockId = newConditional.waitingBlock->id;
    //     }
    //     sprite->conditionals[newConditional.id] = newConditional;
    // }

    auto it = block.inputs.find("CONDITION");
    if (it != block.inputs.end()) {
        const auto& condition = it->second;
        if (condition.is_array() && condition.size() > 1 && condition[1].is_null()) {
            if(!executor.runConditionalBlock(block.inputs.at("CONDITION")[1], sprite)){
                sprite->conditionals[block.id].isTrue = true;
            } else {
                sprite->conditionals[block.id].isTrue = false;
                *waitingBlock = sprite->conditionals[block.id].waitingBlock;
    }
            
        }
    }
    else sprite->conditionals[block.id].isTrue = true;

    // run repeat block if theres any block inside of it
    if (sprite->conditionals[block.id].isTrue) {
        auto substackIt = block.inputs.find("SUBSTACK");
        if (substackIt != block.inputs.end()) {
            const auto& substack = substackIt->second;
            if (substack.is_array() && substack.size() > 1 && !substack[1].is_null()) {
                Block* subBlock = findBlock(substack[1]);
                if (subBlock != nullptr) {
                    executor.runBlock(*subBlock, sprite,const_cast<Block*>(&block));
                }
            }
        }
        return BlockResult::RETURN;
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::forever(const Block& block, Sprite* sprite, Block** waitingBlock, bool withoutScreenRefresh){
    // if (sprite->conditionals.find(block.id) == sprite->conditionals.end()) {
    //     Conditional newConditional;
    //     newConditional.id = block.id;
    //     newConditional.hostSprite = sprite;
    //     newConditional.isTrue = false;
    //     newConditional.times = -1;
    //     newConditional.waitingBlock = *waitingBlock;
    //     newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
    //     newConditional.waitingConditional = getParentConditional(sprite,block.id);
    //     if(newConditional.waitingConditional != nullptr) newConditional.waitingConditional->isActive = false;
    //     sprite->conditionals[newConditional.id] = newConditional;
    // }

    if (sprite->conditionals[block.id].isTrue) {
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
    }
    sprite->conditionals[block.id].isTrue = true;
    return BlockResult::CONTINUE;
}