/******************************************************************************
 *
 * JPEG Compression 
 *   - DCT JPEG from the the Independent JPEG Group's libjpeg (see notice below)
 * 
 * Ben Johnson <circuitben@gmail.com>
 * April 2008
 * IPRE Fluke Firmware
 *
 * 
 *
 ******************************************************************************/

/******************************************************************************
 * The Independent JPEG Group's libjpeg copyright notice below 
 *
 * see JPEG_README
 * 
 * The authors make NO WARRANTY or representation, either express or implied, 
 * with respect to this software, its quality, accuracy, merchantability, or 
 * fitness for a particular purpose.  This software is provided "AS IS", and you, 
 * its user, assume the entire risk as to its quality and accuracy. 
 *
 * This software is copyright (C) 1991-1998, Thomas G. Lane. 
 * All Rights Reserved except as specified below. 
 *
 * Permission is hereby granted to use, copy, modify, and distribute this 
 * software (or portions thereof) for any purpose, without fee, subject to these 
 * conditions: 
 * (1) If any part of the source code for this software is distributed, then this 
 * README file must be included, with this copyright and no-warranty notice 
 * unaltered; and any additions, deletions, or changes to the original files 
 * must be clearly indicated in accompanying documentation. 
 * (2) If only executable code is distributed, then the accompanying 
 * documentation must state that "this software is based in part on the work of 
 * the Independent JPEG Group". 
 * (3) Permission for use of this software is granted only if the user accepts 
 * full responsibility for any undesirable consequences; the authors accept 
 * NO LIABILITY for damages of any kind. 
 *
 * These conditions apply to any software derived from or based on the IJG code, 
 * not just to the unmodified library.  If you use our work, you ought to 
 * acknowledge us. 
 *
 * Permission is NOT granted for the use of any IJG author's name or company name 
 * in advertising or publicity relating to this software or products derived from 
 * it.  This software may be referred to only as "the Independent JPEG Group's T
 * software". 
 *
 * We specifically permit and encourage the use of this software as the basis of 
 * commercial products, provided that all warranty or liability claims are 
 * assumed by the product vendor. 
 *
 ******************************************************************************/

#include "fluke.h"
#include "ov7649.h"
#include "constants.h"
#include "jpeg.h"

extern uint8_t img[];

// Bytes are filled in MSB to LSB.
int bits_left = 32;
uint32_t cur_word;

// DC coefficient of last MCU
int dc_pred[3] = {0, 0, 0};

void safe_putch(uint8_t byte)
{
    putch(byte);
    
    // Byte stuffing
    if (byte == 0xff)
    {
        putch(0);
    }
}

// Writes <size> bits of <value>, where the bits are LSB-aligned in <value>.
void write_bits(uint32_t value, int size)
{
    do
    {
        if (size >= bits_left)
        {
            // Add as many bits as possible to cur_word
            cur_word |= value >> (size - bits_left);
            
            safe_putch((cur_word >> 24) & 0xff);
            safe_putch((cur_word >> 16) & 0xff);
            safe_putch((cur_word >> 8) & 0xff);
            safe_putch(cur_word & 0xff);
            cur_word = 0;
            
            size -= bits_left;
            bits_left = 32;
        } else {
            cur_word |= value << (bits_left - size);
            bits_left -= size;
            size = 0;
        }
    } while (size);
}

// Writes a Huffman codeword
void write_code(const codeword_t *c)
{
    write_bits(c->code, c->size);
}

// Writes 1 bits to finish the current byte
void byte_align()
{
    int n = bits_left & 7;
    write_bits((1 << n) - 1, n);
}

// Returns SSSS according to table F.1 (called CSIZE in the spec)
int magnitude_category(int n)
{
    if (n < 0)
    {
        n = -n;
    }
    
    int s = 0;
    while (n)
    {
        ++s;
        n >>= 1;
    }
    
    return s;
}

#define DCTSIZE     8
#define DCTSIZE2    64
#define DCTELEM     int32_t

