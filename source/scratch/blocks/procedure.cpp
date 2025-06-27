#include "procedure.hpp"

std::string ProcedureBlocks::stringNumber(const Block& block, Sprite* sprite) {
    return findCustomValue(block.fields.at("VALUE")[0], sprite, block);
}