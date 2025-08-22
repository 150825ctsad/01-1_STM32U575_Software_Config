#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_jpeg.h"
#include <stdint.h>
#include <stdlib.h>

// 禁用tiny_jpeg调试日志
#define NDEBUG

// 图像解码结果结构体
typedef struct {
    uint8_t* pixel_data;
    int width;
    int height;
} DecodedImage;

// JPEG解码（使用tiny_jpeg）
DecodedImage decode_jpeg(const uint8_t* jpeg_data, size_t data_size) {
    DecodedImage result = {0};
    int channels;
    uint8_t* pixels = stbi_load_from_memory(jpeg_data, data_size, &result.width, &result.height, &channels, STBI_rgb);
    if (pixels != NULL) {
        result.pixel_data = pixels;
    }
    return result;
}
// 通用图像解码（使用stb_image支持多种格式）
DecodedImage decode_image(const uint8_t* data, size_t data_size) {
    DecodedImage result = {NULL, 0, 0};
    int w, h, ch;
    
    // 自动检测图像格式（JPEG/PNG/BMP等）
    uint8_t* pixels = stbi_load_from_memory(data, data_size, &w, &h, &ch, 3);
    if (pixels) {
        result.width = w;
        result.height = h;
        result.pixel_data = pixels;
    }
    return result;
}

// 释放解码图像内存
void free_image(DecodedImage* image) {
    if (image->pixel_data) {
        // 根据解码方式选择对应释放函数
        stbi_image_free(image->pixel_data);
        // 若使用tiny_jpeg解码需改用：tje_free(image->pixel_data);
        image->pixel_data = NULL;
    }
}
