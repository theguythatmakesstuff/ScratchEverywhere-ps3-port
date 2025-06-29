#include "interpret.hpp"

std::vector<Sprite*> sprites;
std::vector<Sprite> spritePool;
std::vector<std::string> broadcastQueue;
//std::unordered_map<std::string,Conditional> conditionals;
std::unordered_map<std::string, Block*> blockLookup;
Mouse mousePointer;
std::string answer;
double timer = 0;
bool toExit = false;
ProjectType projectType;

BlockExecutor executor;

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

    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, chars.size() - 1);

    for (int i = 0; i < length; i++) {
        result += chars[distribution(generator)];
    }

    return result;


}

std::string removeQuotations(std::string value) {
    value.erase(std::remove_if(value.begin(),value.end(),[](char c){return c == '"';}),value.end());
    return value;
}

void initializeSpritePool(int poolSize) {
    for (int i = 0; i < poolSize; i++) {
        Sprite newSprite;
        newSprite.id = generateRandomString(15);
        newSprite.isClone = true;
        newSprite.toDelete = true;
        spritePool.push_back(newSprite);
    }
}

Sprite* getAvailableSprite() {
    for (Sprite& sprite : spritePool) {
        if (sprite.toDelete) {
            sprite.toDelete = false;  // Reactivate sprite
            return &sprite;
        }
    }
    return nullptr;  // No available sprites
}

void cleanupSprites() {
    for (Sprite* sprite : sprites) {
        delete sprite;
    }

    spritePool.clear();
    sprites.clear();
}

std::vector<std::pair<double, double>> getCollisionPoints(Sprite* currentSprite) {
    std::vector<std::pair<double, double>> collisionPoints;

    // Get sprite dimensions, scaled by size
    double halfWidth = (currentSprite->spriteWidth * currentSprite->size / 100.0) / 2.0;
    double halfHeight = (currentSprite->spriteHeight * currentSprite->size / 100.0) / 2.0;

    // Calculate rotation in radians
    double rotationRadians = (currentSprite->rotation - 90) * M_PI / 180.0;

    // Define the four corners relative to the sprite's center
    std::vector<std::pair<double, double>> corners = {
        { -halfWidth, -halfHeight }, // Top-left
        { halfWidth, -halfHeight },  // Top-right
        { halfWidth, halfHeight },   // Bottom-right
        { -halfWidth, halfHeight }   // Bottom-left
    };

    // Rotate and translate each corner
    for (const auto& corner : corners) {
        double rotatedX = corner.first * cos(rotationRadians) - corner.second * sin(rotationRadians);
        double rotatedY = corner.first * sin(rotationRadians) + corner.second * cos(rotationRadians);

        collisionPoints.emplace_back(
            currentSprite->xPosition + rotatedX,
            currentSprite->yPosition + rotatedY
        );
    }

    return collisionPoints;
}

