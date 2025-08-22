#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stddef.h>

int jpeg_to_base64(const uint8_t *input, size_t input_len, char *output, size_t *output_len);

#endif /* BASE64_H */