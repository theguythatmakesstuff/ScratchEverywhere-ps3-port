#ifndef SPRITE_H
#define SPRITE_H
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <iostream>

class Sprite;

struct Variable {
    std::string id;
    std::string name;
    std::string value;
};



struct Block {
    enum opCode{
        NONE,
        EVENT_WHENFLAGCLICKED,
        EVENT_WHENBROADCASTRECEIVED,
        EVENT_BROADCAST,
        PROCEDURES_PROTOTYPE,
        PROCEDURES_DEFINITION,
        PROCEDURES_CALL,
        ARGUMENT_REPORTER_STRING_NUMBER,
        ARGUMENT_REPORTER_BOOLEAN,
        MOTION_XPOSITION,
        MOTION_YPOSITION,
        MOTION_DIRECTION,
        MOTION_GOTOXY,
        MOTION_POINTINDIRECTION,
        MOTION_TURNRIGHT,
        MOTION_TURNLEFT,
        MOTION_CHANGEXBY,
        MOTION_CHANGEYBY,
        MOTION_SETX,
        MOTION_SETY,
        LOOKS_SIZE,
        SOUND_VOLUME,
        SENSING_TIMER,
        SENSING_RESETTIMER,
        CONTROL_IF,
        CONTROL_IF_ELSE,
        CONTROL_REPEAT_UNTIL,
        CONTROL_WHILE,
        CONTROL_FOREVER,
        CONTROL_REPEAT,
        CONTROL_CREATE_CLONE_OF,
        CONTROL_CREATE_CLONE_OF_MENU,
        CONTROL_START_AS_CLONE,
        CONTROL_DELETE_THIS_CLONE,
        DATA_SETVARIABLETO,
        DATA_CHANGEVARIABLEBY,
        OPERATOR_ADD,
        OPERATOR_SUBTRACT,
        OPERATOR_MULTIPLY,
        OPERATOR_DIVIDE,
        OPERATOR_RANDOM,
        OPERATOR_JOIN,
        OPERATOR_LETTER_OF,
        OPERATOR_LENGTH,
        OPERATOR_MOD,
        OPERATOR_ROUND,
        OPERATOR_MATHOP,
        DATA_ITEMOFLIST,
        DATA_ITEMNUMOFLIST,
        DATA_LENGTHOFLIST,
        SENSING_KEYPRESSED,
        OPERATOR_EQUALS,
        OPERATOR_GT,
        OPERATOR_LT,
        OPERATOR_AND,
        OPERATOR_OR,
        OPERATOR_NOT,
        OPERATOR_CONTAINS,
        



    };
    