void loadSprites(const nlohmann::json& json){
    std::cout<<"Beginning to load sprites..."<< std::endl;
    sprites.reserve(400);
    int count = 0;
    for (const auto& target : json["targets"]){ // "target" is sprite in Scratch speak, so for every sprite in sprites
    
        Sprite* newSprite = new Sprite();
        if(target.contains("name")){
        newSprite->name = target["name"].get<std::string>();}
        newSprite->id = generateRandomString(15);
        if(target.contains("isStage")){
        newSprite->isStage = target["isStage"].get<bool>();}
        if(target.contains("draggable")){
        newSprite->draggable = target["draggable"].get<bool>();}
        if(target.contains("visible")){
        newSprite->visible = target["visible"].get<bool>();}
        else newSprite->visible = true;
        if(target.contains("currentCostume")){
        newSprite->currentCostume = target["currentCostume"].get<int>();}
        if(target.contains("volume")){
        newSprite->volume = target["volume"].get<int>();}
        if(target.contains("x")){
        newSprite->xPosition = target["x"].get<int>();}
        if(target.contains("y")){
        newSprite->yPosition = target["y"].get<int>();}
        if(target.contains("size")){
        newSprite->size = target["size"].get<int>();}
        else newSprite->size = 100;
        if(target.contains("direction")){
        newSprite->rotation = target["direction"].get<int>();}
        else newSprite->rotation = 90;
        if(target.contains("layerOrder")){
        newSprite->layer = target["layerOrder"].get<int>();}
        else newSprite->layer = 0;
        if(target.contains("rotationStyle")){
        newSprite->rotationStyle = target["rotationStyle"].get<std::string>();}
        newSprite->toDelete = false;
        newSprite->isClone = false;
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
            newSprite->variables[newVariable.id] = newVariable; // add variable to sprite
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
            else newBlock.parent = "null";
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
            newSprite->blocks[newBlock.id] = newBlock; // add block


            
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

                newSprite->customBlocks[newCustomBlock.name] = newCustomBlock; // add custom block
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
            newSprite->lists[newList.id] = newList; // add list
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
            newSprite->sounds[newSound.id] = newSound;
        }

        // set Costumes
        for(const auto &[id,data] : target["costumes"].items()){
            Costume newCostume;
            newCostume.id = data["assetId"];
            if(data.contains("name")){
            newCostume.name = data["name"];}
            if(data.contains("bitmapResolution")){
            newCostume.bitmapResolution = data["bitmapResolution"];}
            if(data.contains("dataFormat")){
            newCostume.dataFormat = data["dataFormat"];}
            if(data.contains("md5ext")){
            newCostume.fullName = data["md5ext"];}
            if(data.contains("rotationCenterX")){
            newCostume.rotationCenterX = data["rotationCenterX"];}
            if(data.contains("rotationCenterY")){
            newCostume.rotationCenterY = data["rotationCenterY"];}
            newSprite->costumes.push_back(newCostume);
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
            newSprite->comments[newComment.id] = newComment;
        }

        // set Broadcasts
        for(const auto &[id,data] : target["broadcasts"].items()){
            Broadcast newBroadcast;
            newBroadcast.id = id;
            newBroadcast.name = data;
            newSprite->broadcasts[newBroadcast.id] = newBroadcast;
           // std::cout<<"broadcast name = "<< newBroadcast.name << std::endl;
        }

        sprites.push_back(newSprite);
        count++;


    }

    // load block lookup table
    blockLookup.clear();
    for (Sprite* sprite : sprites) {
        for (auto& [id, block] : sprite->blocks) {
            blockLookup[id] = &block;
        }
    }
    // setup top level blocks
    for (Sprite* currentSprite : sprites) {
    for(auto& [id,block]: currentSprite->blocks){
        if(block.topLevel) continue; // skip top level blocks
        block.topLevelParentBlock = getBlockParent(&block)->id; // get parent block id
        //std::cout<<"block id = "<< block.topLevelParentBlock << std::endl;
    }
}

    // try to find the advanced project settings comment
    nlohmann::json config;
    for (Sprite* currentSprite : sprites) {
        if(!currentSprite->isStage) continue;
        for(auto& [id,comment] : currentSprite->comments){
        std::size_t json_start = comment.text.find('{');
        if (json_start == std::string::npos) continue;

        std::string json_str = comment.text.substr(json_start);
        std::size_t json_end = json_str.find("}");
        if (json_end != std::string::npos) {
            json_str = json_str.substr(0, json_end + 1);}
        else continue;

        try {
            config = nlohmann::json::parse(json_str);
            std::cout << "Parsed JSON:\n" << config.dump(4) << "\n";
            break;
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "Failed to parse JSON: " << e.what() << "\n";
        continue;
    }
        }
    }
    // set advanced project settings properties
    try{
       int framerate = config["framerate"].get<int>();
       FPS = framerate;
    }
    catch(...){
        std::cerr << "no framerate property." << std::endl;
    }
        try{
       int wdth = config["width"].get<int>();
       projectWidth = wdth;
    }
    catch(...){
        std::cerr << "no width property." << std::endl;
    }
        try{
       int hght = config["height"].get<int>();
       projectHeight = hght;
    }
    catch(...){
        std::cerr << "no height property." << std::endl;
    }
    
    // if unzipped, load initial sprites
    if(projectType == UNZIPPED){
        for(auto& currentSprite : sprites){
            loadImageFromFile(currentSprite->costumes[currentSprite->currentCostume].id);
        }
    }


    initializeSpritePool(300);

    // add nextBlock during load time so it doesn't have to do it at runtime
        // for(auto& sprite : sprites){
        //     for(auto& [id,block] : sprite.blocks){
        //         block.nextBlock = blockLookup[block.next];
        //     }
        // }
    

    //buildBlockHierarchyCache();

    // get block chains for every block
    for (Sprite* currentSprite : sprites) {
    for(auto& [id,block]: currentSprite->blocks){
        if(!block.topLevel) continue;
        std::string outID;
        BlockChain chain;
        chain.blockChain = getBlockChain(block.id,&outID);
        currentSprite->blockChains[outID] = chain;
        std::cout << "ok = " << outID << std::endl;
        block.blockChainID = outID;

        for(auto& chainBlock : chain.blockChain) {
            if(currentSprite->blocks.find(chainBlock->id) != currentSprite->blocks.end()) {
                currentSprite->blocks[chainBlock->id].blockChainID = outID;
            }
        }

    }
}

    std::cout<<"Loaded " << sprites.size() << " sprites."<< std::endl;
}



