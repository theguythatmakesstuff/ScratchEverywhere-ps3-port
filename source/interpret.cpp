#include "interpret.hpp"

std::list<Sprite> sprites;
std::vector<std::string> broadcastQueue;
std::unordered_map<std::string,Conditional> conditionals;
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

std::string generateRandomString(int length) {
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    srand(time(0)); // Seed the random number generator

    for (int i = 0; i < length; i++) {
        result += chars[rand() % chars.size()];
    }
    
    return result;
}

std::string removeQuotations(std::string value) {
    value.erase(std::remove_if(value.begin(),value.end(),[](char c){return c == '"';}),value.end());
    return value;
}


void loadSprites(const nlohmann::json& json){
    std::cout<<"Beginning to load sprites..."<< std::endl;
    for (const auto& target : json["targets"]){ // "target" is sprite in Scratch speak, so for every sprite in sprites
    
        Sprite newSprite;
        if(target.contains("name")){
        newSprite.name = target["name"].get<std::string>();}
        newSprite.id = generateRandomString(15);
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
            newBlock.opcode = newBlock.stringToOpcode(data["opcode"].get<std::string>());}
            if (data.contains("next") && !data["next"].is_null()){
            newBlock.next = data["next"].get<std::string>();}
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
            if (data.contains("mutation")){
                newBlock.mutation = data["mutation"];
            }
            newSprite.blocks[newBlock.id] = newBlock; // add block

            // add custom function blocks
            if(newBlock.opcode == newBlock.PROCEDURES_PROTOTYPE){
                CustomBlock newCustomBlock;
                newCustomBlock.name = data["mutation"]["proccode"];
                newCustomBlock.blockId = newBlock.id;

                // custom blocks uses a different json structure for some reason?? have to parse them.
                std::string rawArgumentNames = data["mutation"]["argumentnames"];
                nlohmann::json parsedAN = nlohmann::json::parse(rawArgumentNames);
                newCustomBlock.argumentNames = parsedAN.get<std::vector<std::string>>();

                std::string rawArgumentDefaults = data["mutation"]["argumentdefaults"];
                nlohmann::json parsedAD = nlohmann::json::parse(rawArgumentDefaults);
                newCustomBlock.argumentDefaults = parsedAD.get<std::vector<std::string>>();

                std::string rawArgumentIds = data["mutation"]["argumentids"];
                nlohmann::json parsedAID = nlohmann::json::parse(rawArgumentIds);
                newCustomBlock.argumentIds = parsedAID.get<std::vector<std::string>>();

                if(data["mutation"]["warp"] == "true"){
                newCustomBlock.runWithoutScreenRefresh = true;}
                else newCustomBlock.runWithoutScreenRefresh = false;

                newSprite.customBlocks[newCustomBlock.name] = newCustomBlock; // add custom block
            }

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
    std::cout<<"Loaded " << sprites.size() << " sprites."<< std::endl;
}

