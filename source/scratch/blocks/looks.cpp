#include "looks.hpp"

BlockResult LooksBlocks::show(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    sprite->visible = true;
    return BlockResult::CONTINUE;
}
BlockResult LooksBlocks::hide(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    sprite->visible = false;
    return BlockResult::CONTINUE;
}

BlockResult LooksBlocks::switchCostumeTo(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string inputValue;
    try{
    inputValue = executor.getBlockValue(*findBlock(block->inputs.at("COSTUME")[1]),sprite);}
    catch(...){
        inputValue = Scratch::getInputValue(block->inputs.at("COSTUME"),block,sprite);
    }
    //std::cout << "costume = " << inputValue << std::endl;

    if (isNumber(inputValue)){
        int costumeIndex = std::stoi(inputValue) - 1;
        if (costumeIndex >= 0 && static_cast<size_t>(costumeIndex) < sprite->costumes.size()) {
            if(sprite->currentCostume != costumeIndex){
                freeImage(sprite,sprite->costumes[sprite->currentCostume].id);
            }
            sprite->currentCostume = costumeIndex;

        }
    } else {
        for (size_t i = 0; i < sprite->costumes.size(); i++) {
            if (sprite->costumes[i].name == inputValue) {
                if((size_t)sprite->currentCostume != i){
                    freeImage(sprite,sprite->costumes[sprite->currentCostume].id);
                }
                sprite->currentCostume = i;
                break;
            }
        }
    }

        if(projectType == UNZIPPED){
            loadImageFromFile(sprite->costumes[sprite->currentCostume].id);
        }

    return BlockResult::CONTINUE;
}

BlockResult LooksBlocks::nextCostume(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    freeImage(sprite,sprite->costumes[sprite->currentCostume].id);
    sprite->currentCostume++;
    if (sprite->currentCostume >= static_cast<int>(sprite->costumes.size())) {
        sprite->currentCostume = 0;
    }
    if(projectType == UNZIPPED){
        loadImageFromFile(sprite->costumes[sprite->currentCostume].id);
    }
    return BlockResult::CONTINUE;
}

BlockResult LooksBlocks::switchBackdropTo(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string inputValue = executor.getBlockValue(*findBlock(block->inputs.at("BACKDROP")[1]),sprite);
    for(Sprite* currentSprite : sprites){
        if(!currentSprite->isStage){
            continue;
        }
    if (isNumber(inputValue)){
        int costumeIndex = std::stoi(inputValue) - 1;
        if (costumeIndex >= 0 && static_cast<size_t>(costumeIndex) < currentSprite->costumes.size()) {
            if(sprite->currentCostume != costumeIndex){
            freeImage(currentSprite,currentSprite->costumes[currentSprite->currentCostume].id);}
            currentSprite->currentCostume = costumeIndex;
            if(projectType == UNZIPPED){
                loadImageFromFile(currentSprite->costumes[currentSprite->currentCostume].id);
                }
        }
    } else {
        for (size_t i = 0; i < currentSprite->costumes.size(); i++) {
            if (currentSprite->costumes[i].name == inputValue) {
                if((size_t)sprite->currentCostume != i){
                    freeImage(currentSprite,currentSprite->costumes[currentSprite->currentCostume].id);
                }
                currentSprite->currentCostume = i;
                if(projectType == UNZIPPED){
                    loadImageFromFile(currentSprite->costumes[currentSprite->currentCostume].id);
                    }
                break;
            }
        }
    }
}
    return BlockResult::CONTINUE;
}

BlockResult LooksBlocks::nextBackdrop(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    for(Sprite* currentSprite : sprites){
        if(!currentSprite->isStage){
            continue;
        }
        freeImage(currentSprite,currentSprite->costumes[currentSprite->currentCostume].id);
        currentSprite->currentCostume++;
        if (currentSprite->currentCostume >= static_cast<int>(currentSprite->costumes.size())) {
            currentSprite->currentCostume = 0;
        }
        if(projectType == UNZIPPED){
            loadImageFromFile(currentSprite->costumes[currentSprite->currentCostume].id);
        }
}
    return BlockResult::CONTINUE;
}

