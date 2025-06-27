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