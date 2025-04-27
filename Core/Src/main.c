// -----------------------------------------------------------------------------
//  Real-Time Convolution Reverb – STM32H750 (no low‑pass section)
// -----------------------------------------------------------------------------

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// CMSIS / DSP
#define __FPU_PRESENT 1U
#define ARM_MATH_CM7
#include "arm_math.h"

// Project headers
#include "pipe.h"
#include "emt_140_dark_3.h"   // IR partitions
#include "fastConvolution.h"  // DSP hot path

// ----------------------------- compile-time constants ------------------------
#define FFT_SIZE 2048u  // 512‑sample block → 1024‑pt FFT with zero‑pad

// ----------------------------- HAL handles ----------------------------------
static ADC_HandleTypeDef  hadc1;
static DMA_HandleTypeDef  hdma_adc1;
static DAC_HandleTypeDef  hdac1;
static DMA_HandleTypeDef  hdma_dac1_ch1;
static TIM_HandleTypeDef  htim8;

// ----------------------------- DSP objects & buffers ------------------------
pipe                       gPipe;
arm_rfft_fast_instance_f32 gFFT;

static uint16_t  adcInput[BUFFER_SIZE * 2];
static uint16_t  dacOutput[BUFFER_SIZE * 2];
__attribute__((section(".dtcm"), aligned(32)))
static float32_t fftOut[BUFFER_SIZE * 2];

// ----------------------------- forward declarations -------------------------
static void SystemClock_Config(void);
static void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM8_Init(void);
static void MX_DAC1_Init(void);

// ----------------------------- ADC DMA callbacks ----------------------------
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    SCB_InvalidateDCache_by_Addr((uint32_t*)adcInput, BUFFER_SIZE * sizeof(uint16_t));
    gPipe.adcHalfComplete(&gPipe, adcInput);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    SCB_InvalidateDCache_by_Addr((uint32_t*)&adcInput[BUFFER_SIZE], BUFFER_SIZE * sizeof(uint16_t));
    gPipe.adcComplete(&gPipe, adcInput);
}

// ----------------------------- application entry ---------------------------
int main(void)
{
    // low‑level setup
    MPU_Config();
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM8_Init();
    MX_DAC1_Init();

    SCB_EnableDCache();
    SCB_EnableICache();

    // project‑level init
    arm_rfft_fast_init_f32(&gFFT, FFT_SIZE);
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcInput, BUFFER_SIZE * 2);
    HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t*)dacOutput, BUFFER_SIZE * 2, DAC_ALIGN_12B_R);

    HAL_TIM_Base_Start(&htim8);
    pipeInit(&gPipe);

    // main loop – wait for ready buffer, process, sleep
    for (;;) {
        if (gPipe.bufferReady) {
            gPipe.updateDelayBuffer(&gPipe);
            gPipe.loadProcess(&gPipe);

            ova_convolve(&gPipe, &fir_emt_140_dark_3);

            arm_copy_f32(gPipe.processBuffer, gPipe.outBuffer, BUFFER_SIZE);
            gPipe.updateDACOutput(&gPipe, dacOutput);
            SCB_CleanDCache_by_Addr((uint32_t*)dacOutput, BUFFER_SIZE * 2 * sizeof(uint16_t));

            gPipe.bufferReady = false;
        } else {
            __WFI(); // sleep until next DMA IRQ
        }
    }
}

// ----------------------------- peripheral init (CubeMX) --------------------
// SystemClock_Config, MX_ADC1_Init, MX_TIM8_Init, etc. – unchanged
// ---------------------------------------------------------------------------

void Error_Handler(void)
{
    __disable_irq();
    while (true) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file; (void)line;
}
#endif
