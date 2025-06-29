#pragma once
#include <chrono>
#include <unordered_map>
#include <functional>
#include "sprite.hpp"
#include "interpret.hpp"

enum class BlockResult {
    CONTINUE,
    RETURN,
    BREAK
};

class BlockExecutor {
private:
    std::unordered_map<Block::opCode, std::function<BlockResult(Block*, Sprite*,Block**, bool*)>> handlers;
    std::unordered_map<Block::opCode, std::function<std::string(const Block&, Sprite*)>> valueHandlers;
    std::unordered_map<Block::opCode, std::function<bool(const Block& block,Sprite*)>> conditionBlockHandlers;
    
public:
    BlockExecutor();
    void runBlock(Block* block, Sprite* sprite, Block* waitingBlock = nullptr, bool* withoutScreenRefresh = nullptr);
    static void runRepeatBlocks();
    static void runRepeatsWithoutRefresh(Sprite* sprite,std::string blockChainID);
    std::string getBlockValue(const Block& block,Sprite*sprite);
    bool runConditionalBlock(std::string blockId, Sprite* sprite);
    static void addToRepeatQueue(Sprite* sprite,Block* block);
    static bool hasActiveRepeats(Sprite* sprite,std::string blockChainID);
    
private:
    void registerHandlers();
    BlockResult executeBlock(Block* block, Sprite* sprite,Block** waitingBlock, bool* withoutScreenRefresh);
};