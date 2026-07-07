# Sorting hat

`sorting_hat` is a framework for classifying undocumented insturcitons
on Apple ARM and RISC-V systems. The repository contains fuzzer patches, 
architecture-specific classifiers, and small helper scripts for 
post-processing generated instruction traces.

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

`scripts/trim.sh` post-processes classifier output by extracting selected fields
and deduplicating the result:

```sh
./scripts/trim.sh <result-file>
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


## Repository Structure

```text
.
|-- Makefile
|   Build rules for Apple ARM and RISC-V classifier binaries.
|
|-- src/
|   |-- apple_arm/
|   |   `-- classifier.c
|   |       Apple ARM classifier implementation.
|   |
|   `-- riscv/
|       |-- classifier.c
|       |   RISC-V classifier implementation.
|       `-- init.S
|           RISC-V bootstrap instruction sequence linked into the classifier.
|
|-- patches/
|   |-- apple_arm/
|   |   |-- patch_Apple_custom.sh
|   |   |-- Apple_inorder_custom_fuzzer/
|   |   `-- Apple_reverse_custom_fuzzer/
|   |
|   `-- riscv/
|       |-- patch_riscv_custom.sh
|       |-- riscv_inorder_custom_fuzzer/
|       `-- riscv_reverse_custom_fuzzer/
|       Architecture-specific iScanU patch scripts and patch files.
|
|-- scripts/
|   |-- start.sh
|   |   Wrapper for running the classifier on an input trace.
|   `-- trim.sh
|       Post-processing helper for classifier results.
|
|-- test/
|   |-- verify.cpp
|   |   LLVM-based verifier for Apple ARM disassembly grouping.
|   `-- accuMet.cpp
|       Utility for comparing classifier and verifier results.
|
|-- docs/
|   Reserved for additional documentation.
|
|-- include/
|   Reserved for shared headers.
|
|
`
```
