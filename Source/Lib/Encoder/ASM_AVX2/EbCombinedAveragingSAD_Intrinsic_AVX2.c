/*!< Copyright(c) 2019 Intel Corporation
 * SPDX - License - Identifier: BSD - 2 - Clause - Patent */

#include "immintrin.h"
#include "EbCombinedAveragingSAD_Inline_AVX2.h"
#include "EbCombinedAveragingSAD_Intrinsic_AVX2.h"
#include "EbMemory_AVX2.h"
#include "EbMemory_SSE4_1.h"
#include "EbComputeSAD_SSE2.h"

uint32_t combined_averaging_8xm_sad_avx2_intrin(uint8_t *src, uint32_t src_stride, uint8_t *ref1,
                                                uint32_t ref1_stride, uint8_t *ref2,
                                                uint32_t ref2_stride, uint32_t height,
                                                uint32_t width) {
    __m256i  sum = _mm256_setzero_si256();
    __m128i  sad;
    uint32_t y = height;
    (void)width;

    do {
        const __m256i s   = load_u8_8x4_avx2(src, src_stride);
        const __m256i r1  = load_u8_8x4_avx2(ref1, ref1_stride);
        const __m256i r2  = load_u8_8x4_avx2(ref2, ref2_stride);
        const __m256i avg = _mm256_avg_epu8(r1, r2);
        const __m256i sad = _mm256_sad_epu8(s, avg);
        sum               = _mm256_add_epi32(sum, sad);
        src += src_stride << 2;
        ref1 += ref1_stride << 2;
        ref2 += ref2_stride << 2;
        y -= 4;
    } while (y);

    sad = _mm_add_epi32(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
    sad = _mm_add_epi32(sad, _mm_srli_si128(sad, 8));

    return _mm_cvtsi128_si32(sad);
}

static INLINE __m256i combined_averaging_sad16x2_avx2(
    const uint8_t *const src, const uint32_t src_stride, const uint8_t *const ref1,
    const uint32_t ref1_stride, const uint8_t *const ref2, const uint32_t ref2_stride,
    const __m256i sum) {
    const __m256i s   = loadu_u8_16x2_avx2(src, src_stride);
    const __m256i r1  = loadu_u8_16x2_avx2(ref1, ref1_stride);
    const __m256i r2  = loadu_u8_16x2_avx2(ref2, ref2_stride);
    const __m256i avg = _mm256_avg_epu8(r1, r2);
    const __m256i sad = _mm256_sad_epu8(s, avg);
    return _mm256_add_epi32(sum, sad);
}

uint32_t combined_averaging_16xm_sad_avx2_intrin(uint8_t *src, uint32_t src_stride, uint8_t *ref1,
                                                 uint32_t ref1_stride, uint8_t *ref2,
                                                 uint32_t ref2_stride, uint32_t height,
                                                 uint32_t width) {
    __m256i  sum = _mm256_setzero_si256();
    __m128i  sad;
    uint32_t y = height;
    (void)width;

    do {
        sum = combined_averaging_sad16x2_avx2(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, sum);
        src += src_stride << 1;
        ref1 += ref1_stride << 1;
        ref2 += ref2_stride << 1;
        y -= 2;
    } while (y);

    sad = _mm_add_epi32(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
    sad = _mm_add_epi32(sad, _mm_srli_si128(sad, 8));

    return _mm_cvtsi128_si32(sad);
}

static INLINE __m256i combined_averaging_sad24_avx2(const uint8_t *const src,
                                                    const uint8_t *const ref1,
                                                    const uint8_t *const ref2, const __m256i sum) {
    const __m256i s   = _mm256_loadu_si256((__m256i *)src);
    const __m256i r1  = _mm256_loadu_si256((__m256i *)ref1);
    const __m256i r2  = _mm256_loadu_si256((__m256i *)ref2);
    const __m256i avg = _mm256_avg_epu8(r1, r2);
    const __m256i sad = _mm256_sad_epu8(s, avg);
    return _mm256_add_epi32(sum, sad);
}

uint32_t combined_averaging_24xm_sad_avx2_intrin(uint8_t *src, uint32_t src_stride, uint8_t *ref1,
                                                 uint32_t ref1_stride, uint8_t *ref2,
                                                 uint32_t ref2_stride, uint32_t height,
                                                 uint32_t width) {
    __m256i  sum = _mm256_setzero_si256();
    __m128i  sad;
    uint32_t y = height;
    (void)width;

    do {
        sum = combined_averaging_sad24_avx2(
            src + 0 * src_stride, ref1 + 0 * ref1_stride, ref2 + 0 * ref2_stride, sum);
        sum = combined_averaging_sad24_avx2(
            src + 1 * src_stride, ref1 + 1 * ref1_stride, ref2 + 1 * ref2_stride, sum);
        src += src_stride << 1;
        ref1 += ref1_stride << 1;
        ref2 += ref2_stride << 1;
        y -= 2;
    } while (y);

    sad = _mm_add_epi32(_mm256_castsi256_si128(sum),
                        _mm_slli_si128(_mm256_extracti128_si256(sum, 1), 8));
    sad = _mm_add_epi32(sad, _mm_srli_si128(sad, 8));

    return _mm_cvtsi128_si32(sad);
}

static INLINE __m256i combined_averaging_sad32_avx2(const uint8_t *const src,
                                                    const uint8_t *const ref1,
                                                    const uint8_t *const ref2, const __m256i sum) {
    const __m256i s   = _mm256_loadu_si256((__m256i *)src);
    const __m256i r1  = _mm256_loadu_si256((__m256i *)ref1);
    const __m256i r2  = _mm256_loadu_si256((__m256i *)ref2);
    const __m256i avg = _mm256_avg_epu8(r1, r2);
    const __m256i sad = _mm256_sad_epu8(s, avg);
    return _mm256_add_epi32(sum, sad);
}

uint32_t combined_averaging_32xm_sad_avx2_intrin(uint8_t *src, uint32_t src_stride, uint8_t *ref1,
                                                 uint32_t ref1_stride, uint8_t *ref2,
                                                 uint32_t ref2_stride, uint32_t height,
                                                 uint32_t width) {
    __m256i  sum = _mm256_setzero_si256();
    __m128i  sad;
    uint32_t y = height;
    (void)width;

    do {
        sum = combined_averaging_sad32_avx2(
            src + 0 * src_stride, ref1 + 0 * ref1_stride, ref2 + 0 * ref2_stride, sum);
        sum = combined_averaging_sad32_avx2(
            src + 1 * src_stride, ref1 + 1 * ref1_stride, ref2 + 1 * ref2_stride, sum);
        src += src_stride << 1;
        ref1 += ref1_stride << 1;
        ref2 += ref2_stride << 1;
        y -= 2;
    } while (y);

    sad = _mm_add_epi32(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
    sad = _mm_add_epi32(sad, _mm_srli_si128(sad, 8));

    return _mm_cvtsi128_si32(sad);
}

uint32_t combined_averaging_48xm_sad_avx2_intrin(uint8_t *src, uint32_t src_stride, uint8_t *ref1,
                                                 uint32_t ref1_stride, uint8_t *ref2,
                                                 uint32_t ref2_stride, uint32_t height,
                                                 uint32_t width) {
    __m256i  sum = _mm256_setzero_si256();
    __m128i  sad;
    uint32_t y = height;
    (void)width;

    do {
        sum = combined_averaging_sad32_avx2(
            src + 0 * src_stride, ref1 + 0 * ref1_stride, ref2 + 0 * ref2_stride, sum);
        sum = combined_averaging_sad32_avx2(
            src + 1 * src_stride, ref1 + 1 * ref1_stride, ref2 + 1 * ref2_stride, sum);
        sum = combined_averaging_sad16x2_avx2(
            src + 32, src_stride, ref1 + 32, ref1_stride, ref2 + 32, ref2_stride, sum);

        src += src_stride << 1;
        ref1 += ref1_stride << 1;
        ref2 += ref2_stride << 1;
        y -= 2;
    } while (y);

    sad = _mm_add_epi32(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
    sad = _mm_add_epi32(sad, _mm_srli_si128(sad, 8));

    return _mm_cvtsi128_si32(sad);
}

uint32_t combined_averaging_64xm_sad_avx2_intrin(uint8_t *src, uint32_t src_stride, uint8_t *ref1,
                                                 uint32_t ref1_stride, uint8_t *ref2,
                                                 uint32_t ref2_stride, uint32_t height,
                                                 uint32_t width) {
    __m256i  sum = _mm256_setzero_si256();
    __m128i  sad;
    uint32_t y = height;
    (void)width;

    do {
        sum = combined_averaging_sad32_avx2(src + 0x00, ref1 + 0x00, ref2 + 0x00, sum);
        sum = combined_averaging_sad32_avx2(src + 0x20, ref1 + 0x20, ref2 + 0x20, sum);
        src += src_stride;
        ref1 += ref1_stride;
        ref2 += ref2_stride;
    } while (--y);

    sad = _mm_add_epi32(_mm256_castsi256_si128(sum), _mm256_extracti128_si256(sum, 1));
    sad = _mm_add_epi32(sad, _mm_srli_si128(sad, 8));

    return _mm_cvtsi128_si32(sad);
}
uint64_t compute_mean8x8_avx2_intrin(
    uint8_t *input_samples, /*!< input parameter, input samples Ptr */
    uint32_t input_stride, /*!< input parameter, input stride */
    uint32_t input_area_width, /*!< input parameter, input area width */
    uint32_t input_area_height) /*!< input parameter, input area height */
{
    __m256i  sum, sum2, xmm2, xmm1, sum1, xmm0 = _mm256_setzero_si256();
    __m128i  upper, lower, mean = _mm_setzero_si128();
    uint64_t result;
    xmm1 =
        _mm256_sad_epu8(xmm0,
                        _mm256_set_m128i(_mm_loadl_epi64((__m128i *)(input_samples + input_stride)),
                                         _mm_loadl_epi64((__m128i *)(input_samples))));
    xmm2 = _mm256_sad_epu8(
        xmm0,
        _mm256_set_m128i(_mm_loadl_epi64((__m128i *)(input_samples + 3 * input_stride)),
                         _mm_loadl_epi64((__m128i *)(input_samples + 2 * input_stride))));
    sum1 = _mm256_add_epi16(xmm1, xmm2);

    input_samples += 4 * input_stride;

    xmm1 =
        _mm256_sad_epu8(xmm0,
                        _mm256_set_m128i(_mm_loadl_epi64((__m128i *)(input_samples + input_stride)),
                                         _mm_loadl_epi64((__m128i *)(input_samples))));
    xmm2 = _mm256_sad_epu8(
        xmm0,
        _mm256_set_m128i(_mm_loadl_epi64((__m128i *)(input_samples + 3 * input_stride)),
                         _mm_loadl_epi64((__m128i *)(input_samples + 2 * input_stride))));
    sum2 = _mm256_add_epi16(xmm1, xmm2);

    sum   = _mm256_add_epi16(sum1, sum2);
    upper = _mm256_extractf128_si256(sum, 1); /*!< extract upper 128 bit */
    upper =
        _mm_add_epi32(upper, _mm_srli_si128(upper, 8)); /*!< shift 2nd 16 bits to the 1st and sum both */

    lower = _mm256_extractf128_si256(sum, 0); /*!<extract lower 128 bit */
    lower =
        _mm_add_epi32(lower, _mm_srli_si128(lower, 8)); /*!< shift 2nd 16 bits to the 1st and sum both */

    mean = _mm_add_epi32(lower, upper);

    (void)input_area_width;
    (void)input_area_height;

    result = (uint64_t)_mm_cvtsi128_si32(mean) << 2;
    return result;
}

/********************************************************************************************************************************/
void compute_interm_var_four8x8_avx2_intrin(uint8_t *input_samples, uint16_t input_stride,
                                            uint64_t *mean_of8x8_blocks, /*!< mean of four  8x8 */
                                            uint64_t *mean_of_squared8x8_blocks) /*!< meanSquared */
{
    __m256i ymm1, ymm2, ymm3, ymm4, ymm_sum1, ymm_sum2, ymm_final_sum, ymm_shift,
        /* ymm_blockMeanSquared*/ //,
        ymm_in, ymm_in_2s, ymm_in_second, ymm_in_2s_second, ymm_shift_squared, ymm_permute8,
        ymm_result, ymm_block_mean_squared_low, ymm_block_mean_squared_high, ymm_inputlo,
        ymm_inputhi;

    __m128i ymm_block_mean_squared_lo, ymm_block_mean_squared_hi, ymm_resultlo, ymm_resulthi;

    __m256i ymm_zero = _mm256_setzero_si256();
    __m128i xmm_zero = _mm_setzero_si128();

    ymm_in    = _mm256_loadu_si256((__m256i *)input_samples);
    ymm_in_2s = _mm256_loadu_si256((__m256i *)(input_samples + 2 * input_stride));

    ymm1 = _mm256_sad_epu8(ymm_in, ymm_zero);
    ymm2 = _mm256_sad_epu8(ymm_in_2s, ymm_zero);

    ymm_sum1 = _mm256_add_epi16(ymm1, ymm2);

    input_samples += 4 * input_stride;
    ymm_in_second    = _mm256_loadu_si256((__m256i *)input_samples);
    ymm_in_2s_second = _mm256_loadu_si256((__m256i *)(input_samples + 2 * input_stride));

    ymm3 = _mm256_sad_epu8(ymm_in_second, ymm_zero);
    ymm4 = _mm256_sad_epu8(ymm_in_2s_second, ymm_zero);

    ymm_sum2 = _mm256_add_epi16(ymm3, ymm4);

    ymm_final_sum = _mm256_add_epi16(ymm_sum1, ymm_sum2);

    ymm_shift     = _mm256_set_epi64x(3, 3, 3, 3);
    ymm_final_sum = _mm256_sllv_epi64(ymm_final_sum, ymm_shift);

    _mm256_storeu_si256((__m256i *)(mean_of8x8_blocks), ymm_final_sum);

    /*******************************Squared Mean******************************/

    ymm_inputlo = _mm256_unpacklo_epi8(ymm_in, ymm_zero);
    ymm_inputhi = _mm256_unpackhi_epi8(ymm_in, ymm_zero);

    ymm_block_mean_squared_low  = _mm256_madd_epi16(ymm_inputlo, ymm_inputlo);
    ymm_block_mean_squared_high = _mm256_madd_epi16(ymm_inputhi, ymm_inputhi);

    ymm_inputlo = _mm256_unpacklo_epi8(ymm_in_2s, ymm_zero);
    ymm_inputhi = _mm256_unpackhi_epi8(ymm_in_2s, ymm_zero);

    ymm_block_mean_squared_low =
        _mm256_add_epi32(ymm_block_mean_squared_low, _mm256_madd_epi16(ymm_inputlo, ymm_inputlo));
    ymm_block_mean_squared_high =
        _mm256_add_epi32(ymm_block_mean_squared_high, _mm256_madd_epi16(ymm_inputhi, ymm_inputhi));

    ymm_inputlo = _mm256_unpacklo_epi8(ymm_in_second, ymm_zero);
    ymm_inputhi = _mm256_unpackhi_epi8(ymm_in_second, ymm_zero);

    ymm_block_mean_squared_low =
        _mm256_add_epi32(ymm_block_mean_squared_low, _mm256_madd_epi16(ymm_inputlo, ymm_inputlo));
    ymm_block_mean_squared_high =
        _mm256_add_epi32(ymm_block_mean_squared_high, _mm256_madd_epi16(ymm_inputhi, ymm_inputhi));

    ymm_inputlo = _mm256_unpacklo_epi8(ymm_in_2s_second, ymm_zero);
    ymm_inputhi = _mm256_unpackhi_epi8(ymm_in_2s_second, ymm_zero);

    ymm_block_mean_squared_low =
        _mm256_add_epi32(ymm_block_mean_squared_low, _mm256_madd_epi16(ymm_inputlo, ymm_inputlo));
    ymm_block_mean_squared_high =
        _mm256_add_epi32(ymm_block_mean_squared_high, _mm256_madd_epi16(ymm_inputhi, ymm_inputhi));

    ymm_block_mean_squared_low  = _mm256_add_epi32(ymm_block_mean_squared_low,
                                                  _mm256_srli_si256(ymm_block_mean_squared_low, 8));
    ymm_block_mean_squared_high = _mm256_add_epi32(
        ymm_block_mean_squared_high, _mm256_srli_si256(ymm_block_mean_squared_high, 8));

    ymm_block_mean_squared_low  = _mm256_add_epi32(ymm_block_mean_squared_low,
                                                  _mm256_srli_si256(ymm_block_mean_squared_low, 4));
    ymm_block_mean_squared_high = _mm256_add_epi32(
        ymm_block_mean_squared_high, _mm256_srli_si256(ymm_block_mean_squared_high, 4));

    ymm_permute8 = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 4, 0);
    ymm_block_mean_squared_low =
        _mm256_permutevar8x32_epi32(ymm_block_mean_squared_low, ymm_permute8 /*!< 8 */);
    ymm_block_mean_squared_high =
        _mm256_permutevar8x32_epi32(ymm_block_mean_squared_high, ymm_permute8);

    ymm_block_mean_squared_lo = _mm256_castsi256_si128(ymm_block_mean_squared_low); /*!< lower 128 */
    ymm_block_mean_squared_hi =
        _mm256_extracti128_si256(ymm_block_mean_squared_high, 0); /*!< lower 128 */

    ymm_result   = _mm256_unpacklo_epi32(_mm256_castsi128_si256(ymm_block_mean_squared_lo),
                                       _mm256_castsi128_si256(ymm_block_mean_squared_hi));
    ymm_resultlo = _mm_unpacklo_epi64(_mm256_castsi256_si128(ymm_result), xmm_zero);
    ymm_resulthi = _mm_unpackhi_epi64(_mm256_castsi256_si128(ymm_result), xmm_zero);

    ymm_result = _mm256_set_m128i(ymm_resulthi, ymm_resultlo);

    ymm_permute8 = _mm256_set_epi32(7, 5, 6, 4, 3, 1, 2, 0);
    ymm_result   = _mm256_permutevar8x32_epi32(ymm_result, ymm_permute8);

    ymm_shift_squared = _mm256_set1_epi64x(11);

    ymm_result = _mm256_sllv_epi64(ymm_result, ymm_shift_squared);

    _mm256_storeu_si256((__m256i *)(mean_of_squared8x8_blocks), ymm_result);
}

uint32_t combined_averaging_ssd_avx2(uint8_t *src, ptrdiff_t src_stride, uint8_t *ref1,
                                     ptrdiff_t ref1_stride, uint8_t *ref2, ptrdiff_t ref2_stride,
                                     uint32_t height, uint32_t width) {
    uint32_t y = height;
    __m128i  sum_128;

    if (width & 4) {
        const __m128i zero = _mm_setzero_si128();

        sum_128 = _mm_setzero_si128();

        do {
            uint32_t x = 0;
            do {
                const __m128i s     = load_u8_4x2_sse4_1(src + x, src_stride);
                const __m128i r1    = load_u8_4x2_sse4_1(ref1 + x, ref1_stride);
                const __m128i r2    = load_u8_4x2_sse4_1(ref2 + x, ref2_stride);
                const __m128i avg   = _mm_avg_epu8(r1, r2);
                const __m128i s16   = _mm_unpacklo_epi8(s, zero);
                const __m128i avg16 = _mm_unpacklo_epi8(avg, zero);
                const __m128i dif   = _mm_sub_epi16(s16, avg16);
                const __m128i sqr   = _mm_madd_epi16(dif, dif);
                sum_128             = _mm_add_epi32(sum_128, sqr);
                x += 4;
            } while (x < width);

            src += 2 * src_stride;
            ref1 += 2 * ref1_stride;
            ref2 += 2 * ref2_stride;
            y -= 2;
        } while (y);
    } else {
        __m256i sum = _mm256_setzero_si256();

        if (width == 8) {
            do {
                ssd8x2_avx2(src, src_stride, ref1, ref1_stride, ref2, ref2_stride, &sum);
                src += 2 * src_stride;
                ref1 += 2 * ref1_stride;
                ref2 += 2 * ref2_stride;
                y -= 2;
            } while (y);
        } else if (width == 16) {
            do {
                const __m128i s       = _mm_loadu_si128((__m128i *)src);
                const __m128i r1      = _mm_loadu_si128((__m128i *)ref1);
                const __m128i r2      = _mm_loadu_si128((__m128i *)ref2);
                const __m128i avg     = _mm_avg_epu8(r1, r2);
                const __m256i s_256   = _mm256_cvtepu8_epi16(s);
                const __m256i avg_256 = _mm256_cvtepu8_epi16(avg);
                const __m256i dif     = _mm256_sub_epi16(s_256, avg_256);
                const __m256i sqr     = _mm256_madd_epi16(dif, dif);
                sum                   = _mm256_add_epi32(sum, sqr);

                src += src_stride;
                ref1 += ref1_stride;
                ref2 += ref2_stride;
            } while (--y);
        } else if (width == 32) {
            do {
                ssd32_avx2(src, ref1, ref2, &sum);
                src += src_stride;
                ref1 += ref1_stride;
                ref2 += ref2_stride;
            } while (--y);
        } else if (width == 64) {
            do {
                ssd32_avx2(src + 0 * 32, ref1 + 0 * 32, ref2 + 0 * 32, &sum);
                ssd32_avx2(src + 1 * 32, ref1 + 1 * 32, ref2 + 1 * 32, &sum);
                src += src_stride;
                ref1 += ref1_stride;
                ref2 += ref2_stride;
            } while (--y);
        } else {
            do {
                uint32_t x = 0;
                do {
                    ssd8x2_avx2(
                        src + x, src_stride, ref1 + x, ref1_stride, ref2 + x, ref2_stride, &sum);
                    x += 8;
                } while (x < width);

                src += 2 * src_stride;
                ref1 += 2 * ref1_stride;
                ref2 += 2 * ref2_stride;
                y -= 2;
            } while (y);
        }

        const __m128i sum0_128 = _mm256_castsi256_si128(sum);
        const __m128i sum1_128 = _mm256_extracti128_si256(sum, 1);
        sum_128                = _mm_add_epi32(sum0_128, sum1_128);
    }

    sum_128 = _mm_add_epi32(sum_128, _mm_srli_si128(sum_128, 8));
    sum_128 = _mm_add_epi32(sum_128, _mm_srli_si128(sum_128, 4));
    return _mm_cvtsi128_si32(sum_128);
}

uint32_t nxm_sad_avg_kernel_helper_avx2(uint8_t *src, uint32_t src_stride, uint8_t *ref1,
                                        uint32_t ref1_stride, uint8_t *ref2, uint32_t ref2_stride,
                                        uint32_t height, uint32_t width) {
    uint32_t nxm_sad_avg = 0;

    switch (width) {
    case 4:
        nxm_sad_avg = combined_averaging_4xm_sad_sse2_intrin(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, height, width);
        break;
    case 8:
        nxm_sad_avg = combined_averaging_8xm_sad_avx2_intrin(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, height, width);
        break;
    case 16:
        nxm_sad_avg = combined_averaging_16xm_sad_avx2_intrin(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, height, width);
        break;
    case 24:
        nxm_sad_avg = combined_averaging_24xm_sad_avx2_intrin(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, height, width);
        break;
    case 32:
        nxm_sad_avg = combined_averaging_32xm_sad_avx2_intrin(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, height, width);
        break;
    case 48:
        nxm_sad_avg = combined_averaging_48xm_sad_avx2_intrin(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, height, width);
        break;
    case 64:
        nxm_sad_avg = combined_averaging_64xm_sad_avx2_intrin(
            src, src_stride, ref1, ref1_stride, ref2, ref2_stride, height, width);
        break;
    case 40:
    case 56: break; //void_func();
    default: assert(0);
    }

    return nxm_sad_avg;
}
