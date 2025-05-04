#ifndef SPRITE_H
#define SPRITE_H
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

class Sprite;

struct Variable {
    std::string id;
    std::string name;
    std::string value;
};

struct Block {
    std::string id;
    std::string opcode;
    std::string next;
    std::string parent;
    std::unordered_map<std::string, nlohmann::json> inputs;
    std::unordered_map<std::string, nlohmann::json> fields;
    bool shadow;
    bool topLevel;
};

struct CustomBlock{

    std::string name;
    std::string tagName;
    std::vector<std::string> argumentIds;
    std::vector<std::string> argumentNames;
    std::vector<std::string> argumentDefaults;
    bool runWithoutScreenRefresh;

};

struct Conditional{
    std::string id;
    std::string blockId;
    Sprite* hostSprite;
    bool isTrue;
    int times;
};

struct List{
    std::string id;
    std::string name;
    std::vector<std::string> items;
};

struct Sound{
    std::string id;
    std::string name;
    std::string dataFormat;
    std::string fullName;
    int sampleRate;
    int sampleCount;

};

struct Costume{
    std::string id;
    std::string name;
    std::string fullName;
    std::string dataFormat;
    int bitmapResolution;
    double rotationCenterX;
    double rotationCenterY;
};

struct Comment{
    std::string id;
    std::string blockId;
    std::string text;
    bool minimized;
    int x;
    int y;
    int width;
    int height;

};

struct Broadcast{
    std::string id;
    std::string name;
};

class Sprite {
    public:
        std::string name;
        std::string id;
        bool isStage;
        bool draggable;
        bool visible;
        bool isClone;
        int currentCostume;
        int volume;
        int xPosition;
        int yPosition;
        int size;
        int rotation;
        int layer;
        std::string rotationStyle;
    

        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, Block> blocks;
        std::unordered_map<std::string, List> lists;
        std::unordered_map<std::string, Sound> sounds;
        std::unordered_map<std::string, Costume> costumes;
        std::unordered_map<std::string, Comment> comments;
        std::unordered_map<std::string, Broadcast> broadcasts;
        std::unordered_map<std::string, Conditional> conditionals;
        std::unordered_map<std::string, CustomBlock> customBlocks;
    
        void loadFromJson(const nlohmann::json& json);
        void runScript(const std::string& blockId);
        void executeBlock(const Block& block);
    };

#endif