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


/* #include <stdint.h> */
/* #include <stddef.h> */

/* uint8_t calculate_cr8x_fast(uint8_t *data, size_t len); */

// // easy_crc.h

// #ifndef EASY_CRC_H
// #define EASY_CRC_H

// #include <stdint.h> // Assuming uint8_t is used
// #include <stddef.h> // Assuming size_t is used

// #ifdef __cplusplus // Check if compiling with a C++ compiler
// extern "C" {       // If yes, declare functions with C linkage
// #endif

// // Function DECLARATION (prototype)
// // This tells both C and C++ compilers about the function
// // and for C++ specifies that it has C linkage (no name mangling)
// uint8_t calculate_cr8x_fast(uint8_t *data, size_t len);

// #ifdef __cplusplus // Close the extern "C" block if in C++
// }
// #endif

// #endif // EASY_CRC_H