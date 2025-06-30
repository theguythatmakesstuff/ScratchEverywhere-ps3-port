#include "sound.hpp"

Value SoundBlocks::volume(Block& block, Sprite* sprite) {
    return std::to_string(sprite->volume);
}