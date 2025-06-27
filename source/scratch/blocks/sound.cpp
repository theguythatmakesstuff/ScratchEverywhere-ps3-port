#include "sound.hpp"

std::string SoundBlocks::volume(const Block& block, Sprite* sprite) {
    return std::to_string(sprite->volume);
}