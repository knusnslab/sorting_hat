#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[+] Applying Apple ARM patches..."
bash "$SCRIPT_DIR/patches/apple_arm/patch_Apple_custom.sh"

echo "[+] Applying RISC-V patches..."
bash "$SCRIPT_DIR/patches/riscv/patch_riscv_custom.sh"

echo "[+] Done."
