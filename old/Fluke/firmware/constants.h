/******************************************************************************
 *
 * JPEG Constants
 * 
 * Ben Johnson <circuitben@gmail.com>
 * April 2008
 * IPRE Fluke Firmware
 *
 ******************************************************************************/
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include "inttypes.h"

/* A Huffman code word */
typedef struct
{
    /* 1-16 bits of data, LSB aligned */
    uint16_t code;
    
    /* Number of bits in this code word */
    uint8_t size;
} codeword_t;

extern const int zigzag_order[];

extern const uint8_t gray_header[];
extern const uint8_t color_header[];
extern const int gray_header_size;
extern const int color_header_size;

extern const int lum_divisors[];
extern const int chrom_divisors[];
extern const codeword_t lum_dc_codes[];
extern const codeword_t lum_ac_codes[];
extern const codeword_t chrom_dc_codes[];
extern const codeword_t chrom_ac_codes[];

#endif /* _CONSTANTS_H_ */
