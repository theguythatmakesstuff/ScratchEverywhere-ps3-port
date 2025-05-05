#ifndef IMAGE_H
#define IMAGE_H
#include "miniz.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <3ds.h>
#include <citro2d.h>
#include <unordered_map>
#include <cstdint>
using u32 = uint32_t;
using u8 = uint8_t;

struct ImageRGBA {
    std::string name;
    int width;
    int height;
    unsigned char* data;
};

extern std::vector<ImageRGBA> imageRBGAs;

void loadImages(mz_zip_archive *zip);
const u32 next_pow2(u32 n);
const u32 clamp(u32 n, u32 lower, u32 upper);
const u32 rgba_to_abgr(u32 px);
C2D_Image get_C2D_Image(ImageRGBA rgba);


#endif