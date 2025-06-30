#include "motion.hpp"

BlockResult MotionBlocks::moveSteps(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Value value = Scratch::getInputValue(block,"STEPS",sprite);
            if (value.isNumeric()) {
                double angle = (sprite->rotation - 90) * M_PI / 180.0;
                sprite->xPosition += std::cos(angle) * value.asDouble();
                sprite->yPosition -= std::sin(angle) * value.asDouble();
            } else {
               // std::cerr << "Invalid Move steps " << value << std::endl;
            }
            
    return BlockResult::CONTINUE;

}

BlockResult MotionBlocks::goTo(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    auto inputValue = block.parsedInputs.find("TO");
    Block* inputBlock = findBlock(inputValue->second.blockId);
            std::string objectName = inputBlock->fields["TO"][0];

            if (objectName == "_random_") {
                sprite->xPosition = rand() % projectWidth - projectWidth / 2;
                sprite->yPosition = rand() % projectHeight - projectHeight / 2;
                return BlockResult::CONTINUE;
            }

            if (objectName == "_mouse_") {
                sprite->xPosition = mousePointer.x;
                sprite->yPosition = mousePointer.y;
                return BlockResult::CONTINUE;
            }

            for (Sprite* currentSprite : sprites) {
                if (currentSprite->name == objectName) {
                    sprite->xPosition = currentSprite->xPosition;
                    sprite->yPosition = currentSprite->yPosition;
                    break;
                }
            }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::goToXY(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Value xVal = Scratch::getInputValue(block,"X",sprite);
    Value yVal = Scratch::getInputValue(block,"Y",sprite);
    if (xVal.isNumeric()) sprite->xPosition = xVal.asDouble();
    if (yVal.isNumeric()) sprite->yPosition = yVal.asDouble();
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::turnLeft(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Value value = Scratch::getInputValue(block,"DEGREES",sprite);
    if (value.isNumeric()) {
        sprite->rotation -= value.asInt();
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::turnRight(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Value value = Scratch::getInputValue(block,"DEGREES",sprite);
    if (value.isNumeric()) {
        sprite->rotation += value.asInt();
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::pointInDirection(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Value value = Scratch::getInputValue(block,"DIRECTION", sprite);
    if (value.isNumeric()) {
        sprite->rotation = value.asInt();
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::changeXBy(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    Value value = Scratch::getInputValue(block,"DX", sprite);
    if (value.isNumeric()) {
        sprite->xPosition += value.asDouble();
    } else {
        std::cerr << "Invalid X position " << value.asDouble() << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::changeYBy(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    Value value = Scratch::getInputValue(block,"DY", sprite);
    if (value.isNumeric()) {
        sprite->yPosition += value.asDouble();
    } else {
        std::cerr << "Invalid Y position " << value.asDouble() << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::setX(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    Value value = Scratch::getInputValue(block,"X", sprite);
    if (value.isNumeric()) {
        sprite->xPosition = value.asDouble();
    } else {
        // std::cerr << "Invalid X position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::setY(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    Value value = Scratch::getInputValue(block,"Y", sprite);
    if (value.isNumeric()) {
        sprite->yPosition = value.asDouble();
    } else {
        // std::cerr << "Invalid Y position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::glideSecsToXY(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    
    if(block.repeatTimes == -1){
        block.repeatTimes = -6;
        
        Value duration = Scratch::getInputValue(block,"SECS",sprite);
        if(duration.isNumeric()) {
            block.waitDuration = duration.asDouble() * 1000;
        } else {
            block.waitDuration = 0;
        }
        
        block.waitStartTime = std::chrono::high_resolution_clock::now();
        block.glideStartX = sprite->xPosition;
        block.glideStartY = sprite->yPosition;
        
        // Get target positions
        Value positionXStr = Scratch::getInputValue(block,"X",sprite);
        Value positionYStr = Scratch::getInputValue(block,"Y",  sprite);
        block.glideEndX = positionXStr.isNumeric() ? positionXStr.asDouble() : block.glideStartX;
        block.glideEndY = positionYStr.isNumeric() ? positionYStr.asDouble() : block.glideStartY;
        
        BlockExecutor::addToRepeatQueue(sprite, const_cast<Block*>(&block));
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - block.waitStartTime).count();
    
    if (elapsedTime >= block.waitDuration) {
        sprite->xPosition = block.glideEndX;
        sprite->yPosition = block.glideEndY;
        
        block.repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    double progress = static_cast<double>(elapsedTime) / block.waitDuration;
    if (progress > 1.0) progress = 1.0;
    
    sprite->xPosition = block.glideStartX + (block.glideEndX - block.glideStartX) * progress;
    sprite->yPosition = block.glideStartY + (block.glideEndY - block.glideStartY) * progress;
    
    return BlockResult::RETURN;
}

BlockResult MotionBlocks::glideTo(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){

    if(block.repeatTimes == -1){
        block.repeatTimes = -7;
        
        Value duration = Scratch::getInputValue(block,"SECS", sprite);
        if(duration.isNumeric()) {
            block.waitDuration = duration.asDouble() * 1000;
        } else {
            block.waitDuration = 0;
        }
        
        block.waitStartTime = std::chrono::high_resolution_clock::now();
        block.glideStartX = sprite->xPosition;
        block.glideStartY = sprite->yPosition;
        
        Block* inputBlock;
        auto itVal = block.parsedInputs.find("TO");
        inputBlock = findBlock(itVal->second.blockId);
        if(!inputBlock) return BlockResult::CONTINUE;
        
        std::string inputValue = inputBlock->fields["TO"][0];
        std::string positionXStr;
        std::string positionYStr;
        
        if(inputValue == "_random_"){
            positionXStr = std::to_string(rand() % projectWidth - projectWidth / 2);
            positionYStr = std::to_string(rand() % projectHeight - projectHeight / 2);
        }
        else if(inputValue == "_mouse_"){
            positionXStr = std::to_string(mousePointer.x);
            positionYStr = std::to_string(mousePointer.y);
        }
        else{
            for(auto & currentSprite : sprites){
                if(currentSprite->name == inputValue){
                    positionXStr = std::to_string(currentSprite->xPosition);
                    positionYStr = std::to_string(currentSprite->yPosition);
                    break;
                }
            }
        }
        
        block.glideEndX = Math::isNumber(positionXStr) ? std::stod(positionXStr) : block.glideStartX;
        block.glideEndY = Math::isNumber(positionYStr) ? std::stod(positionYStr) : block.glideStartY;
        
        BlockExecutor::addToRepeatQueue(sprite, const_cast<Block*>(&block));
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - block.waitStartTime).count();
    
    if (elapsedTime >= block.waitDuration) {
        sprite->xPosition = block.glideEndX;
        sprite->yPosition = block.glideEndY;
        
        block.repeatTimes = -1;
        sprite->blockChains[block.blockChainID].blocksToRepeat.pop_back();
        return BlockResult::CONTINUE;
    }
    
    double progress = static_cast<double>(elapsedTime) / block.waitDuration;
    if (progress > 1.0) progress = 1.0;
    
    sprite->xPosition = block.glideStartX + (block.glideEndX - block.glideStartX) * progress;
    sprite->yPosition = block.glideStartY + (block.glideEndY - block.glideStartY) * progress;
    
    return BlockResult::RETURN;
}

BlockResult MotionBlocks::pointToward(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    auto itVal = block.parsedInputs.find("TOWARDS");
    Block* inputBlock = findBlock(itVal->second.blockId);
    if (inputBlock->fields.find("TOWARDS") == inputBlock->fields.end()) {
        // std::cerr << "Error: Unable to find object for POINT_TOWARD block." << std::endl;
        return BlockResult::CONTINUE;
    }
    
    std::string objectName = inputBlock->fields["TOWARDS"][0];
    double targetX = 0;
    double targetY = 0;
    
    if (objectName == "_random_") {
        sprite->rotation = rand() % 360;
        return BlockResult::CONTINUE;
    }
    
    if (objectName == "_mouse_") {
        targetX = mousePointer.x;
        targetY = mousePointer.y;
    } else {
        for (Sprite* currentSprite : sprites) {
            if (currentSprite->name == objectName) {
                targetX = currentSprite->xPosition;
                targetY = currentSprite->yPosition;
                break;
            }
        }
    }
    
    const double dx = targetX - sprite->xPosition;
    const double dy = targetY - sprite->yPosition;
    double angle = 90 - (atan2(dy, dx) * 180.0 / M_PI);
    sprite->rotation = angle;
    // std::cout << "Pointing towards " << sprite->rotation << std::endl;
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::setRotationStyle(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    std::string value;
    try {
        value = block.fields.at("STYLE")[0];
    } catch (...) {
        std::cerr << "unable to find rotation style." << std::endl;
        return BlockResult::CONTINUE;
    }
    
    if (value == "left-right") {
        sprite->rotationStyle = "left-right";
    } else if (value == "don't rotate") {
        sprite->rotationStyle = "don't rotate";
    } else {
        sprite->rotationStyle = "all around";
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::ifOnEdgeBounce(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    double halfWidth = projectWidth / 2.0;
    double halfHeight = projectHeight / 2.0;
    
    // Check if the current sprite is touching the edge of the screen
    if (sprite->xPosition <= -halfWidth || sprite->xPosition >= halfWidth ||
        sprite->yPosition <= -halfHeight || sprite->yPosition >= halfHeight) {
        sprite->rotation *= -1;
    }
    return BlockResult::CONTINUE;
}

Value MotionBlocks::xPosition(Block& block,Sprite*sprite){
   return Value(sprite->xPosition); 
}

Value MotionBlocks::yPosition(Block& block,Sprite*sprite){
   return Value(sprite->yPosition); 
}

Value MotionBlocks::direction(Block& block,Sprite*sprite){
   return Value(sprite->rotation); 
}

