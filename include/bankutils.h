#ifndef __INCLUDE_BANKUTILS_H__
#define __INCLUDE_BANKUTILS_H__

#include <gbdk/platform.h>
#include <stdint.h>

void * zx0_decompress_banked(const void *sour, void *dest, uint8_t bank);

#endif
