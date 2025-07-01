#pragma once
#include "miniz/miniz.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <algorithm>
using u32 = uint32_t;
using u8 = uint8_t;

struct ImageRGBA {
    std::string name;
    int width;
    int height;
    unsigned char* data;
};

struct Image{
    C2D_Image image;
    u16 freeTimer = 120;
};

extern std::vector<ImageRGBA> imageRBGAs;
extern std::unordered_map<std::string, Image> imageC2Ds;

void loadImages(mz_zip_archive *zip);
void loadImageFromFile(std::string filePath);
const u32 next_pow2(u32 n);
const u32 clamp(u32 n, u32 lower, u32 upper);
const u32 rgba_to_abgr(u32 px);
C2D_Image get_C2D_Image(ImageRGBA rgba);
void freeImage(const std::string& costumeId);
void FlushImages();