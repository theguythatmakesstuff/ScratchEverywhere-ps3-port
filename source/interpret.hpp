#ifndef INTERPRET_H
#define INTERPRET_H
#include "sprite.hpp"
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <iostream>

extern std::vector<Sprite> sprites;

void loadSprites(const nlohmann::json& json);
void runBlock(std::string blockId);
Block findBlock(std::string blockId);
void runAllBlocksByOpcode(std::string opcode);

#endif