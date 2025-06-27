#include "control.hpp"

BlockResult ControlBlocks::If(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    if (block.inputs.at("CONDITION")[1].is_null()) {
        return BlockResult::CONTINUE;
    }
    if (runConditionalStatement(block.inputs.at("CONDITION")[1], sprite)) {
        if (!block.inputs.at("SUBSTACK")[1].is_null()) {
            Block* subBlock = findBlock(block.inputs.at("SUBSTACK")[1]);
            executor.runBlock(*subBlock, sprite);
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::ifElse(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    if (block.inputs.at("CONDITION")[1].is_null()) {
        return BlockResult::CONTINUE;
    }
    if (runConditionalStatement(block.inputs.at("CONDITION")[1], sprite)) {
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

BlockResult ControlBlocks::createCloneOf(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
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
                        runBlock(block, currentSprite);
                    }
                }
            }
        }
    }
    return BlockResult::CONTINUE;
}
BlockResult ControlBlocks::deleteThisClone(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    if(sprite->isClone)
    sprite->toDelete = true;
    return BlockResult::CONTINUE;
}

BlockResult ControlBlocks::stop(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
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
        return;
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