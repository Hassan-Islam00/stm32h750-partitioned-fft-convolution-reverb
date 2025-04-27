/*
 * pipe.h
 *
 *  Created on: Feb 19, 2025
 *      Author: spencer
 */
#ifndef BUFFER_SIZE
  #define BUFFER_SIZE 1024
#endif

#ifndef DELAY_BUFFER_SIZE
  #define DELAY_BUFFER_SIZE (BUFFER_SIZE * 10)
#endif

#ifndef ADC_BITS2VOLTS
#define ADC_BITS2VOLTS 0.00005035400390625f
#endif

#ifndef DAC_VOLTS2BITS
#define DAC_VOLTS2BITS 1241.2121212121212121212121212121f
#endif


#ifndef SRC_PIPE_H_
#define SRC_PIPE_H_

#include <stdint.h>
#include <stdbool.h>

#define __FPU_PRESENT  1U
#define ARM_MATH_CM7
#include "arm_math.h"


typedef struct pipe pipe;

// Function pointer types for the "methods"
typedef void (*ADCCallback_Fn)(pipe *self, const volatile uint16_t *adcInput);
typedef void (*updateDelay_Fn)(pipe *self);
typedef void (*updateDAC_Fn)(pipe *self, volatile uint16_t *dacBuffer);
typedef void (*loadProcess_Fn)(pipe *self);
typedef float32_t *(*getDelayBuffer_Fn)(pipe *self, uint16_t n);

struct pipe
{

    float32_t inBuffer1[BUFFER_SIZE];
    float32_t inBuffer2[BUFFER_SIZE];

    float32_t outBuffer1[BUFFER_SIZE];
    float32_t outBuffer2[BUFFER_SIZE];

    float32_t *inBuffer;
    float32_t *outBuffer;

    float32_t processBuffer[BUFFER_SIZE]; // HI - changed processBuffer size from BUFFER_SIZE to BUFFER_SIZE*2 to be inherently zero-padded.

    float32_t delayBuffer[DELAY_BUFFER_SIZE];

    uint8_t ppState;
    uint32_t delayIndex;
    bool bufferReady;

    ADCCallback_Fn adcHalfComplete;
    ADCCallback_Fn adcComplete;
    updateDelay_Fn updateDelayBuffer;
    updateDAC_Fn updateDACOutput;
    getDelayBuffer_Fn getDelayBuffer;
    loadProcess_Fn loadProcess;


};

void pipeInit(pipe *self);

#endif /* SRC_PIPE_H_ */
