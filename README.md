# Real-Time Audio DSP on STM32H750

Real time low-latency convolution-reverb implemented on an **STM32H750** Cortex-M7 microcontroller.

---

## Overview
This repository contains source code for a **real-time stereo convolution reverb** optimised for the **STMicroelectronics STM32H750**. A uniform 512-sample, partitioned FFT engine—built on **Arm CMSIS-DSP** and intrinsics—achieves sub-2 ms end-to-end latency for a 48K sample convolution (1 second IR) while streaming 48 kHz audio via DMA.

---

## Key Features
- Partitioned overlap-save FFT convolution (uniform 512-sample segments)
- Zero-copy DMA: SAI/I²S peripheral → double-buffered ring in **AXI-SRAM**
- Critical DSP mapped to **ITCM/DTCM** for single-cycle low latency access

---

## Architecture & Toolchain
| Layer | Technology |
|-------|------------|
| **MCU** | ST STM32H750 @ 480 MHz (Cortex-M7, FPU + DSP) |
| **DSP Library** | Arm CMSIS-DSP v1.15 (e.g. `arm_rfft_fast_f32`, `arm_cmplx_mult_cmplx_f32`) |
| **Optimisations** | `__builtin_fmaf`, loop unrolling, Karatsuba-style complex multiply |
| **Language** | ISO C17, GCC 13 (`arm-none-eabi-gcc`) |
| **Scripting** | MATLAB R2024a (IR preprocessing), Python 3.12 (asset packaging) |
| **Hardware I/O** | SAI/I²S, on-chip ADC (profiling), QSPI-Flash (IR storage)|
| **Debug & Trace** | ST-LINK V3, OpenOCD, hardware trace (`CYCCNT`)

---

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

## Performance
| Metric | Value | Conditions |
|--------|-------|------------|
| End-to-end latency | **1.92 ms** | 512-sample block, 48 kHz, analogue-in → analogue-out |
| Peak CPU load | **38 %** | 12 IR partitions retrieved from QSPI-Flash |
| FFT throughput | **≈ 12 k FFT/s** | Determined via hardware cycle counter over 60 s run |

---
