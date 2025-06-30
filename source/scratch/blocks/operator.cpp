#include "operator.hpp"

std::string OperatorBlocks::add(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("NUM1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("NUM2"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        double result = std::stod(value1) + std::stod(value2);
        if (std::floor(result) == result) {
            return std::to_string(static_cast<int>(result));
        }
        return std::to_string(result);
    }
    return "0";
}

std::string OperatorBlocks::subtract(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("NUM1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("NUM2"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        double result = std::stod(value1) - std::stod(value2);
        if (std::floor(result) == result) {
            return std::to_string(static_cast<int>(result));
        }
        return std::to_string(result);
    }
    return "0";
}

std::string OperatorBlocks::multiply(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("NUM1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("NUM2"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        double result = std::stod(value1) * std::stod(value2);
        if (std::floor(result) == result) {
            return std::to_string(static_cast<int>(result));
        }
        return std::to_string(result);
    }
    return "0";
}

std::string OperatorBlocks::divide(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("NUM1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("NUM2"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        double result = std::stod(value1) / std::stod(value2);
        if (std::floor(result) == result) {
            return std::to_string(static_cast<int>(result));
        }
        return std::to_string(result);
    }
    return "0";
}

std::string OperatorBlocks::random(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("FROM"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("TO"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        if (value1.find('.') == std::string::npos && value2.find('.') == std::string::npos) {
            // Both are integers
            int from = std::stoi(value1);
            int to = std::stoi(value2);
            return std::to_string(rand() % (to - from + 1) + from);
        } else {
            // At least one is a decimal
            double from = std::stod(value1);
            double to = std::stod(value2);
            return std::to_string(from + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (to - from))));
        }
    }
    return "0";
}

std::string OperatorBlocks::join(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("STRING1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("STRING2"), &block, sprite);
    return value1 + value2;
}

std::string OperatorBlocks::letterOf(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("LETTER"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("STRING"), &block, sprite);
    if (Math::isNumber(value1) && !value2.empty()) {
        int index = std::stoi(value1) - 1;
        if (index >= 0 && index < static_cast<int>(value2.size())) {
            return std::string(1, value2[index]);
        }
    }
    return "";
}

std::string OperatorBlocks::length(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("STRING"), &block, sprite);
    return std::to_string(value1.size());
}

std::string OperatorBlocks::mod(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("NUM1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("NUM2"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        if (floor(std::stod(value1)) == std::stod(value1) && floor(std::stod(value2)) == std::stod(value2)) {
            // Both are integers
            return std::to_string(static_cast<int>(std::fmod(std::stod(value1), std::stod(value2))));
        }
        return std::to_string(std::fmod(std::stod(value1), std::stod(value2)));
    }
    return "0";
}

std::string OperatorBlocks::round(const Block& block, Sprite* sprite) {
    std::string value1 = Scratch::getInputValue(block.inputs.at("NUM"), &block, sprite);
    if (Math::isNumber(value1)) {
        return std::to_string(static_cast<int>(std::round(std::stod(value1))));
    }
    return "0";
}

std::string OperatorBlocks::mathOp(const Block& block, Sprite* sprite) {
    std::string inputValue = Scratch::getInputValue(block.inputs.at("NUM"), &block, sprite);
    if (Math::isNumber(inputValue)) {
        std::string operation = block.fields.at("OPERATOR")[0];
        double value = std::stod(inputValue);
        
        if (operation == "abs") {
            return std::to_string(abs(value));
        }
        if (operation == "floor") {
            return std::to_string(static_cast<int>(floor(value)));
        }
        if (operation == "ceiling") {
            return std::to_string(static_cast<int>(ceil(value)));
        }
        if (operation == "sqrt") {
            return std::to_string(sqrt(value));
        }
        if (operation == "sin") {
            return std::to_string(sin(value * M_PI / 180.0));
        }
        if (operation == "cos") {
            return std::to_string(cos(value * M_PI / 180.0));
        }
        if (operation == "tan") {
            return std::to_string(tan(value * M_PI / 180.0));
        }
        if (operation == "asin") {
            return std::to_string(asin(value) * 180.0 / M_PI);
        }
        if (operation == "acos") {
            return std::to_string(acos(value) * 180.0 / M_PI);
        }
        if (operation == "atan") {
            return std::to_string(atan(value) * 180.0 / M_PI);
        }
        if (operation == "ln") {
            return std::to_string(log(value));
        }
        if (operation == "log") {
            return std::to_string(log10(value));
        }
        if (operation == "e ^") {
            return std::to_string(exp(value));
        }
        if (operation == "10 ^") {
            return std::to_string(pow(10, value));
        }
    }
    return "0";
}

bool OperatorBlocks::equals(const Block& block, Sprite* sprite){
    std::string value1;
    std::string value2;
    try{
        value1 = Scratch::getInputValue(block.inputs.at("OPERAND1"), &block, sprite);
        value2 = Scratch::getInputValue(block.inputs.at("OPERAND2"), &block, sprite);
    }
    catch(...){
        std::cout << "failed to get equals values." << std::endl;
        return false;
    }
    
    try{
        if(std::floor(std::stod(value1)) == std::stod(value1) && std::floor(std::stod(value2)) == std::stod(value2) ){
            return (std::floor(std::stod(value1)) == std::floor(std::stod(value2)));
        }
    }
    catch(...){
        // If conversion fails, fall back to string comparison
    }

    return value1 == value2;
}

bool OperatorBlocks::greaterThan(const Block& block, Sprite* sprite){
    std::string value1 = Scratch::getInputValue(block.inputs.at("OPERAND1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("OPERAND2"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        return std::stod(value1) > std::stod(value2);
    }
    return false;
}

bool OperatorBlocks::lessThan(const Block& block, Sprite* sprite){
    std::string value1 = Scratch::getInputValue(block.inputs.at("OPERAND1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("OPERAND2"), &block, sprite);
    if (Math::isNumber(value1) && Math::isNumber(value2)) {
        return std::stod(value1) < std::stod(value2);
    }
    return false;
}

bool OperatorBlocks::and_(const Block& block, Sprite* sprite){
    bool value1 = executor.runConditionalBlock(block.inputs.at("OPERAND1")[1], sprite);
    bool value2 = executor.runConditionalBlock(block.inputs.at("OPERAND2")[1], sprite);
    return value1 && value2;
}

bool OperatorBlocks::or_(const Block& block, Sprite* sprite){
    bool value1 = executor.runConditionalBlock(block.inputs.at("OPERAND1")[1], sprite);
    bool value2 = executor.runConditionalBlock(block.inputs.at("OPERAND2")[1], sprite);
    return value1 || value2;
}

bool OperatorBlocks::not_(const Block& block, Sprite* sprite){
    bool value = executor.runConditionalBlock(block.inputs.at("OPERAND")[1], sprite);
    return !value;
}

bool OperatorBlocks::contains(const Block& block, Sprite* sprite){
    std::string value1 = Scratch::getInputValue(block.inputs.at("STRING1"), &block, sprite);
    std::string value2 = Scratch::getInputValue(block.inputs.at("STRING2"), &block, sprite);
    return value1.find(value2) != std::string::npos;
}