Block* findBlock(std::string blockId){


    auto block = blockLookup.find(blockId);
    if(block != blockLookup.end()) {
        return block->second;
    }


    return nullptr;
}

std::vector<Block*> getBlockChain(std::string blockId,std::string* outID){
    std::vector<Block*> blockChain;
    Block* currentBlock = findBlock(blockId);
    while(currentBlock != nullptr){
        blockChain.push_back(currentBlock);
        if(outID)
        *outID += currentBlock->id;
        if(!currentBlock->inputs["SUBSTACK"][1].is_null()){
            std::vector<Block*> subBlockChain;
            subBlockChain = getBlockChain(currentBlock->inputs["SUBSTACK"][1],outID);
            for(auto& block : subBlockChain){
                blockChain.push_back(block);
                if(outID)
                *outID += block->id;
            }
        }
        if(!currentBlock->inputs["SUBSTACK2"][1].is_null()){
            std::vector<Block*> subBlockChain;
            subBlockChain = getBlockChain(currentBlock->inputs["SUBSTACK2"][1],outID);
            for(auto& block : subBlockChain){
                blockChain.push_back(block);
                if(outID)
                *outID += block->id;
            }
        }
        currentBlock = findBlock(currentBlock->next);
    }
    return blockChain;
}

Block* getBlockParent(const Block* block){
    Block* parentBlock;
    const Block* currentBlock = block;
    while(currentBlock->parent != "null"){
        parentBlock = findBlock(currentBlock->parent);
        if(parentBlock != nullptr){
            currentBlock = parentBlock;
        }
        else{
            break;
        }
    }
    return const_cast<Block*>(currentBlock);
}