    opCode stringToOpcode(std::string opCodeString){
        if(opCodeString == "event_whenflagclicked")return EVENT_WHENFLAGCLICKED;
        if(opCodeString == "event_whenbroadcastreceived")return EVENT_WHENBROADCASTRECEIVED;
        if(opCodeString == "event_broadcast")return EVENT_BROADCAST;
        if(opCodeString == "procedures_prototype")return PROCEDURES_PROTOTYPE;
        if(opCodeString == "procedures_definition")return PROCEDURES_DEFINITION;
        if(opCodeString == "procedures_call")return PROCEDURES_CALL;
        if(opCodeString == "argument_reporter_string_number")return ARGUMENT_REPORTER_STRING_NUMBER;
        if(opCodeString == "argument_reporter_boolean")return ARGUMENT_REPORTER_BOOLEAN;
        if(opCodeString == "motion_xposition")return MOTION_XPOSITION;
        if(opCodeString == "motion_yposition")return MOTION_YPOSITION;
        if(opCodeString == "motion_direction")return MOTION_DIRECTION;
        if(opCodeString == "motion_gotoxy")return MOTION_GOTOXY;
        if(opCodeString == "motion_pointindirection")return MOTION_POINTINDIRECTION;
        if(opCodeString == "motion_turnright")return MOTION_TURNRIGHT;
        if(opCodeString == "motion_turnleft")return MOTION_TURNLEFT;
        if(opCodeString == "motion_changexby")return MOTION_CHANGEXBY;
        if(opCodeString == "motion_changeyby")return MOTION_CHANGEYBY;
        if(opCodeString == "motion_setx")return MOTION_SETX;
        if(opCodeString == "motion_sety")return MOTION_SETY;
        if(opCodeString == "looks_size")return LOOKS_SIZE;
        if(opCodeString == "sound_volume")return SOUND_VOLUME;
        if(opCodeString == "sensing_timer")return SENSING_TIMER;
        if(opCodeString == "sensing_resettimer")return SENSING_RESETTIMER;
        if(opCodeString == "control_if")return CONTROL_IF;
        if(opCodeString == "control_if_else")return CONTROL_IF_ELSE;
        if(opCodeString == "control_repeat_until")return CONTROL_REPEAT_UNTIL;
        if(opCodeString == "control_while")return CONTROL_WHILE;
        if(opCodeString == "control_forever")return CONTROL_FOREVER;
        if(opCodeString == "control_repeat")return CONTROL_REPEAT;
        if(opCodeString == "control_create_clone_of")return CONTROL_CREATE_CLONE_OF;
        if(opCodeString == "control_create_clone_of_menu")return CONTROL_CREATE_CLONE_OF_MENU;
        if(opCodeString == "control_start_as_clone")return CONTROL_START_AS_CLONE;
        if(opCodeString == "control_delete_this_clone")return CONTROL_DELETE_THIS_CLONE;
        if(opCodeString == "data_setvariableto")return DATA_SETVARIABLETO;
        if(opCodeString == "data_changevariableby")return DATA_CHANGEVARIABLEBY;
        if(opCodeString == "operator_add")return OPERATOR_ADD;
        if(opCodeString == "operator_subtract")return OPERATOR_SUBTRACT;
        if(opCodeString == "operator_multiply")return OPERATOR_MULTIPLY;
        if(opCodeString == "operator_divide")return OPERATOR_DIVIDE;
        if(opCodeString == "operator_random")return OPERATOR_RANDOM;
        if(opCodeString == "operator_join")return OPERATOR_JOIN;
        if(opCodeString == "operator_letter_of")return OPERATOR_LETTER_OF;
        if(opCodeString == "operator_length")return OPERATOR_LENGTH;
        if(opCodeString == "operator_mod")return OPERATOR_MOD;
        if(opCodeString == "operator_round")return OPERATOR_ROUND;
        if(opCodeString == "operator_mathop")return OPERATOR_MATHOP;
        if(opCodeString == "data_itemoflist")return DATA_ITEMOFLIST;
        if(opCodeString == "data_itemnumoflist")return DATA_ITEMNUMOFLIST;
        if(opCodeString == "data_lengthoflist")return DATA_LENGTHOFLIST;
        if(opCodeString == "sensing_keypressed")return SENSING_KEYPRESSED;
        if(opCodeString == "operator_equals")return OPERATOR_EQUALS;
        if(opCodeString == "operator_gt")return OPERATOR_GT;
        if(opCodeString == "operator_lt")return OPERATOR_LT;
        if(opCodeString == "operator_and")return OPERATOR_AND;
        if(opCodeString == "operator_or")return OPERATOR_OR;
        if(opCodeString == "operator_not")return OPERATOR_NOT;
        if(opCodeString == "operator_contains")return OPERATOR_CONTAINS;
        std::cerr << "Unknown opcode: " << opCodeString << std::endl;
        return NONE;

    }
    
    std::string id;
    opCode opcode;
    std::string next;
    Block* nextBlock;
    std::string parent;
    std::unordered_map<std::string, nlohmann::json> inputs;
    std::unordered_map<std::string, nlohmann::json> fields;
    std::unordered_map<std::string, nlohmann::json> mutation;
    bool shadow;
    bool topLevel;

};

struct CustomBlock{

    std::string name;
    std::string tagName;
    std::string blockId;
    std::vector<std::string> argumentIds;
    std::vector<std::string> argumentNames;
    std::vector<std::string> argumentDefaults;
    std::unordered_map<std::string,std::string> argumentValues;
    bool runWithoutScreenRefresh;

};

struct Conditional{
    std::string id;
    std::string blockId;
    Sprite* hostSprite;
    Block waitingBlock;
    bool runWithoutScreenRefresh;
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