std::string getValueOfBlock(Block block,Sprite*sprite){
    switch (block.opcode) {
        case Block::ARGUMENT_REPORTER_STRING_NUMBER: {
            return findCustomValue(block.fields["VALUE"][0], sprite, block);
        }
        case Block::MOTION_XPOSITION: {
            return std::to_string(sprite->xPosition);
        }
        case Block::MOTION_YPOSITION: {
            return std::to_string(sprite->yPosition);
        }
        case Block::MOTION_DIRECTION: {
            return std::to_string(sprite->rotation);
        }
        case Block::LOOKS_SIZE: {
            return std::to_string(sprite->size);
        }
        case Block::SOUND_VOLUME: {
            return std::to_string(sprite->volume);
        }
        case Block::SENSING_TIMER: {
            return std::to_string(timer);
        }
        case Block::OPERATOR_ADD: {
            std::string value1 = getInputValue(block.inputs["NUM1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["NUM2"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                return std::to_string(std::stod(value1) + std::stod(value2));
            }
            break;
        }
        case Block::OPERATOR_SUBTRACT: {
            std::string value1 = getInputValue(block.inputs["NUM1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["NUM2"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                return std::to_string(std::stod(value1) - std::stod(value2));
            }
            break;
        }
        case Block::OPERATOR_MULTIPLY: {
            std::string value1 = getInputValue(block.inputs["NUM1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["NUM2"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                return std::to_string(std::stod(value1) * std::stod(value2));
            }
            break;
        }
        case Block::OPERATOR_DIVIDE: {
            std::string value1 = getInputValue(block.inputs["NUM1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["NUM2"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                return std::to_string(std::stod(value1) / std::stod(value2));
            }
            break;
        }
        case Block::OPERATOR_RANDOM: {
            std::string value1 = getInputValue(block.inputs["FROM"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["TO"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                if (value1.find('.') == std::string::npos && value2.find('.') == std::string::npos) {
                    int from = std::stoi(value1);
                    int to = std::stoi(value2);
                    return std::to_string(rand() % (to - from + 1) + from);
                } else {
                    double from = std::stod(value1);
                    double to = std::stod(value2);
                    return std::to_string(from + static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / (to - from))));
                }
            }
            break;
        }
        case Block::OPERATOR_JOIN: {
            std::string value1 = getInputValue(block.inputs["STRING1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["STRING2"], &block, sprite);
            return value1 + value2;
        }
        case Block::OPERATOR_LETTER_OF: {
            std::string value1 = getInputValue(block.inputs["LETTER"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["STRING"], &block, sprite);
            if (isNumber(value1) && !value2.empty()) {
                int index = std::stoi(value1) - 1;
                if (index >= 0 && index < static_cast<int>(value2.size())) {
                    return std::string(1, value2[index]);
                } else {
                    std::cerr << "Index out of bounds for string: " << value2 << std::endl;
                    return "";
                }
            }
            break;
        }
        case Block::OPERATOR_LENGTH: {
            std::string value1 = getInputValue(block.inputs["STRING"], &block, sprite);
            if (!value1.empty()) {
                return std::to_string(value1.size());
            }
            break;
        }
        case Block::OPERATOR_MOD: {
            std::string value1 = getInputValue(block.inputs["NUM1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["NUM2"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                return std::to_string(std::fmod(std::stod(value1), std::stod(value2)));
            }
            break;
        }
        case Block::OPERATOR_ROUND: {
            std::string value1 = getInputValue(block.inputs["NUM"], &block, sprite);
            if (isNumber(value1)) {
                return std::to_string(std::round(std::stod(value1)));
            }
            break;
        }
        case Block::OPERATOR_MATHOP: {
            std::string inputValue = getInputValue(block.inputs["NUM"], &block, sprite);
            if (isNumber(inputValue)) {
                if (block.fields["OPERATOR"][0] == "abs") {
                    return std::to_string(abs(std::stod(inputValue)));
                }
                if (block.fields["OPERATOR"][0] == "floor") {
                    return std::to_string(floor(std::stod(inputValue)));
                }
                if (block.fields["OPERATOR"][0] == "ceiling") {
                    return std::to_string(ceil(std::stod(inputValue)));
                }
                if (block.fields["OPERATOR"][0] == "sqrt") {
                    return std::to_string(sqrt(std::stod(inputValue)));
                }
                if (block.fields["OPERATOR"][0] == "sin") {
                    return std::to_string(sin(std::stod(inputValue) * M_PI / 180.0));
                }
                if (block.fields["OPERATOR"][0] == "cos") {
                    return std::to_string(cos(std::stod(inputValue) * M_PI / 180.0));
                }
                if (block.fields["OPERATOR"][0] == "tan") {
                    return std::to_string(tan(std::stod(inputValue) * M_PI / 180.0));
                }
                if (block.fields["OPERATOR"][0] == "asin") {
                    return std::to_string(asin(std::stod(inputValue)) * 180.0 / M_PI);
                }
                if (block.fields["OPERATOR"][0] == "acos") {
                    return std::to_string(acos(std::stod(inputValue)) * 180.0 / M_PI);
                }
                if (block.fields["OPERATOR"][0] == "atan") {
                    return std::to_string(atan(std::stod(inputValue)) * 180.0 / M_PI);
                }
                if (block.fields["OPERATOR"][0] == "ln") {
                    return std::to_string(log(std::stod(inputValue)));
                }
                if (block.fields["OPERATOR"][0] == "log") {
                    return std::to_string(log10(std::stod(inputValue)));
                }
                if (block.fields["OPERATOR"][0] == "e ^") {
                    return std::to_string(exp(std::stod(inputValue)));
                }
                if (block.fields["OPERATOR"][0] == "10 ^") {
                    return std::to_string(pow(10, std::stod(inputValue)));
                }
            }
            break;
        }
        case Block::DATA_ITEMOFLIST: {
            std::string indexStr = getInputValue(block.inputs["INDEX"], &block, sprite);
            int index = std::stoi(indexStr) - 1;
            std::string listName = block.fields["LIST"][1];
            for (Sprite& currentSprite : sprites) {
                for (auto& [id, list] : currentSprite.lists) {
                    if (id == listName) {
                        if (index >= 0 && index < static_cast<int>(list.items.size())) {
                            return removeQuotations(list.items[index]);
                        } else {
                            std::cerr << "Index out of bounds for list: " << listName << std::endl;
                            return "";
                        }
                    }
                }
            }
            break;
        }
        case Block::DATA_ITEMNUMOFLIST: {
            std::string listName = block.fields["LIST"][1];
            std::string itemToFind = getInputValue(block.inputs["ITEM"], &block, sprite);
            for (Sprite& currentSprite : sprites) {
                for (auto& [id, list] : currentSprite.lists) {
                    if (id == listName) {
                        int index = 1;
                        for (auto& item : list.items) {
                            if (removeQuotations(item) == itemToFind) {
                                return std::to_string(index);
                            }
                            index++;
                        }
                    }
                }
            }
            return "0";
        }

        case Block::DATA_LENGTHOFLIST:{
   // std::cout << "Length of List! " << std::endl;
   std::string listName = block.fields["LIST"][1];
   // Search every sprite for the list
   for (Sprite& currentSprite : sprites) {
       for (auto& [id, list] : currentSprite.lists) {
           if (id == listName) {
               // Found the list
              // std::cout << "List size = " <<list.items.size()<< std::endl;
               return std::to_string(list.items.size());
           }
       }
   }
   return "";
}
    
        default:
            break;
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

std::vector<Block> getBlockChain(std::string blockId){
    std::vector<Block> blockChain;
    Block currentBlock = findBlock(blockId);
    while(currentBlock.id != "null"){
        blockChain.push_back(currentBlock);
        currentBlock = findBlock(currentBlock.next);
    }
    return blockChain;
}

bool runConditionalStatement(std::string blockId, Sprite* sprite) {
    Block block = findBlock(blockId);

    switch (block.opcode) {
        case Block::ARGUMENT_REPORTER_BOOLEAN: { // string from a custom block
            std::string value = findCustomValue(block.fields["VALUE"][0], sprite, block);
            return value == "1";
        }

        case Block::SENSING_KEYPRESSED: {
            Block inputBlock = findBlock(block.inputs["KEY_OPTION"][1]);
            for (std::string button : inputButtons) {
                if (inputBlock.fields["KEY_OPTION"][0] == button) {
                    return true;
                }
            }
            break;
        }

        case Block::OPERATOR_EQUALS: {
            std::string value1 = getInputValue(block.inputs["OPERAND1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["OPERAND2"], &block, sprite);
            return value1 == value2;
        }

        case Block::OPERATOR_GT: {
            std::string value1 = getInputValue(block.inputs["OPERAND1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["OPERAND2"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                return std::stod(value1) > std::stod(value2);
            }
            break;
        }

        case Block::OPERATOR_LT: {
            std::string value1 = getInputValue(block.inputs["OPERAND1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["OPERAND2"], &block, sprite);
            if (isNumber(value1) && isNumber(value2)) {
                return std::stod(value1) < std::stod(value2);
            }
            break;
        }

        case Block::OPERATOR_AND: {
            bool value1 = runConditionalStatement(block.inputs["OPERAND1"][1], sprite);
            bool value2 = runConditionalStatement(block.inputs["OPERAND2"][1], sprite);
            return value1 && value2;
        }

        case Block::OPERATOR_OR: {
            bool value1 = runConditionalStatement(block.inputs["OPERAND1"][1], sprite);
            bool value2 = runConditionalStatement(block.inputs["OPERAND2"][1], sprite);
            return value1 || value2;
        }

        case Block::OPERATOR_NOT: {
            bool value = runConditionalStatement(block.inputs["OPERAND"][1], sprite);
            return !value;
        }

        case Block::OPERATOR_CONTAINS: {
            std::string value1 = getInputValue(block.inputs["STRING1"], &block, sprite);
            std::string value2 = getInputValue(block.inputs["STRING2"], &block, sprite);
            return value1.find(value2) != std::string::npos;
        }

        default:
            break;
    }

    return false;
}

void runBroadcasts(){
    while(!broadcastQueue.empty()){
        std::string broadcastName = broadcastQueue.front();
       // std::cout<<"Broadcasting " << broadcastName << std::endl;
        for(Sprite &currentSprite : sprites){
                    for(auto &[id,block] : currentSprite.blocks){
                        if(block.opcode == block.EVENT_WHENBROADCASTRECEIVED && block.fields["BROADCAST_OPTION"][0] == broadcastName){
                           // std::cout<<"Running broadcast block " << block.id << std::endl;
                            runBlock(block,&currentSprite);
                        }
                    }
        }
        broadcastQueue.erase(broadcastQueue.begin());
    }
}

std::string findCustomValue(std::string valueName, Sprite* sprite, Block block) {
    for (auto& [custId, custBlock] : sprite->customBlocks) {
        // Find the index of valueName in argumentNames
        auto it = std::find(custBlock.argumentNames.begin(), custBlock.argumentNames.end(), valueName);
        
        if (it != custBlock.argumentNames.end()) {
            // Get the index of the found argument name
            size_t index = std::distance(custBlock.argumentNames.begin(), it);

            // Ensure index is within bounds of argumentIds
            if (index < custBlock.argumentIds.size()) {
                std::string argumentId = custBlock.argumentIds[index];

                // Find the value in argumentValues using argumentId
                auto valueIt = custBlock.argumentValues.find(argumentId);
                if (valueIt != custBlock.argumentValues.end()) {
                   // std::cout << "FOUND that shit BAAANG: " << valueIt->second << std::endl;
                    return valueIt->second;
                } else {
                   // std::cout << "Argument ID found, but no value exists for it." << std::endl;
                }
            } else {
              //  std::cout << "Index out of bounds for argumentIds!" << std::endl;
            }
        }
    }
    return "";
}


void runCustomBlock(Sprite*sprite,Block block){
    for(auto &[id,data] : sprite->customBlocks){
        if(id == block.mutation["proccode"]){
           // std::cout<<"burrrrp"<<std::endl;
            for(std::string arg : data.argumentIds){
                if(!block.inputs[arg].is_null()){
                    data.argumentValues[arg] = getInputValue(block.inputs[arg],&block,sprite);
                    //std::cout<<"yes! " << data.argumentValues[arg] <<std::endl;
                }
            }
           // std::cout<<data.blockId<<std::endl;
            // run the parent of the prototype block since that block is the definition, containing all the blocks
            
            runBlock(findBlock(findBlock(data.blockId).parent),sprite,conditionals[block.id].waitingBlock,data.runWithoutScreenRefresh);
        }
    }

}

void runBlock(Block block, Sprite* sprite, Block waitingBlock, bool withoutScreenRefresh) {
    auto start = std::chrono::high_resolution_clock::now();

    switch (block.opcode) {
        case block.PROCEDURES_CALL: {
            if (conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = -1;
                newConditional.waitingBlock = findBlock(block.next);
                conditionals[newConditional.id] = newConditional;
            }
            waitingBlock = findBlock(block.next);
            runCustomBlock(sprite, block);
            return;
        }
        case block.PROCEDURES_DEFINITION:{
            goto nextBlock;
        }
        case block.CONTROL_START_AS_CLONE:{
            goto nextBlock;
        }
        case block.EVENT_WHENFLAGCLICKED:{
            goto nextBlock;
        }

        case block.MOTION_GOTOXY: {
            std::string xVal = getInputValue(block.inputs["X"], &block, sprite);
            std::string yVal = getInputValue(block.inputs["Y"], &block, sprite);
            if (isNumber(xVal)) sprite->xPosition = std::stod(xVal);
            else std::cerr << "Set X Position invalid with pos " << xVal << std::endl;
            if (isNumber(yVal)) sprite->yPosition = std::stod(yVal);
            else std::cerr << "Set Y Position invalid with pos " << yVal << std::endl;
            goto nextBlock;
        }

        case block.MOTION_POINTINDIRECTION: {
            std::string value = getInputValue(block.inputs["DIRECTION"], &block, sprite);
            if (isNumber(value)) {
                sprite->rotation = std::stoi(value);
            } else {
                std::cerr << "Invalid Turn direction " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_TURNRIGHT: {
            std::string value = getInputValue(block.inputs["DEGREES"], &block, sprite);
            if (isNumber(value)) {
                sprite->rotation += std::stoi(value);
            } else {
                std::cerr << "Invalid Turn direction " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_TURNLEFT: {
            std::string value = getInputValue(block.inputs["DEGREES"], &block, sprite);
            if (isNumber(value)) {
                sprite->rotation -= std::stoi(value);
            } else {
                std::cerr << "Invalid Turn direction " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_CHANGEXBY: {
            std::string value = getInputValue(block.inputs["DX"], &block, sprite);
            if (isNumber(value)) {
                sprite->xPosition += std::stod(value);
            } else {
                std::cerr << "Invalid X position " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_CHANGEYBY: {
            std::string value = getInputValue(block.inputs["DY"], &block, sprite);
            if (isNumber(value)) {
                sprite->yPosition += std::stod(value);
            } else {
                std::cerr << "Invalid Y position " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_SETX: {
            std::string value = getInputValue(block.inputs["X"], &block, sprite);
            if (isNumber(value)) {
                sprite->xPosition = std::stod(value);
            } else {
                std::cerr << "Invalid X position " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_SETY: {
            std::string value = getInputValue(block.inputs["Y"], &block, sprite);
            if (isNumber(value)) {
                sprite->yPosition = std::stod(value);
            } else {
                std::cerr << "Invalid Y position " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.EVENT_BROADCAST: {
            broadcastQueue.push_back(getInputValue(block.inputs["BROADCAST_INPUT"], &block, sprite));
            goto nextBlock;
        }

        case block.CONTROL_IF: {
            if (block.inputs["CONDITION"][1].is_null()) {
                goto nextBlock;
            }
            if (runConditionalStatement(block.inputs["CONDITION"][1], sprite)) {
                if (!block.inputs["SUBSTACK"][1].is_null()) {
                    Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                    runBlock(subBlock, sprite);
                }
            }
            goto nextBlock;
        }

        case block.CONTROL_IF_ELSE: {
            if (block.inputs["CONDITION"][1].is_null()) {
                goto nextBlock;
            }
            if (runConditionalStatement(block.inputs["CONDITION"][1], sprite)) {
                if (!block.inputs["SUBSTACK"][1].is_null()) {
                    Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                    runBlock(subBlock, sprite);
                }
            } else {
                if (!block.inputs["SUBSTACK2"][1].is_null()) {
                    Block subBlock = findBlock(block.inputs["SUBSTACK2"][1]);
                    runBlock(subBlock, sprite);
                }
            }
            goto nextBlock;
        }

        case block.CONTROL_REPEAT_UNTIL: {
            if (conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = -1;
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                conditionals[newConditional.id] = newConditional;
            }
            if (block.inputs["CONDITION"][1].is_null() || !runConditionalStatement(block.inputs["CONDITION"][1], sprite)) {
                conditionals[block.id].isTrue = true;
            } else {
                conditionals[block.id].isTrue = false;
                waitingBlock = conditionals[block.id].waitingBlock;
            }
            if (conditionals[block.id].isTrue && !block.inputs["SUBSTACK"][1].is_null()) {
                Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                runBlock(subBlock, sprite);
                return;
            }
            if (conditionals[block.id].isTrue && block.inputs["SUBSTACK"][1].is_null()) {
                return;
            }
            goto nextBlock;
        }

        case block.CONTROL_REPEAT: {
            if (conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = std::stoi(getInputValue(block.inputs["TIMES"], &block, sprite));
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                conditionals[newConditional.id] = newConditional;
            }
            if (conditionals[block.id].isTrue && !block.inputs["SUBSTACK"][1].is_null()) {
                Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                runBlock(subBlock, sprite);
            }
            if (conditionals[block.id].times > 0) {
                conditionals[block.id].isTrue = true;
                conditionals[block.id].times--;
                return;
            } else {
                conditionals[block.id].isTrue = false;
                waitingBlock = conditionals[block.id].waitingBlock;
            }
            goto nextBlock;
        }

        case block.CONTROL_FOREVER: {
            if (conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = -1;
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                conditionals[newConditional.id] = newConditional;
            }
            if (conditionals[block.id].isTrue && !block.inputs["SUBSTACK"][1].is_null()) {
                Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                runBlock(subBlock, sprite);
            }
            conditionals[block.id].isTrue = true;
            goto nextBlock;
        }

        case block.CONTROL_CREATE_CLONE_OF: {
            Block cloneOptions = findBlock(block.inputs["CLONE_OPTION"][1]);
            Sprite spriteToClone;
            if (cloneOptions.fields["CLONE_OPTION"][0] == "_myself_") {
                spriteToClone = *sprite;
            } else {
                for (Sprite& currentSprite : sprites) {
                    if (currentSprite.name == removeQuotations(cloneOptions.fields["CLONE_OPTION"][0]) && currentSprite.isClone == sprite->isClone) {
                        spriteToClone = currentSprite;
                    }
                }
            }
            if (!spriteToClone.name.empty()) {
                for (auto& [id, conditional] : spriteToClone.conditionals) {
                    conditional.hostSprite = &spriteToClone;
                }
                spriteToClone.isClone = true;
                spriteToClone.isStage = false;
                spriteToClone.id = generateRandomString(15);
                std::unordered_map<std::string, Block> newBlocks;
                for (auto& [id, block] : spriteToClone.blocks) {
                    if (block.opcode == block.CONTROL_START_AS_CLONE || block.opcode == block.EVENT_WHENBROADCASTRECEIVED) {
                        std::vector<Block> blockChain = getBlockChain(block.id);
                        for (const Block& block : blockChain) {
                            newBlocks[block.id] = block;
                        }
                    }
                }
                spriteToClone.blocks.clear();
                spriteToClone.blocks = newBlocks;
                sprites.push_back(spriteToClone);
                Sprite* addedSprite = &sprites.back();
                for (Sprite& currentSprite : sprites) {
                    if (&currentSprite == addedSprite) {
                        for (auto& [id, block] : currentSprite.blocks) {
                            if (block.opcode == block.CONTROL_START_AS_CLONE) {
                                runBlock(block, &currentSprite);
                            }
                        }
                    }
                }
            }
            goto nextBlock;
        }

        case block.CONTROL_DELETE_THIS_CLONE: {
            sprites.erase(std::remove_if(sprites.begin(), sprites.end(), [&](const Sprite& s) { return s.id == sprite->id && s.isClone; }), sprites.end());
            return;
        }

        case block.DATA_SETVARIABLETO: {
            std::string val = getInputValue(block.inputs["VALUE"], &block, sprite);
            std::string varId = block.fields["VARIABLE"][1];
            setVariableValue(varId, val, sprite, false);
            goto nextBlock;
        }

        case block.DATA_CHANGEVARIABLEBY: {
            std::string val = getInputValue(block.inputs["VALUE"], &block, sprite);
            std::string varId = block.fields["VARIABLE"][1];
            setVariableValue(varId, val, sprite, true);
            goto nextBlock;
        }

        case block.SENSING_RESETTIMER: {
            timer = 0;
            goto nextBlock;
        }
        default:
        std::cerr << "Unhandled opcode: " << block.opcode  << "???????????"<< std::endl;
        goto nextBlock;
    }

nextBlock:
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    if (duration.count() > 10) {
        std::cout << "\x1b[14;0H" << block.opcode << " took " << duration.count() << " milliseconds!";
    }
    if (!block.next.empty()) {
        Block nextBlock = findBlock(block.next);
        if (nextBlock.id != "null") {
            runBlock(nextBlock, sprite, waitingBlock, withoutScreenRefresh);
        }
    } else {
        runBroadcasts();
        if (!waitingBlock.id.empty()) {
            runBlock(waitingBlock, sprite, Block(), withoutScreenRefresh);
        }
    }
}

void runRepeatBlocks(){
    //std::cout<<"Running repeat blocks..."<< std::endl;
    for(const auto &[id,data] : conditionals){
        if(data.isTrue){
            if(data.runWithoutScreenRefresh){
                while(data.isTrue){
                    runBlock(findBlock(data.blockId),data.hostSprite);
                }
            }
            else{
            runBlock(findBlock(data.blockId),data.hostSprite);
            }
            
        }
    }
}

void setVariableValue(std::string variableId,std::string value,Sprite* sprite,bool isChangingBy){
    if(sprite->variables.find(variableId) != sprite->variables.end()){ // if not a global Variable
        Variable& var = sprite->variables[variableId];


        if(!isChangingBy){
            if(isNumber(value)){
            double val = std::stod(value); // to make it consistent with changing variables
            var.value = std::to_string(val);}
            else{
                var.value = value;
            }
        }
        else{
            if(isNumber(var.value) && isNumber(value)){
            var.value = std::to_string(std::stod(var.value) + std::stod(value));
            }
            else{
                if(!isNumber(value)) return;
                if(isNumber(value)){
                    double val = std::stod(value); // to make it consistent with changing variables
                    var.value = std::to_string(val);}
                    else{
                        var.value = value;
                    }
            }
        }

        //std::cout<<"Local Variable set. " << sprite->variables[variableId].value << std::endl;

    }
    // global Variable (TODO fix redundant code later :grin:)
    else{
        for(Sprite &currentSprite : sprites){
            if (currentSprite.isStage){
                Variable& var = currentSprite.variables[variableId];
        
                if(!isChangingBy){
                    if(isNumber(value)){
                        double val = std::stod(value); // to make it consistent with changing variables
                        var.value = std::to_string(val);}
                        else{
                            var.value = value;
                        }
                }
                else{
                    if(isNumber(var.value) && isNumber(value)){
                    var.value = std::to_string(std::stod(var.value) + std::stod(value));
                    }
                    else{
                        if(!isNumber(value)) return;
                        if(isNumber(value)){
                            double val = std::stod(value); // to make it consistent with changing variables
                            var.value = std::to_string(val);}
                            else{
                                var.value = value;
                            }
                    }
                }
        
               // std::cout<<"Global Variable set to " << var.value << std::endl;
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
      //  std::cout<<"Checking list " << variableId << std::endl;
        for(const auto &[id,data] : currentSprite.lists){
            if(id == variableId){
                std::string finalValue;
                for(const auto &listItem : data.items){
               //     std::cout<<"Found one " << std::endl;
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
    // 2 SEEMS to be a boolean
    if(item[0] == 2){
        return std::to_string(runConditionalStatement(findBlock(item[1]).id, sprite));
    }
    return "0";
}




std::vector<Sprite*> findSprite(std::string spriteName){
    std::vector<Sprite*> sprts;
    for(Sprite currentSprite : sprites){
        if(currentSprite.name == spriteName){
            sprts.push_back(&currentSprite);
        }
    }
    return sprts;
}



void runAllBlocksByOpcode(Block::opCode opcodeToFind){
    //std::cout << "Running all " << opcodeToFind << " blocks." << "\n";
    for(Sprite &currentSprite : sprites){
        for(auto &[id,data] : currentSprite.blocks){
            if(data.opcode == opcodeToFind){
                runBlock(data,&currentSprite);
            }
        }
    }
}