/* Scaling decisions are generally the same as in the LL&M algorithm;
 * see jfdctint.c for more details.  However, we choose to descale
 * (right shift) multiplication products as soon as they are formed,
 * rather than carrying additional fractional bits into subsequent additions.
 * This compromises accuracy slightly, but it lets us save a few shifts.
 * More importantly, 16-bit arithmetic is then adequate (for 8-bit samples)
 * everywhere except in the multiplications proper; this saves a good deal
 * of work on 16-bit-int machines.
 *
 * Again to save a few shifts, the intermediate results between pass 1 and
 * pass 2 are not upscaled, but are represented only to integral precision.
 *
 * A final compromise is to represent the multiplicative constants to only
 * 8 fractional bits, rather than 13.  This saves some shifting work on some
 * machines, and may also reduce the cost of multiplication (since there
 * are fewer one-bits in the constants).
 */

#define CONST_BITS  8
#define CONST_SCALE (1 << CONST_BITS)
#define FIX(x)  ((int32_t) ((x) * CONST_SCALE + 0.5))

/* Some C compilers fail to reduce "FIX(constant)" at compile time, thus
 * causing a lot of useless floating-point operations at run time.
 * To get around this we use the following pre-calculated constants.
 * If you change CONST_BITS you may want to add appropriate values.
 * (With a reasonable C compiler, you can just rely on the FIX() macro...)
 */

#if CONST_BITS == 8
#define FIX_0_382683433  ((int32_t)   98)     /* FIX(0.382683433) */
#define FIX_0_541196100  ((int32_t)  139)     /* FIX(0.541196100) */
#define FIX_0_707106781  ((int32_t)  181)     /* FIX(0.707106781) */
#define FIX_1_306562965  ((int32_t)  334)     /* FIX(1.306562965) */
#else
#define FIX_0_382683433  FIX(0.382683433)
#define FIX_0_541196100  FIX(0.541196100)
#define FIX_0_707106781  FIX(0.707106781)
#define FIX_1_306562965  FIX(1.306562965)
#endif

/* Multiply a DCTELEM variable by an int32_t constant, and immediately
 * descale to yield a DCTELEM result.
 */
//#define MULTIPLY(var,const)  ((DCTELEM)((var) * (const) + (1 << (CONST_BITS - 1))) >> CONST_BITS)
#define MULTIPLY(var,const)  ((DCTELEM)((var) * (const)) >> CONST_BITS)

/*
 * Perform the forward DCT on one block of samples.
 */
