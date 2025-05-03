#include "interpret.hpp"

std::vector<Sprite> sprites;
double timer = 0;


bool isNumber(const std::string& str) {
    // i rewrote this function like 5 times vro if ts dont work...
    if (str.empty()) return false; // Reject empty strings

    size_t start = 0;
    if (str[0] == '-') { // Allow negative numbers
        if (str.size() == 1) return false; // just "-" alone is invalid
        start = 1; // Skip '-' for digit checking
    }

    return std::count(str.begin() + start, str.end(), '.') <= 1 && // At most one decimal point
           std::any_of(str.begin() + start, str.end(), ::isdigit) && // At least one digit
           std::all_of(str.begin() + start, str.end(), [](char c) { return std::isdigit(c) || c == '.'; });
}



void loadSprites(const nlohmann::json& json){
    std::cout<<"Beginning to load sprites..."<< std::endl;
    for (const auto& target : json["targets"]){ // "target" is sprite in Scratch speak, so for every sprite in sprites
    
        Sprite newSprite;
        if(target.contains("name")){
        newSprite.name = target["name"].get<std::string>();}
        if(target.contains("isStage")){
        newSprite.isStage = target["isStage"].get<bool>();}
        if(target.contains("draggable")){
        newSprite.draggable = target["draggable"].get<bool>();}
        if(target.contains("visible")){
        newSprite.visible = target["visible"].get<bool>();}
        if(target.contains("currentCostume")){
        newSprite.currentCostume = target["currentCostume"].get<int>();}
        if(target.contains("volume")){
        newSprite.volume = target["volume"].get<int>();}
        if(target.contains("x")){
        newSprite.xPosition = target["x"].get<int>();}
        if(target.contains("y")){
        newSprite.yPosition = target["y"].get<int>();}
        if(target.contains("size")){
        newSprite.size = target["size"].get<int>();}
        if(target.contains("direction")){
        newSprite.rotation = target["direction"].get<int>();}
        if(target.contains("layerOrder")){
        newSprite.layer = target["layerOrder"].get<int>();}
        if(target.contains("rotationStyle")){
        newSprite.rotationStyle = target["rotationStyle"].get<std::string>();}
       // std::cout<<"name = "<< newSprite.name << std::endl;


        // set variables
        for (const auto& [id,data] : target["variables"].items()){
            
            Variable newVariable;
            newVariable.id = id;
            newVariable.name = data[0];
            //newVariable.value = std::to_string(data[1]);
            if (data[1].is_number_integer()) {
                newVariable.value = std::to_string(data[1].get<int>());
            } else if (data[1].is_number_float()) {
                newVariable.value = std::to_string(data[1].get<double>());
            } else if (data[1].is_string()) {
                newVariable.value = data[1].get<std::string>();
            } else {
                newVariable.value = "0";
            }
            newSprite.variables[newVariable.id] = newVariable; // add variable to sprite
        }

        // set Blocks
        for(const auto& [id,data] : target["blocks"].items()){

            Block newBlock;
            newBlock.id = id;
            if (data.contains("opcode")){
            newBlock.opcode = data["opcode"].get<std::string>();}
            if (data.contains("next") && !data["next"].is_null()){
            newBlock.next = data["next"].get<std::string>();}
            //else newBlock.next = "";
           // std::cout<<"next = "<< newBlock.next << std::endl;
            if (data.contains("parent") && !data["parent"].is_null()){
            newBlock.parent = data["parent"].get<std::string>();}
            if (data.contains("fields")){
            newBlock.fields = data["fields"];}
            if (data.contains("inputs")){
            newBlock.inputs = data["inputs"];}
            if (data.contains("topLevel")){
            newBlock.topLevel = data["topLevel"].get<bool>();}
            if (data.contains("shadow")){
            newBlock.shadow = data["shadow"].get<bool>();}
            newSprite.blocks[newBlock.id] = newBlock; // add block
        }

        // set Lists
        for(const auto &[id,data] : target["lists"].items()){
            List newList;
            newList.id = id;
            newList.name = data[0];
            for(const auto &listItem : data[1]){
            newList.items.push_back(listItem.dump());
            }
            newSprite.lists[newList.id] = newList; // add list
        }

        // set Sounds
        for(const auto &[id,data] : target["sounds"].items()){
            Sound newSound;
            newSound.id = data["assetId"];
            newSound.name = data["name"];
            newSound.fullName = data["md5ext"];
            newSound.dataFormat = data["dataFormat"];
            newSound.sampleRate = data["rate"];
            newSound.sampleCount = data["sampleCount"];
            newSprite.sounds[newSound.id] = newSound;
        }

        // set Costumes
        for(const auto &[id,data] : target["costumes"].items()){
            Costume newCostume;
            newCostume.id = data["assetId"];
            newCostume.name = data["name"];
            if(data.contains("bitmapResolution")){
            newCostume.bitmapResolution = data["bitmapResolution"];}
            newCostume.dataFormat = data["dataFormat"];
            newCostume.fullName = data["md5ext"];
            if(data.contains("rotationCenterX")){
            newCostume.rotationCenterX = data["rotationCenterX"];}
            if(data.contains("rotationCenterY")){
            newCostume.rotationCenterY = data["rotationCenterY"];}
            newSprite.costumes[newCostume.id] = newCostume;
        }

       // set comments
        for(const auto &[id,data] : target["comments"].items()){
            Comment newComment;
            newComment.id = id;
            if(data.contains("blockId") && !data["blockId"].is_null()){
            newComment.blockId = data["blockId"];}
            newComment.width = data["width"];
            newComment.height = data["height"];
            newComment.minimized = data["minimized"];
            newComment.x = data["x"];
            newComment.y = data["y"];
            newComment.text = data["text"];
            newSprite.comments[newComment.id] = newComment;
        }

        // set Broadcasts
        for(const auto &[id,data] : target["broadcasts"].items()){
            Broadcast newBroadcast;
            newBroadcast.id = id;
            newBroadcast.name = data;
            newSprite.broadcasts[newBroadcast.id] = newBroadcast;
           // std::cout<<"broadcast name = "<< newBroadcast.name << std::endl;
        }

        sprites.push_back(newSprite);
    }
}

