#include "interpret.hpp"

std::vector<Sprite*> sprites;
std::vector<Sprite> spritePool;
std::vector<std::string> broadcastQueue;
std::unordered_map<std::string,Conditional> conditionals;
std::unordered_map<std::string, Block*> blockLookup;
Mouse mousePointer;
double timer = 0;
bool toExit = false;


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

    // Get sprite dimensions
    double halfWidth = (currentSprite->spriteWidth * currentSprite->size / 100.0) / 2.0;
    double halfHeight = (currentSprite->spriteHeight * currentSprite->size / 100.0) / 2.0;

    // Calculate rotation in radians
    double rotationRadians = -(currentSprite->rotation - 90) * M_PI / 180.0;

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
        block.topLevelParentBlock = getBlockParent(block).id; // get parent block id
        //std::cout<<"block id = "<< block.topLevelParentBlock << std::endl;
    }
}

    initializeSpritePool(100);

    // add nextBlock during load time so it doesn't have to do it at runtime
        // for(auto& sprite : sprites){
        //     for(auto& [id,block] : sprite.blocks){
        //         block.nextBlock = blockLookup[block.next];
        //     }
        // }
    



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
        case Block::LOOKS_COSTUME: {
           return block.fields["COSTUME"][0];
        }
        case Block::LOOKS_BACKDROPS:{
            return block.fields["BACKDROP"][0];
        }
        case Block::LOOKS_COSTUMENUMBERNAME:{
            std::string value = block.fields["NUMBER_NAME"][0];
            if(value == "name"){
                return sprite->costumes[sprite->currentCostume].name;
            }
            else if(value == "number"){
                return std::to_string(sprite->currentCostume + 1);
            }
        }
        case Block::LOOKS_BACKDROPNUMBERNAME:{
            std::string value = block.fields["NUMBER_NAME"][0];
            if(value == "name"){
                for(Sprite* currentSprite : sprites){
                    if(currentSprite->isStage){
                        return currentSprite->costumes[currentSprite->currentCostume].name;
                    }
                }
            }
            else if(value == "number"){
                for(Sprite* currentSprite : sprites){
                    if(currentSprite->isStage){
                        return std::to_string(currentSprite->currentCostume + 1);
                    }
                }
            }
        }
        case Block::SOUND_VOLUME: {
            return std::to_string(sprite->volume);
        }
        case Block::SENSING_TIMER: {
            return std::to_string(timer);
        }

        case block.SENSING_OF:{
            std::string value = block.fields["PROPERTY"][0];
            std::string object;
            try{
            object = findBlock(block.inputs["OBJECT"][1]).fields["OBJECT"][0];}
            catch(...){
               // std::cerr << "Error: Unable to find object for SENSING_OF block." << std::endl;
                return "0";
            }
            Sprite* spriteObject = nullptr;

            for(Sprite* currentSprite : sprites){
                if(currentSprite->name == object && !currentSprite->isClone){
                    spriteObject = currentSprite;
                    break;
                }
            }

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
            for(const auto& [id, variable] : spriteObject->variables) {
               // std::cout << "Variable ID: " << variable.name << ", Variable Name: " << variable.name << std::endl;
                if (value == variable.name) {
                    return variable.value;
                }
            }
        }

        case Block::SENSING_MOUSEX:{
            return std::to_string(mousePointer.x);
        }
        case Block::SENSING_MOUSEY:{
            return std::to_string(mousePointer.y);
        }

        case Block::SENSING_DISTANCETO:{
            Block inputBlock = findBlock(block.inputs["DISTANCETOMENU"][1]);
            std::string object = inputBlock.fields["DISTANCETOMENU"][0];
           // std::cout << "Object: " << object << std::endl;
            Sprite* spriteObject = nullptr;

            if(object == "_mouse_"){
                return std::to_string(sqrt(pow(mousePointer.x - sprite->xPosition,2) + pow(mousePointer.y - sprite->yPosition,2)));
            }

            for(Sprite* currentSprite : sprites){
                if(currentSprite->name == object && !currentSprite->isClone){
                    spriteObject = currentSprite;
                    break;
                }
            }
            if(spriteObject != nullptr){
                double distance = sqrt(pow(spriteObject->xPosition - sprite->xPosition,2) + pow(spriteObject->yPosition - sprite->yPosition,2));
                return std::to_string(distance);
            }
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
                   // std::cerr << "Index out of bounds for string: " << value2 << std::endl;
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
            for (Sprite* currentSprite : sprites) {
                for (auto& [id, list] : currentSprite->lists) {
                    if (id == listName) {
                        if (index >= 0 && index < static_cast<int>(list.items.size())) {
                            return removeQuotations(list.items[index]);
                        } else {
                           // std::cerr << "Index out of bounds for list: " << listName << std::endl;
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
            for (Sprite* currentSprite : sprites) {
                for (auto& [id, list] : currentSprite->lists) {
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
   for (Sprite* currentSprite : sprites) {
       for (auto& [id, list] : currentSprite->lists) {
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


    auto block = blockLookup.find(blockId);
    if(block != blockLookup.end()) {
        return *block->second;
    }

    static Block null;
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

Block getBlockParent(Block block){
    Block parentBlock;
    Block currentBlock = block;
    while(currentBlock.parent != "null"){
        parentBlock = findBlock(currentBlock.parent);
        if(parentBlock.id != "null"){
            currentBlock = parentBlock;
        }
        else{
            break;
        }
    }
    return currentBlock;
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

        case Block::SENSING_TOUCHINGOBJECT: {
            Block inputBlock = findBlock(block.inputs["TOUCHINGOBJECTMENU"][1]);
            std::string objectName;
            try {
                objectName = inputBlock.fields["TOUCHINGOBJECTMENU"][0];
            } catch (...) {
               // std::cerr << "Error: Unable to find object for SENSING_TOUCHINGOBJECT block." << std::endl;
                return false;
            }

           // std::cout << "Object name = " << objectName << std::endl;

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

            if(objectName == "_edge_"){
                // Check if the current sprite is touching the edge of the screen
                if (sprite->xPosition <= -240 || sprite->xPosition >= 240 ||
                    sprite->yPosition <= -160 || sprite->yPosition >= 160) {
                    return true;
                }
                return false;
            }

            for (Sprite* targetSprite : sprites) {
                if (targetSprite->name == objectName) {
                    //std::cout << "Found object: " << targetSprite->name << std::endl;

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
            break;
        }

        case Block::SENSING_MOUSEDOWN:{
            return mousePointer.isPressed;
        }

        case Block::DATA_LIST_CONTAINS_ITEM:{
            std::string listName = block.fields["LIST"][1];
            std::string itemToFind = getInputValue(block.inputs["ITEM"], &block, sprite);
            for (Sprite* currentSprite : sprites) {
                for (auto& [id, list] : currentSprite->lists) {
                    if (id == listName) {
                        for (const auto& item : list.items) {
                            if (removeQuotations(item) == itemToFind) {
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
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
        runBlock(*blockPtr, spritePtr);
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
    while(block.id != "null"){

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
           // std::cout << "GOTOXY!" << std::endl;
            std::string xVal = getInputValue(block.inputs["X"], &block, sprite);
            std::string yVal = getInputValue(block.inputs["Y"], &block, sprite);
            if (isNumber(xVal)) sprite->xPosition = std::stod(xVal);
            //else std::cerr << "Set X Position invalid with pos " << xVal << std::endl;
            if (isNumber(yVal)) sprite->yPosition = std::stod(yVal);
           // else std::cerr << "Set Y Position invalid with pos " << yVal << std::endl;
            goto nextBlock;
        }

        case block.MOTION_GOTO:{
            Block inputBlock = findBlock(block.inputs["TO"][1]);
            std::string objectName = inputBlock.fields["TO"][0];

            if (objectName == "_random_") {
                sprite->xPosition = rand() % projectWidth - projectWidth / 2;
                sprite->yPosition = rand() % projectHeight - projectHeight / 2;
                goto nextBlock;
            }

            if (objectName == "_mouse_") {
                sprite->xPosition = mousePointer.x;
                sprite->yPosition = mousePointer.y;
                goto nextBlock;
            }

            for (Sprite* currentSprite : sprites) {
                if (currentSprite->name == objectName) {
                    sprite->xPosition = currentSprite->xPosition;
                    sprite->yPosition = currentSprite->yPosition;
                    break;
                }
            }
            goto nextBlock;
        }

        case block.MOTION_POINTINDIRECTION: {
            std::string value = getInputValue(block.inputs["DIRECTION"], &block, sprite);
            if (isNumber(value)) {
                sprite->rotation = std::stoi(value);
            } else {
               // std::cerr << "Invalid Turn direction " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_TURNRIGHT: {
            std::string value = getInputValue(block.inputs["DEGREES"], &block, sprite);
            if (isNumber(value)) {
                sprite->rotation += std::stoi(value);
            } else {
               // std::cerr << "Invalid Turn direction " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_TURNLEFT: {
            std::string value = getInputValue(block.inputs["DEGREES"], &block, sprite);
            if (isNumber(value)) {
                sprite->rotation -= std::stoi(value);
            } else {
                //std::cerr << "Invalid Turn direction " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_MOVE_STEPS: {
            std::string value = getInputValue(block.inputs["STEPS"], &block, sprite);
            if (isNumber(value)) {
                double angle = (sprite->rotation - 90) * M_PI / 180.0;
                sprite->xPosition += std::cos(angle) * std::stod(value);
                sprite->yPosition += std::sin(angle) * std::stod(value);
            } else {
               // std::cerr << "Invalid Move steps " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_POINT_TOWARD:{
            Block inputBlock = findBlock(block.inputs["TOWARDS"][1]);

            if(inputBlock.fields.find("TOWARDS") == inputBlock.fields.end()){
                //std::cerr << "Error: Unable to find object for POINT_TOWARD block." << std::endl;
                goto nextBlock;
            }

            std::string objectName = inputBlock.fields["TOWARDS"][0];
            double targetX = 0;
            double targetY = 0;

            if(objectName == "_random_"){
                sprite->rotation = rand() % 360;
                goto nextBlock;
            }

            if (objectName == "_mouse_") {
                targetX = mousePointer.x;
                targetY = mousePointer.y;
            }

            for (Sprite* currentSprite : sprites) {
                if (currentSprite->name == objectName) {
                    targetX = currentSprite->xPosition;
                    targetY = currentSprite->yPosition;
                    break;
                }
            }

            const double dx = targetX - sprite->xPosition;
            const double dy = targetY - sprite->yPosition;
            double angle = 90 - (atan2(dy,dx) * 180.0 / M_PI);
            sprite->rotation = angle;
           // std::cout << "Pointing towards " << sprite->rotation << std::endl;

            goto nextBlock;

        }

        case block.MOTION_CHANGEXBY: {
            std::string value = getInputValue(block.inputs["DX"], &block, sprite);
            if (isNumber(value)) {
                sprite->xPosition += std::stod(value);
            } else {
                //std::cerr << "Invalid X position " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_CHANGEYBY: {
            std::string value = getInputValue(block.inputs["DY"], &block, sprite);
            if (isNumber(value)) {
                sprite->yPosition += std::stod(value);
            } else {
                //std::cerr << "Invalid Y position " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_SETX: {
            std::string value = getInputValue(block.inputs["X"], &block, sprite);
            if (isNumber(value)) {
                sprite->xPosition = std::stod(value);
            } else {
               // std::cerr << "Invalid X position " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.MOTION_SETY: {
            std::string value = getInputValue(block.inputs["Y"], &block, sprite);
            if (isNumber(value)) {
                sprite->yPosition = std::stod(value);
            } else {
                //std::cerr << "Invalid Y position " << value << std::endl;
            }
            goto nextBlock;
        }
        
        case block.LOOKS_SHOW:{
            sprite->visible = true;
            goto nextBlock;
        }
        case block.LOOKS_HIDE:{
            sprite->visible = false;
            goto nextBlock;
        }
        case block.LOOKS_SWITCHCOSTUMETO:{
           std::string inputValue = getValueOfBlock(findBlock(block.inputs["COSTUME"][1]),sprite);
           //std::cout << "costume = " << inputValue << std::endl;
           
           if (isNumber(inputValue)){
                int costumeIndex = std::stoi(inputValue) - 1;
                if (costumeIndex >= 0 && static_cast<size_t>(costumeIndex) < sprite->costumes.size()) {
                    if(sprite->currentCostume != costumeIndex){
                        freeImage(sprite,sprite->costumes[sprite->currentCostume].id);
                    }
                    sprite->currentCostume = costumeIndex;
                } else {
                    //std::cerr << "Invalid costume index: " << costumeIndex << std::endl;
                }
            } else {
                for (size_t i = 0; i < sprite->costumes.size(); i++) {
                    if (sprite->costumes[i].name == inputValue) {
                        if((size_t)sprite->currentCostume != i){
                            freeImage(sprite,sprite->costumes[sprite->currentCostume].id);
                        }
                        sprite->currentCostume = i;
                        break;
                    }
                }
            }
            goto nextBlock;

        }

        case block.LOOKS_NEXTCOSTUME:{
            freeImage(sprite,sprite->costumes[sprite->currentCostume].id);
            sprite->currentCostume++;
            if (sprite->currentCostume >= static_cast<int>(sprite->costumes.size())) {
                sprite->currentCostume = 0;
            }
            goto nextBlock;
        }

        case block.LOOKS_SWITCHBACKDROPTO:{
            std::string inputValue = getValueOfBlock(findBlock(block.inputs["BACKDROP"][1]),sprite);
            for(Sprite* currentSprite : sprites){
                if(!currentSprite->isStage){
                    continue;
                }
            if (isNumber(inputValue)){
                int costumeIndex = std::stoi(inputValue) - 1;
                if (costumeIndex >= 0 && static_cast<size_t>(costumeIndex) < currentSprite->costumes.size()) {
                    if(sprite->currentCostume != costumeIndex){
                    freeImage(currentSprite,currentSprite->costumes[currentSprite->currentCostume].id);}
                    currentSprite->currentCostume = costumeIndex;
                } else {
                   // std::cerr << "Invalid costume index: " << costumeIndex << std::endl;
                }
            } else {
                for (size_t i = 0; i < currentSprite->costumes.size(); i++) {
                    if (currentSprite->costumes[i].name == inputValue) {
                        if((size_t)sprite->currentCostume != i){
                            freeImage(currentSprite,currentSprite->costumes[currentSprite->currentCostume].id);
                        }
                        currentSprite->currentCostume = i;
                        break;
                    }
                }
            }
        }
            goto nextBlock;
        }

        case block.LOOKS_NEXTBACKDROP:{
            for(Sprite* currentSprite : sprites){
                if(!currentSprite->isStage){
                    continue;
                }
            freeImage(currentSprite,currentSprite->costumes[currentSprite->currentCostume].id);
            currentSprite->currentCostume++;
            if (currentSprite->currentCostume >= static_cast<int>(currentSprite->costumes.size())) {
                currentSprite->currentCostume = 0;
            }
        }
            goto nextBlock;
        }

        case block.LOOKS_CHANGESIZEBY:{
            std::string value = getInputValue(block.inputs["CHANGE"], &block, sprite);
            if (isNumber(value)) {
                sprite->size += std::stod(value);
            } else {
               // std::cerr << "Invalid size change " << value << std::endl;
            }
            goto nextBlock;
        }
        case block.LOOKS_SETSIZETO:{
            std::string value = getInputValue(block.inputs["SIZE"], &block, sprite);
            if (isNumber(value)) {
                sprite->size = std::stod(value);
            } else {
              //  std::cerr << "Invalid size change " << value << std::endl;
            }
            goto nextBlock;
        }

        case block.LOOKS_GO_FORWARD_BACKWARD_LAYERS:{
            std::string value = getInputValue(block.inputs["NUM"], &block, sprite);
            std::string forwardBackward = block.fields["FORWARD_BACKWARD"][0];
            if (isNumber(value)) {
            if (forwardBackward == "forward") {
                sprite->layer += std::stoi(value);
            } else if (forwardBackward == "backward") {
                sprite->layer -= std::stoi(value);
            }
        } else {
                //std::cerr << "Invalid layer change " << value << std::endl;
            }
            goto nextBlock;
        }
        case block.LOOKS_GO_TO_FRONT_BACK:{
            std::string value = block.fields["FRONT_BACK"][0];
            if (value == "front") {
                sprite->layer = getMaxSpriteLayer() + 1;
            } else if (value == "back") {
                sprite->layer = 0;
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

        case block.CONTROL_WAIT_UNTIL: {
            if (sprite->conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = -1;
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                sprite->conditionals[newConditional.id] = newConditional;
            }
            if (block.inputs["CONDITION"][1].is_null() || !runConditionalStatement(block.inputs["CONDITION"][1], sprite)) {
                sprite->conditionals[block.id].isTrue = true;
                return;
            } else {
                sprite->conditionals[block.id].isTrue = false;
                waitingBlock = conditionals[block.id].waitingBlock;
            }
            goto nextBlock;
        }

        case block.CONTROL_WAIT: {
            if (sprite->conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = -1;
                newConditional.time = std::chrono::high_resolution_clock::now();
                std::string duration = getInputValue(block.inputs["DURATION"], &block, sprite);
                if(isNumber(duration)) {
                    newConditional.endTime = std::stoi(duration) * 1000; // convert to milliseconds
                } else {
                    newConditional.endTime = 0;
                }
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                sprite->conditionals[newConditional.id] = newConditional;
            }

            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - sprite->conditionals[block.id].time).count();
            if (elapsedTime < sprite->conditionals[block.id].endTime) {
                sprite->conditionals[block.id].isTrue = true;
                return;
            } else {
                sprite->conditionals[block.id].isTrue = false;
                waitingBlock = conditionals[block.id].waitingBlock;
                sprite->conditionals.erase(block.id);
            }
            goto nextBlock;

        }

        case block.CONTROL_REPEAT_UNTIL: {
            if (sprite->conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = -1;
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                sprite->conditionals[newConditional.id] = newConditional;
            }
            if (block.inputs["CONDITION"][1].is_null() || !runConditionalStatement(block.inputs["CONDITION"][1], sprite)) {
                sprite->conditionals[block.id].isTrue = true;
            } else {
                sprite->conditionals[block.id].isTrue = false;
                waitingBlock = conditionals[block.id].waitingBlock;
                //conditionals.erase(block.id);
            }
            if (sprite->conditionals[block.id].isTrue && !block.inputs["SUBSTACK"][1].is_null()) {
                Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                runBlock(subBlock, sprite);
                return;
            }
            if (sprite->conditionals[block.id].isTrue && block.inputs["SUBSTACK"][1].is_null()) {
                return;
            }
            goto nextBlock;
        }

        case block.CONTROL_REPEAT: {
            if (sprite->conditionals.find(block.id) == conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = std::stoi(getInputValue(block.inputs["TIMES"], &block, sprite));
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                sprite->conditionals[newConditional.id] = newConditional;
            }
            if (conditionals[block.id].isTrue && !block.inputs["SUBSTACK"][1].is_null()) {
                Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                runBlock(subBlock, sprite);
            }
            if (sprite->conditionals[block.id].times > 0) {
                sprite->conditionals[block.id].isTrue = true;
                sprite->conditionals[block.id].times--;
                return;
            } else {
                sprite->conditionals[block.id].isTrue = false;
                waitingBlock = conditionals[block.id].waitingBlock;
                sprite->conditionals.erase(block.id);
            }
            goto nextBlock;
        }

        case block.CONTROL_FOREVER: {
            if (sprite->conditionals.find(block.id) == sprite->conditionals.end()) {
                Conditional newConditional;
                newConditional.id = block.id;
                newConditional.blockId = block.id;
                newConditional.hostSprite = sprite;
                newConditional.isTrue = false;
                newConditional.times = -1;
                newConditional.waitingBlock = waitingBlock;
                newConditional.runWithoutScreenRefresh = withoutScreenRefresh;
                sprite->conditionals[newConditional.id] = newConditional;
            }
            if (sprite->conditionals[block.id].isTrue && !block.inputs["SUBSTACK"][1].is_null()) {
                Block subBlock = findBlock(block.inputs["SUBSTACK"][1]);
                runBlock(subBlock, sprite);
            }
            sprite->conditionals[block.id].isTrue = true;
            goto nextBlock;
        }

        case block.CONTROL_CREATE_CLONE_OF: {
            Block cloneOptions = findBlock(block.inputs["CLONE_OPTION"][1]);
            Sprite* spriteToClone = getAvailableSprite();
            if(!spriteToClone) goto nextBlock;
            if (cloneOptions.fields["CLONE_OPTION"][0] == "_myself_") {
                *spriteToClone = *sprite;
            } else {
                for (Sprite* currentSprite : sprites) {
                    if (currentSprite->name == removeQuotations(cloneOptions.fields["CLONE_OPTION"][0]) && currentSprite->isClone == sprite->isClone) {
                        *spriteToClone = *currentSprite;
                    }
                }
            }
            if (spriteToClone != nullptr && !spriteToClone->name.empty()) {
                for (auto& [id, conditional] : spriteToClone->conditionals) {
                    conditional.hostSprite = spriteToClone;
                    conditional.isTrue = false;
                }
                spriteToClone->isClone = true;
                spriteToClone->isStage = false;
                spriteToClone->toDelete = false;
                spriteToClone->id = generateRandomString(15);
                //std::cout << "Created clone of " << sprite->name << std::endl;
                std::unordered_map<std::string, Block> newBlocks;
                for (auto& [id, block] : spriteToClone->blocks) {
                    if (block.opcode == block.CONTROL_START_AS_CLONE || block.opcode == block.EVENT_WHENBROADCASTRECEIVED || block.opcode == block.PROCEDURES_DEFINITION || block.opcode == block.PROCEDURES_PROTOTYPE) {
                        std::vector<Block> blockChain = getBlockChain(block.id);
                        for (const Block& block : blockChain) {
                            newBlocks[block.id] = block;
                        }
                    }
                }
                spriteToClone->blocks.clear();
                spriteToClone->blocks = newBlocks;


                // add clone to sprite list
                sprites.push_back(spriteToClone);
                Sprite* addedSprite = sprites.back();
                // Run "when I start as a clone" scripts for the clone
                for (Sprite* currentSprite : sprites) {
                    if (currentSprite == addedSprite) {
                        for (auto& [id, block] : currentSprite->blocks) {
                            if (block.opcode == block.CONTROL_START_AS_CLONE) {
                               // std::cout << "Running clone block " << block.id << std::endl;
                                runBlock(block, currentSprite);
                            }
                        }
                    }
                }
            }
            goto nextBlock;
        }

        case block.CONTROL_DELETE_THIS_CLONE: {
           // std::cout << "Deleting clone " << sprite->name << std::endl;
            sprite->toDelete = true;
            return;
        }

        case block.CONTROL_STOP: {
            std::string stopType = block.fields["STOP_OPTION"][0];
            if(stopType == "all"){
                toExit = true;
                break;
            }
            if(stopType == "this script"){
                Block parent = getBlockParent(block);
                //std::cout << "Stopping script " << parent.id << std::endl;
                for(auto& [id,block] : sprite->blocks){
                    if(block.topLevelParentBlock == parent.id){
                        if(sprite->conditionals.find(id) != sprite->conditionals.end()){
                            sprite->conditionals.erase(id);
                        }
                    }
                }
                return;
            }

            if(stopType == "other scripts in sprite"){
                std::string topLevelParentBlock = getBlockParent(block).id;
                //std::cout << "Stopping other scripts in sprite " << sprite->id << std::endl;
                for(auto& [id,block] : sprite->blocks){
                    if(block.topLevelParentBlock != topLevelParentBlock){
                        if(sprite->conditionals.find(id) != sprite->conditionals.end()){
                            sprite->conditionals.erase(id);
                        }
                    }
                }
            }
            goto nextBlock;
            
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

        case block.DATA_ADD_TO_LIST:{
            std::string val = getInputValue(block.inputs["ITEM"], &block, sprite);
            std::string listId = block.fields["LIST"][1];
            for(Sprite* currentSprite : sprites){
            if (currentSprite->lists.find(listId) != currentSprite->lists.end()) {
               // std::cout << "Adding to list " << listId << std::endl;
                sprite->lists[listId].items.push_back(val);
                break;
            }
        }
            goto nextBlock;
        }

        case block.DATA_DELETE_OF_LIST: {
            std::string val = getInputValue(block.inputs["INDEX"], &block, sprite);
            std::string listId = block.fields["LIST"][1];
        
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
            goto nextBlock;
        }

        case block.DATA_DELETE_ALL_OF_LIST:{
            std::string listId = block.fields["LIST"][1];
            for (Sprite* currentSprite : sprites) {
                if (currentSprite->lists.find(listId) != currentSprite->lists.end()) {
                    //std::cout << "Deleting all from list " << listId << std::endl;
                    currentSprite->lists[listId].items.clear(); // Clear the list
                    break;
                }
            }
            goto nextBlock;
        }

        case block.DATA_INSERT_AT_LIST:{
            std::string val = getInputValue(block.inputs["ITEM"], &block, sprite);
            std::string listId = block.fields["LIST"][1];
            std::string index = getInputValue(block.inputs["INDEX"], &block, sprite);
        
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
            goto nextBlock;
        }

        case block.DATA_REPLACE_ITEM_OF_LIST:{
            std::string val = getInputValue(block.inputs["ITEM"], &block, sprite);
            std::string listId = block.fields["LIST"][1];
            std::string index = getInputValue(block.inputs["INDEX"], &block, sprite);
        
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
            goto nextBlock;
        }

        case block.SENSING_RESETTIMER: {
            timer = 0;
            goto nextBlock;
        }


        default:
        //std::cerr << "Unhandled opcode: " << block.opcode  << "???????????"<< std::endl;
        goto nextBlock;
    }

nextBlock:
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    if (duration.count() > 0) {
       // std::cout << block.opcode << " took " << duration.count() << " milliseconds!"<< std::endl;
    }
    if (!block.next.empty()) {

            block = *blockLookup[block.next];
    } else {
       runBroadcasts();
        if (!waitingBlock.id.empty()) {
            block = *blockLookup[waitingBlock.id];
            waitingBlock = Block(); // reset waiting block
        }
        else break;
    }
}

}

void runRepeatBlocks(){
    //std::cout<<"Running repeat blocks..."<< std::endl;
    for(auto &currentSprite : sprites){
    for(const auto &[id,data] : currentSprite->conditionals){
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
            //else currentSprite->conditionals.erase(id);
        }
    }
           // remove sprites ready for deletion
           sprites.erase(std::remove_if(sprites.begin(), sprites.end(), [](Sprite* s) { return s->toDelete; }), sprites.end());
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
        
               // std::cout<<"Global Variable set to " << var.value << std::endl;
            }
        }
    }

}

std::string getVariableValue(std::string variableId,Sprite*sprite){

        for(const auto &[id,data] : sprite->variables){
            if (id == variableId) {
                return data.value; // Assuming `Variable` has a `value` field
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

std::string getInputValue(nlohmann::json& item, Block* block, Sprite* sprite) {
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
            return getValueOfBlock(findBlock(data), sprite);
        }
    }
    // 2 SEEMS to be a boolean
    if(type == 2){
        return std::to_string(runConditionalStatement(findBlock(data).id, sprite));
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
                runBlock(data,currentSprite);
            }
        }
    }
}