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
#include <iomanip>

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

    // control
    handlers[Block::CONTROL_IF] = ControlBlocks::If;
    handlers[Block::CONTROL_IF_ELSE] = ControlBlocks::ifElse;
    handlers[Block::CONTROL_CREATE_CLONE_OF] = ControlBlocks::createCloneOf;
    handlers[Block::CONTROL_DELETE_THIS_CLONE] = ControlBlocks::deleteThisClone;
    handlers[Block::CONTROL_STOP] = ControlBlocks::stop;

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

    // procedures / arguments
    valueHandlers[Block::ARGUMENT_REPORTER_STRING_NUMBER] = ProcedureBlocks::stringNumber;

}

void BlockExecutor::runBlock(Block block, Sprite* sprite, Block waitingBlock, bool withoutScreenRefresh){
    auto start = std::chrono::high_resolution_clock::now();
    
    if (!sprite || sprite->toDelete) {
        return;
    }
    
    while (block.id != "null") {

        BlockResult result = executeBlock(block, sprite, waitingBlock, withoutScreenRefresh);
        
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
            if (!waitingBlock.id.empty() && blockLookup.find(waitingBlock.id) != blockLookup.end()) {
                block = *blockLookup[waitingBlock.id];
                std::cout << "block is now " << block.id << " from waiting." << std::endl;
                withoutScreenRefresh = false;
                waitingBlock = Block(); // reset waiting block
            } else {
                break;
            }
        }
    }
        // Timing measurement
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    if (duration.count() > 0) {
        std::cout << " took " << duration.count() << " milliseconds!" << std::endl;
    }
}


BlockResult BlockExecutor::executeBlock(const Block& block, Sprite* sprite, const Block& waitingBlock, bool withoutScreenRefresh){
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