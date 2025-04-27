#pragma GCC optimize ("Ofast,unroll-loops,fast-math")

#include "arm_math.h"
#include "pipe.h"

#ifndef FFT_SIZE

	#define FFT_SIZE    1024

#endif

#ifndef BUFFER_SIZE

#define BUFFER_SIZE (FFT_SIZE/2)

#endif

__attribute__((section(".dtcm"), aligned(32))) float zeropaddedinput[FFT_SIZE];
__attribute__((section(".dtcm"), aligned(32))) float overlap[BUFFER_SIZE];

extern arm_rfft_fast_instance_f32 fft;
extern float fftOut[FFT_SIZE];

void ova_convolve(pipe *pipe, fir_t *fir)
{

    // prepare input
    arm_copy_f32(pipe->processBuffer, zeropaddedinput, BUFFER_SIZE);
    arm_fill_f32(0.0f, &zeropaddedinput[BUFFER_SIZE], BUFFER_SIZE);

    // forward FFT
    arm_rfft_fast_f32(&fft, zeropaddedinput, fftOut, 0);

    // save index and spectrum
    fir->prev_fftidx = fir->curr_fftidx;
    arm_copy_f32(fftOut, fir->prev_ffts[fir->curr_fftidx], FFT_SIZE);

    const uint32_t segs = fir->numSegments;
    uint32_t i = 0;
    uint32_t idx = fir->prev_fftidx;

    // partitioned convolution with pointer arithmetic
    do {
        const float32_t *restrict xPtr = fir->prev_ffts[idx];
        const float32_t *restrict hPtr = fir->ir_ffts[i];
              float32_t *restrict yPtr = fftOut;

              // DC + Nyquist bins
                 yPtr[0] += xPtr[0] * hPtr[0];
                 yPtr[1] += xPtr[1] * hPtr[1];
                 xPtr += 2;  hPtr += 2;  yPtr += 2;

                 for (uint32_t u = FFT_SIZE / 20;  u--;  ) {
                     // 1
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 2
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 3
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 4
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 5
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 6
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 7
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 8
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 9
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                     // 10
                     {
                         float xr = *xPtr++; float xi = *xPtr++;
                         float hr = *hPtr++; float hi = *hPtr++;
                         float accR = __builtin_fmaf(xr, hr, __builtin_fmaf(-xi, hi, *yPtr));
                         float accI = __builtin_fmaf(xr, hi, __builtin_fmaf(xi, hr, *(yPtr+1)));
                         *yPtr     = accR;
                         *(yPtr+1) = accI;
                         yPtr += 2;
                     }
                 }

                 // tail loop for any remaining bins
                 for (uint32_t rem = (FFT_SIZE % 20) / 2;  rem--;  ) {
                     float xr = *xPtr++;  float xi = *xPtr++;
                     float hr = *hPtr++;  float hi = *hPtr++;

                     float accR = __builtin_fmaf(xr, hr,
                                    __builtin_fmaf(-xi, hi, *yPtr));
                     float accI = __builtin_fmaf(xr, hi,
                                    __builtin_fmaf( xi, hr, *(yPtr+1)));

                     *yPtr     = accR;
                     *(yPtr+1) = accI;
                     yPtr += 2;
                 }

	    // advance segment indices
		if (++i >= segs) {
			i = 0;
		}
		if (idx-- == 0) {
			idx = segs - 1;
		}

    } while (idx != fir->curr_fftidx);

    // advance write index
    if (++fir->curr_fftidx >= segs) {
        fir->curr_fftidx = 0;
    }

    // inverse FFT
    arm_rfft_fast_f32(&fft, fftOut, zeropaddedinput, 1);

    // overlap-add and scaling
    const float32_t invN = 1.0f / (float32_t)segs;
    for (uint32_t k = 0;  k < BUFFER_SIZE;  ++k) {
        float32_t s = zeropaddedinput[k]            * invN;
        float32_t o = overlap[k]                    * invN;
        pipe->processBuffer[k] = s + o + 0.5f;
        overlap[k]             = zeropaddedinput[BUFFER_SIZE + k];
    }

}
