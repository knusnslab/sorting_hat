#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[+] Removing Apple ARM patches..."
rm -f "$SCRIPT_DIR"/apple_arm/iSacnU*

echo "[+] Removing RISC-V patches..."
rm -f "$SCRIPT_DIR"/riscv/iSacnU*

echo "[+] Done."
