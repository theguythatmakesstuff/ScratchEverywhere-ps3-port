#include "procedure.hpp"

std::string ProcedureBlocks::stringNumber(const Block& block, Sprite* sprite) {
    return findCustomValue(block.fields.at("VALUE")[0], sprite, block);
}

bool ProcedureBlocks::booleanArgument(const Block& block, Sprite* sprite){
    std::string value = findCustomValue(block.fields.at("VALUE")[0], sprite, block);
    return value == "1";
}