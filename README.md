# Real-Time DSP Convolution Reverb on STM32H750
_Uniform‑Partitioned FFT • 48 kHz • 512‑Sample Blocks_

## Overview
This repository contains source code for a **real-time stereo convolution reverb** optimised for the **STMicroelectronics STM32H750**. A uniform 512-sample, partitioned FFT engine—built on **Arm CMSIS-DSP** and intrinsics—achieves sub-2 ms end-to-end latency for a 48K sample convolution (1 second IR) while streaming 48 kHz audio via DMA.

Key elements:
- **Uniform‑partitioned convolution** (uniform 512-sample segments)
- **Ping‑pong DMA buffering** Zero-copy DMA: ADC and DAC streaming 
- Critical DSP mapped to **ITCM/DTCM** for single-cycle low latency access
- hardware‑accelerated **Arm CMSIS-DSP** library

---

## Technologies 
| Layer | Technology |
|-------|------------|
| **MCU** | ST STM32H750 @ 480 MHz (Cortex-M7, FPU + DSP) |
| **DSP Library** | Arm CMSIS-DSP v1.15 (e.g. `arm_rfft_fast_f32`, `arm_cmplx_mult_cmplx_f32`) |
| **Optimisations** | `__builtin_fmaf`, loop unrolling, Karatsuba-style complex multiply |
| **Language** | ISO C17, GCC 13 (`arm-none-eabi-gcc`) |
| **Scripting** | MATLAB R2024a (IR preprocessing), Python 3.12 (asset packaging) |
| **Hardware I/O** | SAI/I²S, QSPI-Flash (IR storage)|
| **Debug & Trace** | ST-LINK V3, hardware trace (`CYCCNT`)

---

## Algorithm Summary
### 1 Ping‑Pong Buffering
The ADC DMA fills _Buffer A_ while the CPU processes _Buffer B_; the roles swap every 512 samples.

### 2 Uniform Partitioning
The IR is pre‑split into **P = 12** equal partitions of 512 samples. Each partition is zero‑padded to 1024 samples and stored as a complex spectrum

### 3 Per‑Block Processing
```text
1. Pad current 512‑sample block to 1024 samples
2. X[k]  = FFT( x[n] )
3. Y[k]  = Σₚ X_{p}[k] × IR_FFT[p][k]
4. y[n]  = IFFT( Y[k] )            ← overlap‑add to output buffer

A good source explaining the algorithm used 
<https://thewolfsound.com/fast-convolution-fft-based-overlap-add-overlap-save-partitioned/>

## Repository Structure
```
<repo>
├─ Core/                   # Application code and BSP
│   ├─ Inc/                # Header files
│   ├─ Src/                # C sources (main, convolution, utilities)
│   └─ Startup/            # Reset handler and vector table
├─ Drivers/                # Device and HAL libraries
│   ├─ CMSIS/              # Cortex-M and device headers
│   └─ STM32H7xx_HAL_Driver/ # ST HAL implementation
├─ Lib/                    # Pre‑built DSP library (libarm_cortexM7lfsp_math.a)
├─ STM32H743-DSP.ioc       # STM32CubeMX project descriptor
├─ *.ld                    # Linker scripts (FLASH, ITCM/DTCM, AXI-SRAM)
├─ .gitignore, .gitattributes, .mxproject
└─ README.md               # Project documentation
```
---

### 4 Performance Metrics
| Metric | Value |
|--------|-------|
| Sample rate | 48 kHz stereo |
| Block size | 512 samples |
| FFT size | 1024 points |
| IR length | 6144 samples |
| End-to-end processing latency | **2.32 ms** |
| CPU load | 21 % (measured) |
| SRAM usage | ~96 kB |

---