std::string getValueOfBlock(Block block,Sprite*sprite){
    if (block.opcode == "motion_xposition") {
        return std::to_string(sprite->xPosition);
    }
    if (block.opcode == "motion_yposition") {
        return std::to_string(sprite->yPosition);
    }
    if(block.opcode == "motion_direction"){
        return std::to_string(sprite->rotation);
    }
    if(block.opcode == "looks_size"){
        return std::to_string(sprite->size);
    }
    if(block.opcode == "sound_volume"){
        return std::to_string(sprite->volume);
    }
    if(block.opcode == "sensing_timer"){
        return std::to_string(timer);
    }
    return "";
}

Block findBlock(std::string blockId){
    for(Sprite currentSprite : sprites){
        auto block = currentSprite.blocks.find(blockId);
        if(block != currentSprite.blocks.end()){
           // std::cout << "Found Block " << block->second.id << "\n";
            return block->second;
        }
    }
    Block null;
    null.id = "null";
    return null;
}

void runBlock(Block block,Sprite*sprite){
    if (block.opcode == "motion_gotoxy") {
        std::string xVal;
        std::string yVal;


        xVal = getInputValue(block.inputs["X"],&block,sprite);
        yVal = getInputValue(block.inputs["Y"],&block,sprite);
        if (isNumber(xVal))
        sprite->xPosition = std::stoi(xVal);
        else{std::cout<<"Set X Position invalid with pos " << xVal<< std::endl;}
        if (isNumber(yVal))
        sprite->yPosition = std::stoi(yVal);
        else{std::cout<<"Set Y Position invalid with pos " << xVal<< std::endl;}
        goto nextBlock;
    }
    if(block.opcode == "data_setvariableto"){
        std::string val;
        std::string varId = block.fields["VARIABLE"][1];
        val = getInputValue(block.inputs["VALUE"],&block,sprite);
        std::cout<<"Setting Variable " << block.fields["VARIABLE"][0] << "from " << getVariableValue(varId) << std::endl;
        setVariableValue(varId,val,sprite,false);
        goto nextBlock;
    }
    if(block.opcode == "data_changevariableby"){
        std::string val;
        std::string varId = block.fields["VARIABLE"][1];
        val = getInputValue(block.inputs["VALUE"],&block,sprite);
        std::cout<<"Changing Variable " << block.fields["VARIABLE"][0] << "from " << getVariableValue(varId) << std::endl;
        setVariableValue(varId,val,sprite,true);
        goto nextBlock;
    }
    if(block.opcode == "sensing_resettimer"){
        timer = 0;
        goto nextBlock;
    }



nextBlock:
if(!block.next.empty()){
    Block nextBlock = findBlock(block.next);
    if (nextBlock.id != "null"){
        runBlock(nextBlock,sprite);
    }
}
}



