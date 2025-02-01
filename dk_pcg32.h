/**
 * \file dk_pcg32.h - v0.1
 * \author KOH Swee Teck Dedrick
 * \brief
 *      A PCG random number generator for C and C++.
 * 
 *      This library (dk_pcg32) makes use of concepts from the PCG Random
 *      Number generator for implementation with a C interface. It provides
 *      additional functions for common random number generation operations
 *      in a real-time application.
 * 
 *      The PCG random number generator was developed by Melissa O'Neill
 *      <oneill@pcg-random.org>.
 * 
 *      For additional information about the PCG random number generation
 *      scheme, including its license and other licensing options, visit
 *      http://www.pcg-random.org.
 * 
 *  LICENSE
 *      License information at the end of the header.
 * 
 *  USAGE
 *      To use this library in your project, add the following to ONE C/C++
 *      file to create the implementation.
 * 
 *      // i.e. it should look like this:
 *      #include ...
 *      #include ...
 *      #define DK_PCG32_IMPLEMENTATION
 *      #include "dk_pcg32.h"
 */

#ifndef DK_INCLUDE_DK_PCG32_H
#define DK_INCLUDE_DK_PCG32_H

#if __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#   if __STDC_VERSION__ < 202311L // C23
#       include <stdbool.h>
#   endif
#endif

#include <stdint.h>

typedef struct dk_pcg32 dk_pcg32;

/**
 * \brief Seeds the random number generator. You should call this
 * once for each instance of a random number generator you wish to create.
 * 
 * \code
 * dk_pcg32 rand;
 * dk_pcg32_seed(&rand, time(NULL));
 * \endcode
 * 
 * \param[in, out] pcg Pointer to dk_pcg32 structure to seed.
 * \param[in] seed Seed value.
 */
void dk_pcg32_seed(dk_pcg32 *pcg, uint64_t seed);

/**
 * \brief Gets a random boolean value, either true or false.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \return Boolean value, either true or false.
 */
bool dk_pcg32_get_bool(dk_pcg32 *pcg);

/**
 * \brief Gets a random uniformly distributed unsigned 32-bit integer.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \return Uniformly distributed unsigned 32-bit random integer.
 */
uint32_t dk_pcg32_get_u32(dk_pcg32 *pcg);

/**
 * \brief Gets a random uniformly distributed unsigned 64-bit integer.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \return Uniformly distributed unsigned 64-bit random integer.
 */
uint64_t dk_pcg32_get_u64(dk_pcg32 *pcg);

/**
 * \brief Gets a random single precision float on [0..1) interval.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \return Random single precision float on [0..1) interval.
 */
float dk_pcg32_get_f32(dk_pcg32 *pcg);

/**
 * \brief Gets a random double precision float on [0..1) interval.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \return Random double precision float on [0..1) interval.
 */
double dk_pcg32_get_f64(dk_pcg32 *pcg);

/**
 * \brief Gets a random unsigned 32-bit integer on [min..max) interval.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \param[in] min Inclusive start of interval.
 * \param[in] max Exclusive end of interval.
 * \return Unsigned 32-bit integer on [min..max) interval.
 */
uint32_t dk_pcg32_get_range_u32(dk_pcg32 *pcg, uint32_t min, uint32_t max);

/**
 * \brief Gets a random signed 32-bit integer on [min..max) interval.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \param[in] min Inclusive start of interval.
 * \param[in] max Exclusive end of interval.
 * \return Signed 32-bit integer on [min..max) interval.
 */
int32_t dk_pcg32_get_range_i32(dk_pcg32 *pcg, int32_t min, int32_t max);

/**
 * \brief Gets a random single precision float on [min..max) interval.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \param[in] min Inclusive start of interval.
 * \param[in] max Exclusive end of interval.
 * \return Single precision float on [min..max) interval.
 */
float dk_pcg32_get_range_f32(dk_pcg32 *pcg, float min, float max);

