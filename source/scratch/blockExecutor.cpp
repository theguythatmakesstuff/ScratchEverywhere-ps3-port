#include "blockExecutor.hpp"
#include "blocks/motion.hpp"
#include "blocks/events.hpp"
#include "blocks/looks.hpp"
#include "blocks/control.hpp"
#include "blocks/data.hpp"
#include "blocks/sensing.hpp"
#include "blocks/operator.hpp"
#include "blocks/procedure.hpp"
#include "blocks/sound.hpp"

BlockExecutor::BlockExecutor(){
    registerHandlers();
}

void BlockExecutor::registerHandlers(){

    // motion
    handlers[Block::MOTION_MOVE_STEPS] = MotionBlocks::moveSteps;
    handlers[Block::MOTION_GOTOXY] = MotionBlocks::goToXY;
    handlers[Block::MOTION_GOTO] = MotionBlocks::goTo;
    handlers[Block::MOTION_CHANGEXBY] = MotionBlocks::changeXBy;
    handlers[Block::MOTION_CHANGEYBY] = MotionBlocks::changeYBy;
    handlers[Block::MOTION_SETX] = MotionBlocks::setX;
    handlers[Block::MOTION_SETY] = MotionBlocks::setY;
    handlers[Block::MOTION_GLIDE_SECS_TO_XY] = MotionBlocks::glideSecsToXY;
    handlers[Block::MOTION_GLIDE_TO] = MotionBlocks::glideTo;
    handlers[Block::MOTION_TURNRIGHT] = MotionBlocks::turnRight;
    handlers[Block::MOTION_TURNLEFT] = MotionBlocks::turnLeft;
    handlers[Block::MOTION_POINTINDIRECTION] = MotionBlocks::pointInDirection;
    handlers[Block::MOTION_POINT_TOWARD] = MotionBlocks::pointToward;
    handlers[Block::MOTION_SET_ROTATION_STYLE] = MotionBlocks::setRotationStyle;
    handlers[Block::MOTION_IF_ON_EDGE_BOUNCE] = MotionBlocks::ifOnEdgeBounce;
    valueHandlers[Block::MOTION_XPOSITION] = MotionBlocks::xPosition;
    valueHandlers[Block::MOTION_YPOSITION] = MotionBlocks::yPosition;
    valueHandlers[Block::MOTION_DIRECTION] = MotionBlocks::direction;
    
    // looks
    handlers[Block::LOOKS_SHOW] = LooksBlocks::show;
    handlers[Block::LOOKS_HIDE] = LooksBlocks::hide;
    handlers[Block::LOOKS_SWITCHCOSTUMETO] = LooksBlocks::switchCostumeTo;
    handlers[Block::LOOKS_NEXTCOSTUME] = LooksBlocks::nextCostume;
    handlers[Block::LOOKS_SWITCHBACKDROPTO] = LooksBlocks::switchBackdropTo;
    handlers[Block::LOOKS_NEXTBACKDROP] = LooksBlocks::nextBackdrop;
    handlers[Block::LOOKS_GO_FORWARD_BACKWARD_LAYERS] = LooksBlocks::goForwardBackwardLayers;
    handlers[Block::LOOKS_GO_TO_FRONT_BACK] = LooksBlocks::goToFrontBack;
    handlers[Block::LOOKS_SETSIZETO] = LooksBlocks::setSizeTo;
    handlers[Block::LOOKS_CHANGESIZEBY] = LooksBlocks::changeSizeBy;
    valueHandlers[Block::LOOKS_SIZE] = LooksBlocks::size;
    valueHandlers[Block::LOOKS_COSTUME] = LooksBlocks::costume;
    valueHandlers[Block::LOOKS_BACKDROPS] = LooksBlocks::backdrops;
    valueHandlers[Block::LOOKS_COSTUMENUMBERNAME] = LooksBlocks::costumeNumberName;
    valueHandlers[Block::LOOKS_BACKDROPNUMBERNAME] = LooksBlocks::backdropNumberName;
    
    // sound
    valueHandlers[Block::SOUND_VOLUME] = SoundBlocks::volume;

    // events
    handlers[Block::EVENT_WHENFLAGCLICKED] = EventBlocks::flagClicked;
    handlers[Block::EVENT_BROADCAST] = EventBlocks::broadcast;
    handlers[Block::EVENT_WHEN_KEY_PRESSED] = EventBlocks::whenKeyPressed;


    // control
    handlers[Block::CONTROL_IF] = ControlBlocks::If;
    handlers[Block::CONTROL_IF_ELSE] = ControlBlocks::ifElse;
    handlers[Block::CONTROL_CREATE_CLONE_OF] = ControlBlocks::createCloneOf;
    handlers[Block::CONTROL_DELETE_THIS_CLONE] = ControlBlocks::deleteThisClone;
    handlers[Block::CONTROL_STOP] = ControlBlocks::stop;
    handlers[Block::CONTROL_START_AS_CLONE] = ControlBlocks::startAsClone;
    handlers[Block::CONTROL_WAIT] = ControlBlocks::wait;
    handlers[Block::CONTROL_WAIT_UNTIL] = ControlBlocks::waitUntil;
    handlers[Block::CONTROL_REPEAT] = ControlBlocks::repeat;
    handlers[Block::CONTROL_REPEAT_UNTIL] = ControlBlocks::repeatUntil;
    handlers[Block::CONTROL_FOREVER] = ControlBlocks::forever;

    // operators
    valueHandlers[Block::OPERATOR_ADD] = OperatorBlocks::add;
    valueHandlers[Block::OPERATOR_SUBTRACT] = OperatorBlocks::subtract;
    valueHandlers[Block::OPERATOR_MULTIPLY] = OperatorBlocks::multiply;
    valueHandlers[Block::OPERATOR_DIVIDE] = OperatorBlocks::divide;
    valueHandlers[Block::OPERATOR_RANDOM] = OperatorBlocks::random;
    valueHandlers[Block::OPERATOR_JOIN] = OperatorBlocks::join;
    valueHandlers[Block::OPERATOR_LETTER_OF] = OperatorBlocks::letterOf;
    valueHandlers[Block::OPERATOR_LENGTH] = OperatorBlocks::length;
    valueHandlers[Block::OPERATOR_MOD] = OperatorBlocks::mod;
    valueHandlers[Block::OPERATOR_ROUND] = OperatorBlocks::round;
    valueHandlers[Block::OPERATOR_MATHOP] = OperatorBlocks::mathOp;
    conditionBlockHandlers[Block::OPERATOR_EQUALS] = OperatorBlocks::equals;
    conditionBlockHandlers[Block::OPERATOR_GT] = OperatorBlocks::greaterThan;
    conditionBlockHandlers[Block::OPERATOR_LT] = OperatorBlocks::lessThan;
    conditionBlockHandlers[Block::OPERATOR_AND] = OperatorBlocks::and_;
    conditionBlockHandlers[Block::OPERATOR_OR] = OperatorBlocks::or_;
    conditionBlockHandlers[Block::OPERATOR_NOT] = OperatorBlocks::not_;
    conditionBlockHandlers[Block::OPERATOR_CONTAINS] = OperatorBlocks::contains;

    // data
    handlers[Block::DATA_SETVARIABLETO] = DataBlocks::setVariable;
    handlers[Block::DATA_CHANGEVARIABLEBY] = DataBlocks::changeVariable;
    handlers[Block::DATA_ADD_TO_LIST] = DataBlocks::addToList;
    handlers[Block::DATA_DELETE_OF_LIST] = DataBlocks::deleteFromList;
    handlers[Block::DATA_DELETE_ALL_OF_LIST] = DataBlocks::deleteAllOfList;
    handlers[Block::DATA_INSERT_AT_LIST] = DataBlocks::insertAtList;
    handlers[Block::DATA_REPLACE_ITEM_OF_LIST] = DataBlocks::replaceItemOfList;
    valueHandlers[Block::DATA_ITEMOFLIST] = DataBlocks::itemOfList;
    valueHandlers[Block::DATA_ITEMNUMOFLIST] = DataBlocks::itemNumOfList;
    valueHandlers[Block::DATA_LENGTHOFLIST] = DataBlocks::lengthOfList;
    conditionBlockHandlers[Block::DATA_LIST_CONTAINS_ITEM] = DataBlocks::listContainsItem;


    // sensing
    handlers[Block::SENSING_RESETTIMER] = SensingBlocks::resetTimer;
    handlers[Block::SENSING_ASK_AND_WAIT] = SensingBlocks::askAndWait;
    valueHandlers[Block::SENSING_TIMER] = SensingBlocks::sensingTimer;
    valueHandlers[Block::SENSING_OF] = SensingBlocks::of;
    valueHandlers[Block::SENSING_MOUSEX] = SensingBlocks::mouseX;
    valueHandlers[Block::SENSING_MOUSEY] = SensingBlocks::mouseY;
    valueHandlers[Block::SENSING_DISTANCETO] = SensingBlocks::distanceTo;
    valueHandlers[Block::SENSING_DAYS_SINCE_2000] = SensingBlocks::daysSince2000;
    valueHandlers[Block::SENSING_CURRENT] = SensingBlocks::current;
    valueHandlers[Block::SENSING_ANSWER] = SensingBlocks::sensingAnswer;
    conditionBlockHandlers[Block::SENSING_KEYPRESSED] = SensingBlocks::keyPressed;
    conditionBlockHandlers[Block::SENSING_TOUCHINGOBJECT] = SensingBlocks::touchingObject;
    conditionBlockHandlers[Block::SENSING_MOUSEDOWN] = SensingBlocks::mouseDown;

    // procedures / arguments
    handlers[Block::PROCEDURES_CALL] = ProcedureBlocks::call;
    handlers[Block::PROCEDURES_DEFINITION] = ProcedureBlocks::definition;
    valueHandlers[Block::ARGUMENT_REPORTER_STRING_NUMBER] = ProcedureBlocks::stringNumber;
    conditionBlockHandlers[Block::ARGUMENT_REPORTER_BOOLEAN] = ProcedureBlocks::booleanArgument;

}