void setVariableValue(std::string variableId,std::string value,Sprite* sprite,bool isChangingBy){
    if(sprite->variables.find(variableId) != sprite->variables.end()){ // if not a global Variable
        Variable& var = sprite->variables[variableId];


        if(!isChangingBy){
            var.value = value;
        }
        else{
            if(isNumber(var.value) && isNumber(value)){
            var.value = std::to_string(std::stod(var.value) + std::stod(value));
            }
            else{
                if(!isNumber(value)) return;
                var.value = value;
            }
        }

        std::cout<<"Local Variable set. " << sprite->variables[variableId].value << std::endl;

    }
    // global Variable (TODO fix redundant code later :grin:)
    else{
        for(Sprite &currentSprite : sprites){
            if (currentSprite.isStage){
                Variable& var = currentSprite.variables[variableId];
        
                if(!isChangingBy){
                    var.value = value;
                }
                else{
                    if(isNumber(var.value) && isNumber(value)){
                    var.value = std::to_string(std::stod(var.value) + std::stod(value));
                    }
                    else{
                        if(!isNumber(value)) return;
                        var.value = value;
                    }
                }
        
                std::cout<<"Global Variable set. " << var.value << std::endl;
            }
        }
    }

}

std::string getVariableValue(std::string variableId){
    for(Sprite &currentSprite : sprites){
        for(const auto &[id,data] : currentSprite.variables){
            if (id == variableId) {
                return data.value; // Assuming `Variable` has a `value` field
            }
        }
        // check if it's a list instead
        std::cout<<"Checking list " << variableId << std::endl;
        for(const auto &[id,data] : currentSprite.lists){
            if(id == variableId){
                std::string finalValue;
                for(const auto &listItem : data.items){
                    std::cout<<"Found one " << std::endl;
                    finalValue += listItem + " ";
                }
                finalValue.pop_back(); // remove extra space
                return finalValue;
            }

        }
    }
    return "";
}

std::string getInputValue(nlohmann::json item, Block* block, Sprite* sprite) {
    // 1 is just a plain number
    if (item[0] == 1) {
        return item[1][1].get<std::string>(); // Convert JSON to string
    }
    // 3 is if there is a variable of some kind inside
    if (item[0] == 3) {
        if (item[1].is_array()) {
            return getVariableValue(item[1][2]);
        } else {
            return getValueOfBlock(findBlock(item[1]), sprite);
        }
    }
    return "0";
}




std::vector<Sprite*> findSprite(std::string spriteId){
    std::vector<Sprite*> sprts;
    for(Sprite currentSprite : sprites){
        if(currentSprite.name == spriteId){
            sprts.push_back(&currentSprite);
        }
    }
    return sprts;
}



void runAllBlocksByOpcode(std::string opcodeToFind){
    std::cout << "Running all " << opcodeToFind << " blocks." << "\n";
    for(Sprite &currentSprite : sprites){
        for(auto &[id,data] : currentSprite.blocks){
            if(data.opcode == opcodeToFind){
                runBlock(data,&currentSprite);
            }
        }
    }
}