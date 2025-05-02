#include "interpret.hpp"

std::vector<Sprite> sprites;

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
        std::cout<<"name = "<< newSprite.name << std::endl;


        // set variables
        for (const auto& [id,data] : target["variables"].items()){
            
            Variable newVariable;
            newVariable.id = id;
            newVariable.name = data[0];
            newVariable.value = data[1];
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
            if (data.contains("parent") && !data["parent"].is_null()){
            newBlock.next = data["parent"].get<std::string>();}
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
            newList.items = data[1];
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
            std::cout<<"broadcast name = "<< newBroadcast.name << std::endl;
        }

        sprites.push_back(newSprite);
    }
}