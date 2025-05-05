#include "image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


std::vector<ImageRGBA> imageRBGAs;
std::unordered_map<std::string, C2D_Image> imageC2Ds;


const u32 next_pow2(u32 n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
  }
  const u32 clamp(u32 n, u32 lower, u32 upper) {
    if (n < lower)
      return lower;
    if (n > upper)
      return upper;
    return n;
  }
  const u32 rgba_to_abgr(u32 px) {
    u8 r = (px & 0xff000000) >> 24;
    u8 g = (px & 0x00ff0000) >> 16;
    u8 b = (px & 0x0000ff00) >> 8;
    u8 a = px & 0x000000ff;
    return (a << 24) | (b << 16) | (g << 8) | r;
  }

void loadImages(mz_zip_archive*zip){
// Loop through all files in the ZIP
int file_count = (int)mz_zip_reader_get_num_files(zip);
for (int i = 0; i < file_count; i++) {
    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(zip, i, &file_stat)) continue;

    std::string zipFileName = file_stat.m_filename;

    // Check if file is a PNG (case-insensitive match)
    if (zipFileName.size() >= 4 && 
        (zipFileName.substr(zipFileName.size() - 4) == ".png" || zipFileName.substr(zipFileName.size() - 4) == ".PNG")) {

        // Extract the file to memory
        size_t png_size;
        void* png_data = mz_zip_reader_extract_to_heap(zip, i, &png_size, 0);
        if (!png_data) {
            printf("Failed to extract %s\n", zipFileName.c_str());
            continue;
        }

        // Load image from memory into RGBA
        int width, height, channels;
        unsigned char* rgba_data = stbi_load_from_memory(
            (unsigned char*)png_data, png_size,
            &width, &height, &channels, 4 // force RGBA
        );

        if (!rgba_data) {
            printf("Failed to decode PNG: %s\n", zipFileName.c_str());
            mz_free(png_data);
            continue;
        }
        ImageRGBA newRGBA;
        newRGBA.name = zipFileName.substr(0, zipFileName.find_last_of('.'));
        newRGBA.width = width;
        newRGBA.height = height;
        newRGBA.data = rgba_data;
        imageRBGAs.push_back(newRGBA);
        imageC2Ds[newRGBA.name] = get_C2D_Image(newRGBA);
        printf("Loaded PNG: %s (%dx%d)\n", zipFileName.c_str(), width, height);

        // You now have rgba_data with dimensions width x height
        // Store it, render it, or do whatever you want here

        // Free the image and ZIP data when done
        stbi_image_free(rgba_data);
        mz_free(png_data);
    }
}
}

/** Read an RGBA image from `path` with dimensions `image_width`x`image_height`
 * and return a `C2D_Image` object.
 * Assumes image data is stored left->right, top->bottom.
 * Dimensions must be within 64x64 and 1024x1024.
 * Code lovingly taken from sheepy0125 on GBATemp, with edits for my needs. */
C2D_Image get_C2D_Image(ImageRGBA rgba) {

    u32 px_count = rgba.width * rgba.height;
    u32 *rgba_raw = reinterpret_cast<u32*>(rgba.data);

  
    // Image data
    C2D_Image image;
  
    // Base texture
    C3D_Tex *tex = (C3D_Tex *)malloc(sizeof(C3D_Tex));
    image.tex = tex;
    // Texture dimensions must be square powers of two between 64x64 and 1024x1024
    tex->width = clamp(next_pow2(rgba.width), 64, 1024);
    tex->height = clamp(next_pow2(rgba.height), 64, 1024);
  
    // Subtexture
    Tex3DS_SubTexture *subtex = (Tex3DS_SubTexture *)malloc(sizeof(Tex3DS_SubTexture));
    image.subtex = subtex;
    subtex->width = rgba.width;
    subtex->height = rgba.height;
    // (U, V) coordinates
    subtex->left = 0.0f;
    subtex->top = 1.0f;
    subtex->right = (float)rgba.width / (float)tex->width;
    subtex->bottom = 1.0 - ((float)rgba.height / (float)tex->height);
  
    C3D_TexInit(tex, tex->width, tex->height, GPU_RGBA8);
    C3D_TexSetFilter(tex, GPU_LINEAR, GPU_NEAREST);
  
    memset(tex->data, 0, px_count * 4);
    for (u8 i = 0; i < rgba.height; i++) {
      for (u8 j = 0; j < rgba.width; j++) {
        u32 src_idx = (j * rgba.width) + i;
        u32 rgba_px = rgba_raw[src_idx];
        u32 abgr_px = rgba_to_abgr(rgba_px);
  
        // Swizzle magic to convert into a t3x format
        u32 dst_ptr_offset = ((((j >> 3) * (tex->width >> 3) + (i >> 3)) << 6) +
                              ((i & 1) | ((j & 1) << 1) | ((i & 2) << 1) |
                               ((j & 2) << 2) | ((i & 4) << 2) | ((j & 4) << 3)));
        ((u32 *)tex->data)[dst_ptr_offset] = abgr_px;
      }
    }
  
    free(rgba_raw);
  
    return image;
  }