BlockResult LooksBlocks::goForwardBackwardLayers(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block->inputs.at("NUM"), block, sprite);
    std::string forwardBackward = block->fields.at("FORWARD_BACKWARD")[0];
    if (isNumber(value)) {
    if (forwardBackward == "forward") {

        // check if a sprite is already on the same layer
        for(Sprite* currentSprite : sprites){
            if(currentSprite->isStage) continue;
            if(currentSprite->layer == (currentSprite->layer + std::stoi(value))){
                for(Sprite* moveupSprite : sprites){
                    if(moveupSprite->isStage || !(moveupSprite->layer >= (currentSprite->layer + std::stoi(value)))) continue;
                    moveupSprite-> layer++;
                }
            }
        }

        sprite->layer += std::stoi(value);

    } else if (forwardBackward == "backward") {

        // check if a sprite is already on the same layer
        for(Sprite* currentSprite : sprites){
            if(currentSprite->isStage) continue;
            if(currentSprite->layer == (currentSprite->layer - std::stoi(value))){
                for(Sprite* moveupSprite : sprites){
                    if(moveupSprite->isStage || !(moveupSprite->layer >= (currentSprite->layer - std::stoi(value)))) continue;
                    moveupSprite-> layer++;
                }
            }
        }

        sprite->layer -= std::stoi(value);
        if(sprite->layer < 1){
            for(Sprite* currentSprite : sprites){
            if(currentSprite->isStage) continue;
            currentSprite->layer+= 2;
        }
        sprite->layer = 0;
        }
    }
}
    return BlockResult::CONTINUE;
}

BlockResult LooksBlocks::goToFrontBack(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string value = block->fields.at("FRONT_BACK")[0];
    if (value == "front") {
        sprite->layer = getMaxSpriteLayer() + 1;
    } else if (value == "back") {
        for(Sprite* currentSprite : sprites){
            if(currentSprite->isStage) continue;
            currentSprite->layer+= 2;
        }
        sprite->layer = 0;
    }
    return BlockResult::CONTINUE;
}

BlockResult LooksBlocks::setSizeTo(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block->inputs.at("SIZE"), block, sprite);
    if (isNumber(value)) {
        sprite->size = std::stod(value);
    }
    return BlockResult::CONTINUE;
}
BlockResult LooksBlocks::changeSizeBy(Block* block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block->inputs.at("CHANGE"), block, sprite);
    if (isNumber(value)) {
        sprite->size += std::stod(value);
    }
    return BlockResult::CONTINUE;
}

std::string LooksBlocks::size(const Block& block, Sprite* sprite) {
    return std::to_string(sprite->size);
}

std::string LooksBlocks::costume(const Block& block, Sprite* sprite) {
    return block.fields.at("COSTUME")[0];
}

std::string LooksBlocks::backdrops(const Block& block, Sprite* sprite) {
    return block.fields.at("BACKDROP")[0];
}

std::string LooksBlocks::costumeNumberName(const Block& block, Sprite* sprite) {
    std::string value = block.fields.at("NUMBER_NAME")[0];
    if (value == "name") {
        return sprite->costumes[sprite->currentCostume].name;
    } else if (value == "number") {
        return std::to_string(sprite->currentCostume + 1);
    }
    return "";
}

std::string LooksBlocks::backdropNumberName(const Block& block, Sprite* sprite) {
    std::string value = block.fields.at("NUMBER_NAME")[0];
    if (value == "name") {
        for (Sprite* currentSprite : sprites) {
            if (currentSprite->isStage) {
                return currentSprite->costumes[currentSprite->currentCostume].name;
            }
        }
    } else if (value == "number") {
        for (Sprite* currentSprite : sprites) {
            if (currentSprite->isStage) {
                return std::to_string(currentSprite->currentCostume + 1);
            }
        }
    }
    return "";
}