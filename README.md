# Sorting hat
<!--
`sorting hat` is a framework for classifying undocumented instructions 
on ARM and RISC-V systems. It is designed to assist the post-discovery 
analysis of undocumented instructions by assigning behavior tags based on changes in architectural state. 
These tags help organize instruction semantics and support later-stage verification. 
The repository contains fuzzer patches, architecture-specific classifiers, 
and helper scripts for post-processing generated instruction traces.
-->
# Sorting Hat

`Sorting Hat` is a research framework developed as part of the paper 
**"[Automated Discovery, Classification, and Systematic Security Verification Methodology for Undocumented Instructions]"**, 
published at the **63rd ACM/IEEE Design Automation Conference (DAC 2026)**.
The paper presents a systematic methodology for automatically discovering undocumented processor instructions, 
classifying their behavior, and verifying their potential security implications. 
Sorting Hat implements the instruction discovery and classification components of this methodology, 
enabling automated behavioral analysis based on architectural state changes before and after instruction execution.
This repository provides architecture-specific classifiers for ARM (Apple Silicon) and RISC-V, 
patches for instruction fuzzers, and supporting scripts for processing execution traces. 
The framework aims to reduce the manual effort required to analyze undocumented instructions and provide a foundation for subsequent security verification.

## Build

The project uses a single `Makefile` with separate build targets for Apple ARM
and RISC-V. Build artifacts are placed under `build/bin/<architecture>/`.

```sh
make apple
make riscv
```

The Apple ARM target expects the Homebrew LLVM toolchain by default:

```text
/opt/homebrew/opt/llvm/bin/clang
/opt/homebrew/opt/llvm/bin/clang++
/opt/homebrew/opt/llvm/bin/llvm-config
```

The RISC-V target builds the classifier from `src/riscv/classifier.c` and
`src/riscv/init.S`:

```sh
make riscv
```

To remove generated build outputs:

```sh
make clean
```

## Fuzzer Patch Generation

The repository does not vendor the full iScanU source tree. Instead, patch
scripts clone iScanU and apply architecture-specific patches to generate custom
fuzzer variants.

```sh
./patches_fuzzer.sh
```

This invokes both architecture-specific patch scripts:

```sh
patches/apple_arm/patch_Apple_custom.sh
patches/riscv/patch_riscv_custom.sh
```

The generated fuzzer directories are created inside the corresponding patch
subdirectories. Network access is required because each script clones iScanU
before applying the local patch files.

## Classifier Workflow

The classifier consumes a fuzzer output file, the number of input lines, and an
output file path. The helper script in `scripts/start.sh` wraps this convention:

```sh
./scripts/start.sh <input-file> <output-file>
```

The current helper expects a `classifier` binary in the working directory. If
the binary was built with the provided `Makefile`, use the generated binary path
directly or copy/symlink it before running the script:

```sh
build/bin/riscv/classifier <input-file> <line-count> <output-file>
```

## Running Overview
```text
GP1 : 0x3fb4219000
IP : 0x3fb0219000
GP2 : 0x3fac219000
instr_pointer : 0x3fb0219000
instr_pointer : 0x3fb4218fc0
sa_flag : 8000004
base cycle = 346775
[>                                                 ] 1%(426 / 23179)
Undoc instr: fee044e3
```
