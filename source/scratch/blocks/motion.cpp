#include "motion.hpp"

BlockResult MotionBlocks::moveSteps(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("STEPS"), &block, sprite);
            if (isNumber(value)) {
                double angle = (sprite->rotation - 90) * M_PI / 180.0;
                sprite->xPosition += std::cos(angle) * std::stod(value);
                sprite->yPosition += std::sin(angle) * std::stod(value);
            } else {
               // std::cerr << "Invalid Move steps " << value << std::endl;
            }
            
    return BlockResult::CONTINUE;

}

BlockResult MotionBlocks::goTo(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
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

BlockResult MotionBlocks::goToXY(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string xVal = Scratch::getInputValue(block.inputs.at("X"), &block, sprite);
    std::string yVal = Scratch::getInputValue(block.inputs.at("Y"), &block, sprite);
    if (isNumber(xVal)) sprite->xPosition = std::stod(xVal);
    if (isNumber(yVal)) sprite->yPosition = std::stod(yVal);
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::turnLeft(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("DEGREES"), &block, sprite);
    if (isNumber(value)) {
        sprite->rotation -= std::stoi(value);
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::turnRight(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("DEGREES"), &block, sprite);
    if (isNumber(value)) {
        sprite->rotation += std::stoi(value);
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::pointInDirection(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string value = Scratch::getInputValue(block.inputs.at("DIRECTION"), &block, sprite);
    if (isNumber(value)) {
        sprite->rotation = std::stoi(value);
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::changeXBy(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("DX"), &block, sprite);
    if (isNumber(value)) {
        sprite->xPosition += std::stod(value);
    } else {
        std::cerr << "Invalid X position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::changeYBy(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("DY"), &block, sprite);
    if (isNumber(value)) {
        sprite->yPosition += std::stod(value);
    } else {
        std::cerr << "Invalid Y position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::setX(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("X"), &block, sprite);
    if (isNumber(value)) {
        sprite->xPosition = std::stod(value);
    } else {
        // std::cerr << "Invalid X position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::setY(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    std::string value = Scratch::getInputValue(block.inputs.at("Y"), &block, sprite);
    if (isNumber(value)) {
        sprite->yPosition = std::stod(value);
    } else {
        // std::cerr << "Invalid Y position " << value << std::endl;
    }
    return BlockResult::CONTINUE;
}

BlockResult MotionBlocks::pointToward(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
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

BlockResult MotionBlocks::setRotationStyle(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
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

BlockResult MotionBlocks::ifOnEdgeBounce(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    double halfWidth = projectWidth / 2.0;
    double halfHeight = projectHeight / 2.0;
    
    // Check if the current sprite is touching the edge of the screen
    if (sprite->xPosition <= -halfWidth || sprite->xPosition >= halfWidth ||
        sprite->yPosition <= -halfHeight || sprite->yPosition >= halfHeight) {
        sprite->rotation *= -1;
    }
    return BlockResult::CONTINUE;
}