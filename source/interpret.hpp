#ifndef INTERPRET_H
#define INTERPRET_H
#include "sprite.hpp"
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <iostream>
#include <list>

extern std::list<Sprite> sprites;
extern std::unordered_map<std::string,Conditional> conditionals;
extern double timer;


void loadSprites(const nlohmann::json& json);
void runRepeatBlocks();
std::string removeQuotations(std::string value);
std::string getValueOfBlock(Block block,Sprite*sprite);
bool runConditionalStatement(std::string blockId,Sprite* sprite);
void runBlock(Block block,Sprite*sprite);
Block findBlock(std::string blockId);
std::vector<Sprite*> findSprite(std::string spriteName);
void runAllBlocksByOpcode(std::string opcodeToFind);
std::string getInputValue(nlohmann::json item,Block* block,Sprite* sprite);
std::string getVariableValue(std::string variableId);
bool isNumber(const std::string& id);
void setVariableValue(std::string variableId,std::string value,Sprite* sprite,bool isChangingBy);

#endif