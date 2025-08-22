#ifndef IMAGE_DECODER_H
#define IMAGE_DECODER_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t* pixel_data;
    int width;
    int height;
} DecodedImage;

// JPEG专用解码（tiny_jpeg实现）
DecodedImage decode_jpeg(const uint8_t* jpeg_data, size_t data_size);

// 通用图像解码（stb_image实现，支持多格式）
DecodedImage decode_image(const uint8_t* data, size_t data_size);

// 释放图像数据
void free_image(DecodedImage* image);

#endif // IMAGE_DECODER_H