void runBroadcasts() {
    // Process broadcasts one at a time until the queue is empty
    if (broadcastQueue.empty()) {
        return;
    }
    
    // Take the first broadcast from the queue
    std::string currentBroadcast = broadcastQueue.front();
    broadcastQueue.erase(broadcastQueue.begin());
    
    // Keep a copy of the sprites to avoid iterator invalidation
    std::vector<std::pair<Block*, Sprite*>> blocksToRun;
    
    // Identify all blocks that should respond to this broadcast
    for (auto* currentSprite : sprites) {
        for (auto& [id, block] : currentSprite->blocks) {
            if (block.opcode == block.EVENT_WHENBROADCASTRECEIVED && 
                block.fields["BROADCAST_OPTION"][0] == currentBroadcast) {
                blocksToRun.push_back({&block, currentSprite});
            }
        }
    }
    
    // Now run all the identified blocks
    for (auto& [blockPtr, spritePtr] : blocksToRun) {
        //std::cout << "Running broadcast block " << blockPtr->id << std::endl;
        executor.runBlock(*blockPtr, spritePtr);
    }
    
    // Check if new broadcasts were added during execution
    if (!broadcastQueue.empty()) {
        runBroadcasts(); // Process the next broadcast
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

void runCustomBlock(Sprite* sprite, const Block& block, Block* callerBlock){
    for(auto &[id, data] : sprite->customBlocks){
        if(id == block.mutation.at("proccode").get<std::string>()){
            // Set up argument values
            for(std::string arg : data.argumentIds){
                if(!block.inputs.at(arg).is_null()){
                    data.argumentValues[arg] = Scratch::getInputValue(block.inputs.at(arg), &block, sprite);
                }
            }
            
            std::cout << "running custom block " << data.blockId << std::endl;
            
            // Get the parent of the prototype block (the definition containing all blocks)
            Block* customBlockDefinition = findBlock(findBlock(data.blockId)->parent);
            callerBlock->customBlockPtr = customBlockDefinition;
            
            std::cout << "RWSR = " << data.runWithoutScreenRefresh << std::endl;
            
            // Execute the custom block definition
            executor.runBlock(*customBlockDefinition, sprite, nullptr, data.runWithoutScreenRefresh);
            
            break;
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

        //std::cout<<"Local Variable set. "  << sprite->variables[variableId].name  << " = "<< sprite->variables[variableId].value << std::endl;

    }
    // global Variable (TODO fix redundant code later :grin:)
    else{
        for(Sprite *currentSprite : sprites){
            if (currentSprite->isStage){
                Variable& var = currentSprite->variables[variableId];
        
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
        
              // std::cout<<"Global Variable set. "  << var.name << " = "<< var.value << std::endl;
            }
        }
    }

}

std::string getVariableValue(std::string variableId,Sprite*sprite){

        for(const auto &[id,data] : sprite->variables){
            if (id == variableId) {

                // check if value is a whole number
                if(isNumber(data.value)){
                double doubleValue = std::stod(data.value);
                if (std::floor(doubleValue) == doubleValue) {
                    return std::to_string(static_cast<int>(doubleValue));
            }
        }

                return data.value; 
            }
        }
        // check if it's a list instead
      //  std::cout<<"Checking list " << variableId << std::endl;
        for(const auto &[id,data] : sprite->lists){
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
        //check globally (blahblah redundant code TODO fix)
        for(const auto &currentSprite : sprites){
            if(currentSprite->isStage){
                for(const auto &[id,data] : currentSprite->variables){
                    if (id == variableId) {

                // check if value is a whole number
                if(isNumber(data.value)){
                double doubleValue = std::stod(data.value);
                if (std::floor(doubleValue) == doubleValue) {
                    return std::to_string(static_cast<int>(doubleValue));
            }
        }

                        return data.value; // Assuming `Variable` has a `value` field
                    }
                }
                // check if it's a list instead
              //  std::cout<<"Checking list " << variableId << std::endl;
                for(const auto &[id,data] : currentSprite->lists){
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
        }
    
    return "";
}

std::string Scratch::getInputValue(const nlohmann::json& item, const Block* block, Sprite* sprite) {
    int type = item[0];
    auto& data = item[1];

    // 1 is just a plain number
    if (type == 1) {
        return data[1].get<std::string>(); // Convert JSON to string
    }
    // 3 is if there is a variable of some kind inside
    if (type == 3) {
        if (data.is_array()) {
           return getVariableValue(data[2],sprite);
        } else {
           return executor.getBlockValue(*findBlock(data), sprite);
        }
    }
    // 2 SEEMS to be a boolean
    if(type == 2){
        return std::to_string(executor.runConditionalBlock(findBlock(data)->id, sprite));
    }
    return "0";
}




std::vector<Sprite*> findSprite(std::string spriteName){
    std::vector<Sprite*> sprts;
    for(Sprite* currentSprite : sprites){
        if(currentSprite->name == spriteName){
            sprts.push_back(currentSprite);
        }
    }
    return sprts;
}



void runAllBlocksByOpcode(Block::opCode opcodeToFind){
    //std::cout << "Running all " << opcodeToFind << " blocks." << "\n";
    for(Sprite *currentSprite : sprites){
        for(auto &[id,data] : currentSprite->blocks){
            if(data.opcode == opcodeToFind){
                //runBlock(data,currentSprite);
                executor.runBlock(data,currentSprite);
            }
        }
    }
}