void fast_dct(int *data, int *out, const int *divisors)
{
    DCTELEM tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    DCTELEM tmp10, tmp11, tmp12, tmp13;
    DCTELEM z1, z2, z3, z4, z5, z11, z13;
    int ctr;
    
    /* Pass 1: process rows. */
    
    DCTELEM *dataptr = data;
    for (ctr = DCTSIZE-1; ctr >= 0; ctr--)
    {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];
        
        /* Even part */
        
        tmp10 = tmp0 + tmp3;    /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;
        
        dataptr[0] = tmp10 + tmp11; /* phase 3 */
        dataptr[4] = tmp10 - tmp11;
        
        z1 = MULTIPLY(tmp12 + tmp13, FIX_0_707106781); /* c4 */
        dataptr[2] = tmp13 + z1;    /* phase 5 */
        dataptr[6] = tmp13 - z1;
        
        /* Odd part */
    
        tmp10 = tmp4 + tmp5;    /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;
    
        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = MULTIPLY(tmp10 - tmp12, FIX_0_382683433); /* c6 */
        z2 = MULTIPLY(tmp10, FIX_0_541196100) + z5; /* c2-c6 */
        z4 = MULTIPLY(tmp12, FIX_1_306562965) + z5; /* c2+c6 */
        z3 = MULTIPLY(tmp11, FIX_0_707106781); /* c4 */
    
        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;
    
        dataptr[5] = z13 + z2;  /* phase 6 */
        dataptr[3] = z13 - z2;
        dataptr[1] = z11 + z4;
        dataptr[7] = z11 - z4;
    
        dataptr += DCTSIZE;     /* advance pointer to next row */
    }
    
    /* Pass 2: process columns. */
    
    dataptr = data;
    for (ctr = DCTSIZE-1; ctr >= 0; ctr--)
    {
        tmp0 = dataptr[DCTSIZE*0] + dataptr[DCTSIZE*7];
        tmp7 = dataptr[DCTSIZE*0] - dataptr[DCTSIZE*7];
        tmp1 = dataptr[DCTSIZE*1] + dataptr[DCTSIZE*6];
        tmp6 = dataptr[DCTSIZE*1] - dataptr[DCTSIZE*6];
        tmp2 = dataptr[DCTSIZE*2] + dataptr[DCTSIZE*5];
        tmp5 = dataptr[DCTSIZE*2] - dataptr[DCTSIZE*5];
        tmp3 = dataptr[DCTSIZE*3] + dataptr[DCTSIZE*4];
        tmp4 = dataptr[DCTSIZE*3] - dataptr[DCTSIZE*4];
        
        /* Even part */
        
        tmp10 = tmp0 + tmp3;    /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;
        
        dataptr[DCTSIZE*0] = tmp10 + tmp11; /* phase 3 */
        dataptr[DCTSIZE*4] = tmp10 - tmp11;
        
        z1 = MULTIPLY(tmp12 + tmp13, FIX_0_707106781); /* c4 */
        dataptr[DCTSIZE*2] = tmp13 + z1; /* phase 5 */
        dataptr[DCTSIZE*6] = tmp13 - z1;
        
        /* Odd part */
    
        tmp10 = tmp4 + tmp5;    /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;
    
        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = MULTIPLY(tmp10 - tmp12, FIX_0_382683433); /* c6 */
        z2 = MULTIPLY(tmp10, FIX_0_541196100) + z5; /* c2-c6 */
        z4 = MULTIPLY(tmp12, FIX_1_306562965) + z5; /* c2+c6 */
        z3 = MULTIPLY(tmp11, FIX_0_707106781); /* c4 */
    
        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;
    
        dataptr[DCTSIZE*5] = z13 + z2; /* phase 6 */
        dataptr[DCTSIZE*3] = z13 - z2;
        dataptr[DCTSIZE*1] = z11 + z4;
        dataptr[DCTSIZE*7] = z11 - z4;
    
        dataptr++;          /* advance pointer to next column */
    }

    int i;
    for (i = 0; i < DCTSIZE2; i++)
    {
        int qval = divisors[i];
        int temp = data[i];
        /* Divide the coefficient value by qval, ensuring proper rounding.
         * Since C does not specify the direction of rounding for negative
         * quotients, we have to force the dividend positive for portability.
         *
         * In most files, at least half of the output values will be zero
         * (at default quantization settings, more like three-quarters...)
         * so we should ensure that this case is fast.  On many machines,
         * a comparison is enough cheaper than a divide to make a special test
         * a win.  Since both inputs will be nonnegative, we need only test
         * for a < b to discover whether a/b is 0.
         * If your machine's division is fast enough, define FAST_DIVIDE.
         */
#ifdef FAST_DIVIDE
#define DIVIDE_BY(a,b)  a /= b
#else
#define DIVIDE_BY(a,b)  if (a >= b) a /= b; else a = 0
#endif
        if (temp < 0)
        {
            temp = -temp;
            temp += qval>>1;      /* for rounding */
            DIVIDE_BY(temp, qval);
            temp = -temp;
        } else {
            temp += qval>>1;      /* for rounding */
            DIVIDE_BY(temp, qval);
        }
        out[zigzag_order[i]] = temp;
    }
}

void write_coeffs(int *zz, int c, const codeword_t *dc_codes, const codeword_t *ac_codes)
{
    // Huffman encode the DC coefficient
    int diff = zz[0] - dc_pred[c];
    dc_pred[c] = zz[0];
    int s = magnitude_category(diff);
    
    // Emit the DC Huffman code for SSSS
    write_code(&dc_codes[s]);
    
    // Emit the value for SSSS
    int v = diff;
    if (v < 0)
    {
        v--;
        v &= (1 << s) - 1;
    }
    
    write_bits(v, s);
    
    // Huffman encode the AC coefficients: figure F.2
    int r = 0, k;
    for (k = 1; k < 64; ++k)
    {
        if (zz[k] == 0)
        {
            if (k == 63)
            {
                // End of block
                write_code(&ac_codes[0x00]);
            } else {
                // Skipping one more zero
                ++r;
            }
        } else {
            // Non-zero coefficient
            
            // Write a ZRL code for every 16 zeros that were passed
            // (since we can't encode a run length longer than 16)
            while (r > 15)
            {
                write_code(&ac_codes[0xf0]);
                
                // Code 0xf0 is equivalent to 16 zeros.
                r -= 16;
            }
            
            // Encode_R,ZZ(K): figure F.3
            
            // Write RS code
            int s = magnitude_category(zz[k]);
            int rs = r * 16 + s;
            write_code(&ac_codes[rs]);
            
            // Write the value
            int v = zz[k];
            if (v < 0)
            {
                v--;
                v &= (1 << s) - 1;
            }
            write_bits(v, s);
            
            r = 0;
        }
    }
}