void BlockExecutor::runBlock(Block block, Sprite* sprite, Block* waitingBlock, bool withoutScreenRefresh){
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!sprite || sprite->toDelete) {
        return;
    }
    
    while (block.id != "null") {

        BlockResult result = executeBlock(block, sprite, &waitingBlock, withoutScreenRefresh);
        
        if (result == BlockResult::RETURN) {
            return;
        } else if (result == BlockResult::BREAK) {
            break;
        }



        
        // Move to next block
        if (!block.next.empty()) {
            block = *blockLookup[block.next];
        } else {
            runBroadcasts();
                break;
        }
    }
        // Timing measurement
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    if (duration.count() > 0) {
        std::cout << " took " << duration.count() << " milliseconds!" << std::endl;
    }
}


BlockResult BlockExecutor::executeBlock(const Block& block, Sprite* sprite,Block** waitingBlock, bool withoutScreenRefresh){
    auto iterator = handlers.find(block.opcode);
    if (iterator != handlers.end()) {
        return iterator->second(block, sprite, waitingBlock, withoutScreenRefresh);
    }

    return BlockResult::CONTINUE;
}

std::string BlockExecutor::getBlockValue(const Block& block,Sprite*sprite){
    auto iterator = valueHandlers.find(block.opcode);
    if (iterator != valueHandlers.end()) {
        return iterator->second(block, sprite);
    }
    return "";
}

bool BlockExecutor::runConditionalBlock(std::string blockId, Sprite* sprite){
    Block* block = findBlock(blockId);
    auto iterator = conditionBlockHandlers.find(block->opcode);
    if (iterator != conditionBlockHandlers.end()) {
        return iterator->second(*block,sprite);
    }
    return false;
}

void BlockExecutor::addToRepeatQueue(Sprite* sprite,Block* block){
    std::cout << "trying..." << std::endl;
            auto& repeatList = sprite->blockChains[block->blockChainID].blocksToRepeat;
            if (std::find(repeatList.begin(), repeatList.end(), block->id) == repeatList.end()) {
                std::cout << "added to list!" << std::endl;
                repeatList.push_back(block->id);
            }
}