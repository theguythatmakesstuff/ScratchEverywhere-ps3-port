#include "data.hpp"

BlockResult DataBlocks::setVariable(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string val = Scratch::getInputValue(block.inputs.at("VALUE"), &block, sprite);
    std::string varId = block.fields.at("VALUE")[1];
    setVariableValue(varId, val, sprite, false);
    return BlockResult::CONTINUE;
}

BlockResult DataBlocks::changeVariable(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string val = Scratch::getInputValue(block.inputs.at("VALUE"), &block, sprite);
    std::string varId = block.fields.at("VALUE")[1];
    setVariableValue(varId, val, sprite, true);
    return BlockResult::CONTINUE;
}

BlockResult DataBlocks::addToList(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string val = Scratch::getInputValue(block.inputs.at("ITEM"), &block, sprite);
    std::string listId = block.fields.at("LIST")[1];
    for(Sprite* currentSprite : sprites){
    if (currentSprite->lists.find(listId) != currentSprite->lists.end()) {
        // std::cout << "Adding to list " << listId << std::endl;
        sprite->lists[listId].items.push_back(val);
        break;
    }
}
    return BlockResult::CONTINUE;
}

BlockResult DataBlocks::deleteFromList(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string val = Scratch::getInputValue(block.inputs.at("INDEX"), &block, sprite);
    std::string listId = block.fields.at("LIST")[1];

    for (Sprite* currentSprite : sprites) {
        if (currentSprite->lists.find(listId) != currentSprite->lists.end()) {
            //std::cout << "Deleting from list " << listId << std::endl;

            // Convert `val` to an integer index
            if (isNumber(val)) {
                int index = std::stoi(val) - 1; // Convert to 0-based index
                auto& items = currentSprite->lists[listId].items;

                // Check if the index is within bounds
                if (index >= 0 && index < static_cast<int>(items.size())) {
                    items.erase(items.begin() + index); // Remove the item at the index
                } else {
                    // std::cerr << "Delete list Index out of bounds: " << index << std::endl;
                }
            } else {
                // std::cerr << "Invalid Delete list index: " << val << std::endl;
            }
            break;
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult DataBlocks::deleteAllOfList(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string listId = block.fields.at("LIST")[1];
    for (Sprite* currentSprite : sprites) {
        if (currentSprite->lists.find(listId) != currentSprite->lists.end()) {
            //std::cout << "Deleting all from list " << listId << std::endl;
            currentSprite->lists[listId].items.clear(); // Clear the list
            break;
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult DataBlocks::insertAtList(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string val = Scratch::getInputValue(block.inputs.at("ITEM"), &block, sprite);
    std::string listId = block.fields.at("LIST")[1];
    std::string index = Scratch::getInputValue(block.inputs.at("INDEX"), &block, sprite);

    for (Sprite* currentSprite : sprites) {
        if (currentSprite->lists.find(listId) != currentSprite->lists.end()) {
            // std::cout << "Inserting into list " << listId << std::endl;

            // Convert `index` to an integer index
            if (isNumber(index)) {
                int idx = std::stoi(index) - 1; // Convert to 0-based index
                auto& items = currentSprite->lists[listId].items;

                // Check if the index is within bounds
                if (idx >= 0 && idx <= static_cast<int>(items.size())) {
                    items.insert(items.begin() + idx, val); // Insert the item at the index
                } else {
                    //std::cerr << "Insert Index out of bounds: " << idx << std::endl;
                }
            } else {
                // std::cerr << "Invalid Insert index: " << index << std::endl;
            }
            break;
        }
    }
    return BlockResult::CONTINUE;
}

BlockResult DataBlocks::replaceItemOfList(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
    std::string val = Scratch::getInputValue(block.inputs.at("ITEM"), &block, sprite);
    std::string listId = block.fields.at("LIST")[1];
    std::string index = Scratch::getInputValue(block.inputs.at("INDEX"), &block, sprite);

    for (Sprite* currentSprite : sprites) {
        if (currentSprite->lists.find(listId) != currentSprite->lists.end()) {
            //std::cout << "Replacing item in list " << listId << std::endl;

            // Convert `index` to an integer index
            if (isNumber(index)) {
                int idx = std::stoi(index) - 1; // Convert to 0-based index
                auto& items = currentSprite->lists[listId].items;

                // Check if the index is within bounds
                if (idx >= 0 && idx < static_cast<int>(items.size())) {
                    items[idx] = val; // Replace the item at the index
                } else {
                    // std::cerr << "Replace item Index out of bounds: " << idx << std::endl;
                }
            } else {
                // std::cerr << "Invalid Replace index: " << index << std::endl;
            }
            break;
        }
    }
    return BlockResult::CONTINUE;
}