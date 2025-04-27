# Real-Time DSP Convolution Reverb on STM32H750

Real time low-latency convolution-reverb implemented on an **STM32H750** Cortex-M7 microcontroller. Ported from the TI TMS320F28379D version. 

---

## Overview
This repository contains source code for a **real-time stereo convolution reverb** optimised for the **STMicroelectronics STM32H750**. A uniform 512-sample, partitioned FFT engine—built on **Arm CMSIS-DSP** and intrinsics—achieves sub-2 ms end-to-end latency for a 48K sample convolution (1 second IR) while streaming 48 kHz audio via DMA.

---

## Key Features
- Partitioned overlap-save FFT convolution (uniform 512-sample segments)
- Zero-copy DMA: ADC and DAC streaming 
- Critical DSP mapped to **ITCM/DTCM** for single-cycle low latency access

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
├─ firmware/          ─ STM32CubeIDE & CMake sources
│  ├─ Core/           ─ Drivers, BSP, application code
│  ├─ Drivers/        ─ HAL/LL subsets
│  └─ linker/         ─ Memory scripts (ITCM, DTCM, AXI-SRAM, SDRAM)
├─ tools/
│  ├─ matlab/         ─ gen_ir_headers.m
│  └─ python/         ─ asset bundler, CI helpers
├─ docs/              ─ Schematics, block diagrams, performance plots
├─ test/              ─ Host-side unit tests (CppUTest)
└─ CMakeLists.txt     ─ Top-level build
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