void write_scan_color()
{
    int data[64];
    int zz[64];
    int x0, y0, y;
    
    // The image format is VYUY with one component per byte.
    
    uint8_t *block_start = img;
    for (y0 = 0; y0 < HEIGHT_SIZE; y0 += 8)
    {
        for (x0 = 0; x0 < WIDTH_SIZE / 2; x0 += 16)
        {
            // Y0
            int32_t *dataptr = data;
            uint8_t *imageptr = block_start + 1;
            for (y = y0; y < y0 + 8; ++y)
            {
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                
                imageptr += WIDTH_SIZE - 16;
            }
            
            fast_dct(data, zz, lum_divisors);
            write_coeffs(zz, 0, lum_dc_codes, lum_ac_codes);
            
            // Y1
            dataptr = data;
            imageptr = block_start + 16;
            for (y = y0; y < y0 + 8; ++y)
            {
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                imageptr++;
                *dataptr++ = *imageptr++ - 128;
                
                imageptr += WIDTH_SIZE - 16;
            }
            
            fast_dct(data, zz, lum_divisors);
            write_coeffs(zz, 0, lum_dc_codes, lum_ac_codes);
            
            // U
            dataptr = data;
            imageptr = block_start + 2;
            for (y = y0; y < y0 + 8; ++y)
            {
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                
                imageptr += WIDTH_SIZE - 32;
            }
            
            fast_dct(data, zz, chrom_divisors);
            write_coeffs(zz, 1, chrom_dc_codes, chrom_ac_codes);
            
            // V
            dataptr = data;
            imageptr = block_start;
            for (y = y0; y < y0 + 8; ++y)
            {
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                *dataptr++ = *imageptr - 128;
                imageptr += 4;
                
                imageptr += WIDTH_SIZE - 32;
            }
            
            fast_dct(data, zz, chrom_divisors);
            write_coeffs(zz, 2, chrom_dc_codes, chrom_ac_codes);
            
            block_start += 32;
        }
        block_start += WIDTH_SIZE * 7;
    }
}

void write_scan_gray()
{
    int data[64];
    int zz[64];
    int x0, y0, y;
    
    // The image format is VYUY with one component per byte.
    
    uint8_t *block_start = img;
    for (y0 = 0; y0 < HEIGHT_SIZE; y0 += 8)
    {
        for (x0 = 0; x0 < WIDTH_SIZE / 2; x0 += 8)
        {
            int32_t *dataptr = data;
            uint8_t *imageptr = block_start + 1;
            for (y = y0; y < y0 + 8; ++y)
            {
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                *dataptr++ = *imageptr - 128;
                imageptr += 2;
                
                imageptr += WIDTH_SIZE - 16;
            }
            
            fast_dct(data, zz, lum_divisors);
            write_coeffs(zz, 0, lum_dc_codes, lum_ac_codes);
            
            block_start += 16;
        }
        block_start += WIDTH_SIZE * 7;
    }
}

void jpeg_compress(int color)
{
    dc_pred[0] = 0;
    dc_pred[1] = 0;
    dc_pred[2] = 0;
    
    cur_word = 0;
    bits_left = 32;
    
    // Compress the image
    if (color)
    {
        write_scan_color();
    } else {
        write_scan_gray();
    }
    
    // Finish the last byte
    byte_align();
    
    if (bits_left < 32)
        safe_putch((cur_word >> 24) & 0xff);
    if (bits_left < 24)
        safe_putch((cur_word >> 16) & 0xff);
    if (bits_left < 16)
        safe_putch((cur_word >> 8) & 0xff);
    if (bits_left < 8)
        safe_putch(cur_word & 0xff);
    
    putch(0xff);
    putch(0xd9);
}
