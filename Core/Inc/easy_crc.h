#ifndef EASY_CRC_H
#define EASY_CRC_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Calculates 8-bit CRC for a given buffer.
// data: pointer to byte array
// len: number of bytes in the array
uint8_t calculate_cr8x_fast(const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // EASY_CRC_H


