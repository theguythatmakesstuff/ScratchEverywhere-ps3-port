#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef INTERPRET_H
#define INTERPRET_H
#include "sprite.hpp"
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <iostream>
#include <list>
#include <cmath>
#include <vector>
#include "input.hpp"
#include "render.hpp"
#include <chrono>
#include <random>


struct Mouse{
    int x;
    int y;
    bool isPressed;
};

enum ProjectType{
	UNZIPPED,
	EMBEDDED,
	UNEMBEDDED
};

extern ProjectType projectType;

extern std::vector<Sprite*> sprites;
extern std::vector<Sprite> spritePool;
extern std::vector<Sprite> spritePool;
extern std::vector<std::string> broadcastQueue;
extern std::unordered_map<std::string,Conditional> conditionals;
extern std::unordered_map<std::string, Block*> blockLookup;
extern Mouse mousePointer;
extern double timer;
extern bool toExit;



std::vector<std::pair<double, double>> getCollisionPoints(Sprite* currentSprite);
void loadSprites(const nlohmann::json& json);
void cleanupSprites();
Block getBlockParent(Block block);
void initializeSpritePool(int poolSize);
void runRepeatBlocks();
Sprite* getAvailableSprite();
void initializeSpritePool(int poolSize);
std::string findCustomValue(std::string valueName,Sprite*sprite,Block block);
std::string removeQuotations(std::string value);
std::string getValueOfBlock(Block block,Sprite*sprite);
bool runConditionalStatement(std::string blockId,Sprite* sprite);
void runBroadcasts();
void runCustomBlock(Sprite*sprite,Block block);
void runBlock(Block block,Sprite*sprite,Block waitingBlock = Block(), bool withoutScreenRefresh = false);
Block findBlock(std::string blockId);
std::vector<Sprite*> findSprite(std::string spriteName);
void runAllBlocksByOpcode(Block::opCode opcodeToFind);
std::string getInputValue(nlohmann::json& item,Block* block,Sprite* sprite);
std::string getVariableValue(std::string variableId,Sprite*sprite);
bool isNumber(const std::string& id);
void setVariableValue(std::string variableId,std::string value,Sprite* sprite,bool isChangingBy);

#endif