#include "sensing.hpp"

BlockResult SensingBlocks::resetTimer(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh) {
    timer = 0;
    return BlockResult::CONTINUE;
}

BlockResult SensingBlocks::askAndWait(const Block& block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh) {
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

bool SensingBlocks::keyPressed(const Block& block, Sprite* sprite){
    Block* inputBlock = findBlock(block.inputs.at("KEY_OPTION")[1]);
    for (std::string button : inputButtons) {
        if (inputBlock->fields["KEY_OPTION"][0] == button) {
            return true;
        }
    }
    return false;
}

bool SensingBlocks::touchingObject(const Block& block, Sprite* sprite){
    Block* inputBlock = findBlock(block.inputs.at("TOUCHINGOBJECTMENU")[1]);
    std::string objectName;
    try {
        objectName = inputBlock->fields["TOUCHINGOBJECTMENU"][0];
    } catch (...) {
        return false;
    }

    // Get collision points of the current sprite
    std::vector<std::pair<double, double>> currentSpritePoints = getCollisionPoints(sprite);

    if(objectName == "_mouse_") {
        // Check if the mouse pointer's position is within the bounds of the current sprite
        if (mousePointer.x >= sprite->xPosition - sprite->spriteWidth / 2 &&
            mousePointer.x <= sprite->xPosition + sprite->spriteWidth / 2 &&
            mousePointer.y >= sprite->yPosition - sprite->spriteHeight / 2 &&
            mousePointer.y <= sprite->yPosition + sprite->spriteHeight / 2) {
            return true;
        }
        return false;
    }

    if (objectName == "_edge_") {
        double halfWidth = projectWidth / 2.0;
        double halfHeight = projectHeight / 2.0;

        // Check if the current sprite is touching the edge of the screen
        if (sprite->xPosition <= -halfWidth || sprite->xPosition >= halfWidth ||
            sprite->yPosition <= -halfHeight || sprite->yPosition >= halfHeight) {
            return true;
        }
        return false;
    }

    for (Sprite* targetSprite : sprites) {
        if (targetSprite->name == objectName) {
            // Get collision points of the target sprite
            std::vector<std::pair<double, double>> targetSpritePoints = getCollisionPoints(targetSprite);

            // Check if any point of the current sprite is inside the target sprite's bounds
            for (const auto& point : currentSpritePoints) {
                if (point.first >= targetSprite->xPosition - targetSprite->spriteWidth / 2 &&
                    point.first <= targetSprite->xPosition + targetSprite->spriteWidth / 2 &&
                    point.second >= targetSprite->yPosition - targetSprite->spriteHeight / 2 &&
                    point.second <= targetSprite->yPosition + targetSprite->spriteHeight / 2) {
                    return true;
                }
            }

            // Check if any point of the target sprite is inside the current sprite's bounds
            for (const auto& point : targetSpritePoints) {
                if (point.first >= sprite->xPosition - sprite->spriteWidth / 2 &&
                    point.first <= sprite->xPosition + sprite->spriteWidth / 2 &&
                    point.second >= sprite->yPosition - sprite->spriteHeight / 2 &&
                    point.second <= sprite->yPosition + sprite->spriteHeight / 2) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool SensingBlocks::mouseDown(const Block& block, Sprite* sprite){
    return mousePointer.isPressed;
}