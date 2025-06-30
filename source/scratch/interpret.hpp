#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#pragma once
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
#include "keyboard.hpp"
#include "blockExecutor.hpp"
#include "math.hpp"
#include <chrono>
#include <random>
#include <time.hpp>


struct Mouse{
    int x;
    int y;
    bool isPressed;
    bool isMoving;
};

enum ProjectType{
	UNZIPPED,
	EMBEDDED,
	UNEMBEDDED
};

class BlockExecutor;
extern BlockExecutor executor;

extern ProjectType projectType;

extern std::vector<Sprite*> sprites;
extern std::vector<Sprite> spritePool;
extern std::vector<std::string> broadcastQueue;
//extern std::unordered_map<std::string,Conditional> conditionals;
extern std::unordered_map<std::string, Block*> blockLookup;
extern Mouse mousePointer;
extern double timer;
extern bool toExit;
extern std::string answer;

class Scratch{
public:
    static std::string getInputValue(const nlohmann::json& item,const Block* block,Sprite* sprite);
};

std::vector<std::pair<double, double>> getCollisionPoints(Sprite* currentSprite);
void loadSprites(const nlohmann::json& json);
void cleanupSprites();
Block* getBlockParent(const Block* block);
void initializeSpritePool(int poolSize);
Sprite* getAvailableSprite();
void initializeSpritePool(int poolSize);
std::string findCustomValue(std::string valueName,Sprite*sprite,Block block);
std::string removeQuotations(std::string value);
void runBroadcasts();
void runCustomBlock(Sprite* sprite, const Block& block, Block* callerBlock,bool* withoutScreenRefresh);
//bool hasAnyConditionals(Sprite* sprite, std::string topLevelParentBlockId);
Block* findBlock(std::string blockId);
std::vector<Sprite*> findSprite(std::string spriteName);
void runAllBlocksByOpcode(Block::opCode opcodeToFind);

std::string getVariableValue(std::string variableId,Sprite*sprite);
//void buildBlockHierarchyCache();
//bool hasActiveConditionalsInside(Sprite* sprite, std::string blockId);
//void processBlockForCache(Sprite* sprite,Block* block, std::string parentConditionalId, Block* topLevelBlock);
void setVariableValue(std::string variableId,std::string value,Sprite* sprite,bool isChangingBy);
std::string generateRandomString(int length);
std::vector<Block*> getBlockChain(std::string blockId,std::string* outID = nullptr);
