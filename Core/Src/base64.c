#include "base64.h"

static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int jpeg_to_base64(const uint8_t *input, size_t input_len, char *output, size_t *output_len) {
    if (input == NULL || output == NULL || output_len == NULL) return -1;

    size_t encoded_len = ((input_len + 2) / 3) * 4;
    if (*output_len < encoded_len + 1) { // +1 for null terminator
        *output_len = encoded_len + 1;
        return -2;
    }

    size_t i, j;
    for (i = 0, j = 0; i < input_len; i += 3) {
        uint32_t chunk = 0;
        int bytes_available = 0;

        for (int k = 0; k < 3; k++) {
            if (i + k < input_len) {
                chunk |= (uint32_t)input[i + k] << (16 - (k * 8));
                bytes_available++;
            }
        }

        output[j++] = base64_table[(chunk >> 18) & 0x3F];
        output[j++] = base64_table[(chunk >> 12) & 0x3F];
        output[j++] = (bytes_available > 1) ? base64_table[(chunk >> 6) & 0x3F] : '=';
        output[j++] = (bytes_available > 2) ? base64_table[chunk & 0x3F] : '=';
    }

    output[j] = '\0';
    *output_len = j;
    return 0;
}