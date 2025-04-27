# Real-Time Audio DSP on STM32H750

Real time low-latency convolution-reverb implemented on an **STM32H750** Cortex-M7 microcontroller.

---

## Project Overview
This repository provides firmware, build scripts, and ancillary tooling for a **real-time stereo convolution reverb** optimised for the **STMicroelectronics STM32H750**. A uniform 512-sample, partitioned FFT engine—built on **Arm CMSIS-DSP** and hand-tuned intrinsics—achieves sub-2 ms end-to-end latency while streaming 48 kHz audio via DMA.

---

## Key Features
- Partitioned overlap-save FFT convolution (uniform 512-sample segments)
- Zero-copy DMA: SAI/I²S peripheral → double-buffered ring in **AXI-SRAM**
- Critical DSP kernels mapped to **ITCM/DTCM** for single-cycle access
- Cycle-accurate instrumentation using `DWT->CYCCNT`
- MATLAB tooling for automatic conversion of WAV impulse responses to pre-computed complex spectra headers
- Flexible build matrix (Debug / Release, floating- vs. fixed-point) using **CMake** or **STM32CubeIDE**
- Modular I/O abstraction: SAI, SPDIF-Rx, or USB Audio Class
- Desktop unit tests for core math routines with **CppUTest**

---

## Architecture & Toolchain
| Layer | Technology |
|-------|------------|
| **MCU** | ST STM32H750 @ 480 MHz (Cortex-M7, FPU + DSP) |
| **DSP Library** | Arm CMSIS-DSP v1.15 (e.g. `arm_rfft_fast_f32`, `arm_cmplx_mult_cmplx_f32`) |
| **Optimisations** | ARM Neon intrinsics, `__builtin_fmaf`, loop unrolling, Karatsuba-style complex multiply |
| **Language** | ISO C17, GCC 13 (`arm-none-eabi-gcc`) |
| **Build System** | CMake 3.22+; STM32CubeIDE project (.ioc); custom linker scripts |
| **Scripting** | MATLAB R2024a (IR preprocessing), Python 3.12 (asset packaging) |
| **Hardware I/O** | SAI/I²S, on-chip ADC (profiling), QSPI-Flash (IR storage), 32 MB SDRAM |
| **Debug & Trace** | ST-LINK V3, OpenOCD, hardware trace (`CYCCNT`), Tracealyzer (optional) |

---

## Getting Started
### Prerequisites
- Hardware: any STM32H750 board (validated on **NUCLEO-H750ZI**)
- Toolchain: `arm-none-eabi-gcc`, CMake ≥ 3.22 *or* STM32CubeIDE ≥ 1.15
- Optional: MATLAB for generating custom impulse-response headers

```bash
# Clone and initialise submodules (CMSIS-DSP, CppUTest)
git clone --recursive https://github.com/<your-handle>/<repo>.git
cd <repo>

# Build desktop unit tests
cmake -B build/host -DCONFIG=host -S .
cmake --build build/host && ctest --test-dir build/host

# Build MCU firmware (Release)
cmake -B build/h750 -DCONFIG=stm32h750_release -S .
cmake --build build/h750 -j

# Flash using OpenOCD (ST-LINK V3)
openocd -f board/st_nucleo_h750.cfg         -c "program build/h750/firmware.elf verify reset exit"
```

#### Regenerating IR Headers
```matlab
addpath("tools/matlab");
gen_ir_headers("IRs/emt_140_dark_3.wav", 48000, 512, "firmware/Inc/IR_FFT.h");
```

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

Detailed timing scatter plots are available under **docs/perf/**.

---

## Roadmap
- [ ] Fixed-point (Q31) build targeting H7 value-line devices (no FPU)
- [ ] USB Audio 2.0 device class (up to 192 kHz)
- [ ] Live IR replacement via UART command-line interface
- [ ] Multi-channel (≥ 4 ch) convolution for immersive formats
- [ ] Port to RP2040 (dual-core Cortex-M0+ SMP)

---

## License
Distributed under the **MIT License**. See the [`LICENSE`](LICENSE) file for full details.

---

## Acknowledgements
- Arm CMSIS-DSP team
- STMicroelectronics STM32 product, HAL, and CubeIDE teams
- OpenAIR library for EMT-140 impulse response
- Contributors on the STM32, DSP, and embedded-audio communities