/**
 * \brief Gets a random double precision float on [min..max) interval.
 * \param[in, out] pcg Pointer to dk_pcg32 context.
 * \param[in] min Inclusive start of interval.
 * \param[in] max Exclusive end of interval.
 * \return Double precision float on [min..max) interval.
 */
double dk_pcg32_get_range_f64(dk_pcg32 *pcg, double min, double max);

// NOTE(Dedrick): This is an opaque structure that you shouldn't mess with
// because it holds the state of the random number generator. It's defined
// here to allow you to construct it on the stack.
struct dk_pcg32 {
    uint64_t state;
};

#if __cplusplus
}
#endif

#ifdef DK_PCG32_IMPLEMENTATION

#define DK_PCG32_MUL 0x5851F42D4C957F2Dull
#define DK_PCG32_INC 0x14057B7EF767814Full

void dk_pcg32_seed(dk_pcg32 *pcg, uint64_t seed) {
    pcg->state = 0ull;
    dk_pcg32_get_u32(pcg);
    pcg->state += seed;
    dk_pcg32_get_u32(pcg);
}

bool dk_pcg32_get_bool(dk_pcg32 *pcg) {
    // NOTE(Dedrick): Split the interval [0, 2^32) in half and test that.
    return dk_pcg32_get_u32(pcg) < 0xFFFFFFFF / 2;
}

uint32_t dk_pcg32_get_u32(dk_pcg32 *pcg) {
    uint64_t const state = pcg->state;
    pcg->state = state * DK_PCG32_MUL + DK_PCG32_INC;

    // NOTE(Dedrick): Use XSH-RR variant.
    uint32_t const value = (uint32_t)((state ^ (state >> 18)) >> 27);
    int32_t const rot = (int32_t)(state >> 59);
    return (value >> rot) | (value << ((-rot + 1u) & 31));
}

uint64_t dk_pcg32_get_u64(dk_pcg32 *pcg) {
    uint64_t const hi = dk_pcg32_get_u32(pcg);
    uint64_t const lo = dk_pcg32_get_u32(pcg);
    return hi << 32 | lo;
}

float dk_pcg32_get_f32(dk_pcg32 *pcg) {
    return (float)(dk_pcg32_get_u32(pcg) >> 8) * (1.0f / (1u << 24));
}

double dk_pcg32_get_f64(dk_pcg32 *pcg) {
    return (double)(dk_pcg32_get_u64(pcg) >> 11) * (1.0f / (1ull << 53));
}

uint32_t dk_pcg32_get_range_u32(dk_pcg32 *pcg, uint32_t min, uint32_t max) {
    uint32_t const bounds = max - min;
    uint32_t const threshold = -(int32_t)bounds % bounds;
    while (1) {
        uint32_t const r = dk_pcg32_get_u32(pcg);
        if (r >= threshold) {
            return min + r % bounds;
        }
    }
}

int32_t dk_pcg32_get_range_i32(dk_pcg32 *pcg, int32_t min, int32_t max) {
    if (min != max) {
        int32_t const range = min - max;
        uint32_t const abs_range = (range < 0 ? -range : range) + 1u;
        uint32_t const range_min = min < max ? min : max;
        return (int32_t)dk_pcg32_get_range_u32(pcg, 0, abs_range + range_min);
    }
    return min;
}

float dk_pcg32_get_range_f32(dk_pcg32 *pcg, float min, float max) {
    return dk_pcg32_get_f32(pcg) * (max - min) + min;
}

double dk_pcg32_get_range_f64(dk_pcg32 *pcg, double min, double max) {
    return dk_pcg32_get_f64(pcg) * (max - min) + min;
}

#undef DK_PCG32_INC
#undef DK_PCG32_MUL

#endif // DK_PCG32_IMPLEMENTATION

/**
 * This software is available under the Apache 2.0 license
 * ---
 * Copyright 2025 KOH Swee Teck Dedrick
 * Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#endif // DK_INCLUDE_DK_PCG32_H
