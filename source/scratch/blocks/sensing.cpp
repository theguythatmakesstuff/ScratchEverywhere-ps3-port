#include "sensing.hpp"

BlockResult SensingBlocks::resetTimer(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    timer = 0;
    return BlockResult::CONTINUE;
}

BlockResult SensingBlocks::askAndWait(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh) {
    Keyboard kbd;
    std::string inputValue = Scratch::getInputValue(block.inputs.at("QUESTION"),&block,sprite);
    std::string output = kbd.openKeyboard(inputValue.c_str());
    answer = output;
    return BlockResult::CONTINUE;
}

std::string SensingBlocks::sensingTimer(const Block& block, Sprite* sprite) {
    return std::to_string(timer);
}

std::string SensingBlocks::of(const Block& block, Sprite* sprite) {
    std::string value = block.fields.at("PROPERTY")[0];
    std::string object;
    try {
        object = findBlock(block.inputs.at("OBJECT")[1])->fields.at("OBJECT")[0];
    } catch (...) {
        return "0";
    }
    
    Sprite* spriteObject = nullptr;
    for (Sprite* currentSprite : sprites) {
        if (currentSprite->name == object && !currentSprite->isClone) {
            spriteObject = currentSprite;
            break;
        }
    }
    
    if (!spriteObject) return "0";
    
    if (value == "timer") {
        return std::to_string(timer);
    } else if (value == "x position") {
        return std::to_string(spriteObject->xPosition);
    } else if (value == "y position") {
        return std::to_string(spriteObject->yPosition);
    } else if (value == "direction") {
        return std::to_string(spriteObject->rotation);
    } else if (value == "costume #" || value == "backdrop #") {
        return std::to_string(spriteObject->currentCostume + 1);
    } else if (value == "costume name" || value == "backdrop name") {
        return spriteObject->costumes[spriteObject->currentCostume].name;
    } else if (value == "size") {
        return std::to_string(spriteObject->size);
    } else if (value == "volume") {
        return std::to_string(spriteObject->volume);
    }
    
    for (const auto& [id, variable] : spriteObject->variables) {
        if (value == variable.name) {
            return variable.value;
        }
    }
    return "0";
}

std::string SensingBlocks::mouseX(const Block& block, Sprite* sprite) {
    return std::to_string(mousePointer.x);
}

std::string SensingBlocks::mouseY(const Block& block, Sprite* sprite) {
    return std::to_string(mousePointer.y);
}

std::string SensingBlocks::distanceTo(const Block& block, Sprite* sprite) {
    Block* inputBlock = findBlock(block.inputs.at("DISTANCETOMENU")[1]);
    std::string object = inputBlock->fields.at("DISTANCETOMENU")[0];
    
    if (object == "_mouse_") {
        return std::to_string(sqrt(pow(mousePointer.x - sprite->xPosition, 2) + 
                                 pow(mousePointer.y - sprite->yPosition, 2)));
    }
    
    for (Sprite* currentSprite : sprites) {
        if (currentSprite->name == object && !currentSprite->isClone) {
            double distance = sqrt(pow(currentSprite->xPosition - sprite->xPosition, 2) + 
                                 pow(currentSprite->yPosition - sprite->yPosition, 2));
            return std::to_string(distance);
        }
    }
    return "0";
}

std::string SensingBlocks::daysSince2000(const Block& block, Sprite* sprite) {
    return std::to_string(Time::getDaysSince2000());
}

std::string SensingBlocks::current(const Block& block, Sprite* sprite) {
    std::string inputValue;
    try {
        inputValue = block.fields.at("CURRENTMENU")[0];
    } catch (...) {
        return "";
    }
    
    if (inputValue == "YEAR") return std::to_string(Time::getYear());
    if (inputValue == "MONTH") return std::to_string(Time::getMonth());
    if (inputValue == "DATE") return std::to_string(Time::getDay());
    if (inputValue == "DAYOFWEEK") return std::to_string(Time::getDayOfWeek());
    if (inputValue == "HOUR") return std::to_string(Time::getHours());
    if (inputValue == "MINUTE") return std::to_string(Time::getMinutes());
    if (inputValue == "SECOND") return std::to_string(Time::getSeconds());
    
    return "";
}

std::string SensingBlocks::sensingAnswer(const Block& block, Sprite* sprite) {
    return answer;
}