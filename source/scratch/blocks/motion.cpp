#include "motion.hpp"

BlockResult MotionBlocks::moveSteps(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("STEPS"), &block, sprite);
            if (Math::isNumber(value)) {
                double angle = (sprite->rotation - 90) * M_PI / 180.0;
                sprite->xPosition += std::cos(angle) * std::stod(value);
                sprite->yPosition -= std::sin(angle) * std::stod(value);
            } else {
               // std::cerr << "Invalid Move steps " << value << std::endl;
            }
            
    return BlockResult::CONTINUE;

}

BlockResult MotionBlocks::goTo(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    Block* inputBlock = findBlock(block.inputs.at("TO")[1]);
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
    std::string xVal = Scratch::getInputValue(block.inputs.at("X"), &block, sprite);
    std::string yVal = Scratch::getInputValue(block.inputs.at("Y"), &block, sprite);
    if (Math::isNumber(xVal)) sprite->xPosition = std::stod(xVal);
    if (Math::isNumber(yVal)) sprite->yPosition = std::stod(yVal);
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::turnLeft(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("DEGREES"), &block, sprite);
    if (Math::isNumber(value)) {
        sprite->rotation -= std::stoi(value);
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::turnRight(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("DEGREES"), &block, sprite);
    if (Math::isNumber(value)) {
        sprite->rotation += std::stoi(value);
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::pointInDirection(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("DIRECTION"), &block, sprite);
    if (Math::isNumber(value)) {
        sprite->rotation = std::stoi(value);
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::changeXBy(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("DX"), &block, sprite);
    if (Math::isNumber(value)) {
        sprite->xPosition += std::stod(value);
    } else {
        std::cerr << "Invalid X position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::changeYBy(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("DY"), &block, sprite);
    if (Math::isNumber(value)) {
        sprite->yPosition += std::stod(value);
    } else {
        std::cerr << "Invalid Y position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::setX(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("X"), &block, sprite);
    if (Math::isNumber(value)) {
        sprite->xPosition = std::stod(value);
    } else {
        // std::cerr << "Invalid X position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::setY(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("Y"), &block, sprite);
    if (Math::isNumber(value)) {
        sprite->yPosition = std::stod(value);
    } else {
        // std::cerr << "Invalid Y position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::glideSecsToXY(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){
    
    if(block.repeatTimes == -1){
        block.repeatTimes = -6;
        
        std::string duration = Scratch::getInputValue(block.inputs.at("SECS"), &block, sprite);
        if(Math::isNumber(duration)) {
            block.waitDuration = std::stod(duration) * 1000;
        } else {
            block.waitDuration = 0;
        }
        
        block.waitStartTime = std::chrono::high_resolution_clock::now();
        block.glideStartX = sprite->xPosition;
        block.glideStartY = sprite->yPosition;
        
        // Get target positions
        std::string positionXStr = Scratch::getInputValue(block.inputs.at("X"), &block, sprite);
        std::string positionYStr = Scratch::getInputValue(block.inputs.at("Y"), &block, sprite);
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

BlockResult MotionBlocks::glideTo(Block& block, Sprite* sprite, Block** waitingBlock, bool* withoutScreenRefresh){

    if(block.repeatTimes == -1){
        block.repeatTimes = -7;
        
        std::string duration = Scratch::getInputValue(block.inputs.at("SECS"), &block, sprite);
        if(Math::isNumber(duration)) {
            block.waitDuration = std::stod(duration) * 1000;
        } else {
            block.waitDuration = 0;
        }
        
        block.waitStartTime = std::chrono::high_resolution_clock::now();
        block.glideStartX = sprite->xPosition;
        block.glideStartY = sprite->yPosition;
        
        Block* inputBlock;
        try {
            inputBlock = findBlock(block.inputs.at("TO")[1]);
        } catch(...) {
            return BlockResult::CONTINUE;
        }
        
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
    Block* inputBlock = findBlock(block.inputs.at("TOWARDS")[1]);
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

std::string MotionBlocks::xPosition(const Block& block,Sprite*sprite){
   return std::to_string(sprite->xPosition); 
}

std::string MotionBlocks::yPosition(const Block& block,Sprite*sprite){
   return std::to_string(sprite->yPosition); 
}

std::string MotionBlocks::direction(const Block& block,Sprite*sprite){
   return std::to_string(sprite->rotation); 
}

