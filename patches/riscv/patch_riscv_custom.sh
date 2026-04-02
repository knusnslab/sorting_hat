#! /bin/bash
set -eou pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[+] Patching inorder RISC-V fuzzer..."
cd "$SCRIPT_DIR"/riscv_inorder_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p1 < custom_inorder.patch
mv "$SCRIPT_DIR"/riscv_inorder_custom_fuzzer/iScanU "$SCRIPT_DIR"/riscv_inorder_custom_fuzzer/iScanU_inorder

echo "[+] Patching reverse RISC-V fuzzer..."
cd "$SCRIPT_DIR"/riscv_reverse_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p1 < custom_reverse.patch
mv "$SCRIPT_DIR"/riscv_reverse_custom_fuzzer/iScanU "$SCRIPT_DIR"/riscv_reverse_custom_fuzzer/iScanU_reverse

echo "[+] Done."
echo "[+] Inorder output : $SCRIPT_DIR/riscv_inorder_custom_fuzzer/iScanU_inorder"
echo "[+] Reverse output : $SCRIPT_DIR/riscv_reverse_custom_fuzzer/iScanU_reverse"
