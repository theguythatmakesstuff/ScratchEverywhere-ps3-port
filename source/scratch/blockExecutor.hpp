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
    std::unordered_map<Block::opCode, std::function<BlockResult(const Block&, Sprite*, const Block&, bool)>> handlers;
    
public:
    BlockExecutor();
    void runBlock(Block block, Sprite* sprite, Block waitingBlock = Block(), bool withoutScreenRefresh = false);
    
private:
    void registerHandlers();
    BlockResult executeBlock(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh);
};