#! /bin/bash
set -eou pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[+] Patching inorder fuzzer..."
cd "$SCRIPT_DIR"/Apple_inorder_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p0 < custom.patch
mv "$SCRIPT_DIR"/Apple_inorder_custom_fuzzer/iScanU "$SCRIPT_DIR"/Apple_inorder_custom_fuzzer/iScanU_inorder

echo "[+] Patching reverse fuzzer..."
cd "$SCRIPT_DIR"/Apple_reverse_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p0 < custom.patch
mv "$SCRIPT_DIR"/Apple_reverse_custom_fuzzer/iScanU "$SCRIPT_DIR"/Apple_reverse_custom_fuzzer/iScanU_inorder

echo "[+] Done."
echo "[+] Inorder output: $SCRIPT_DIR/Apple_inorder_custom_fuzzer/iSacnU_inorder"
echo "[+] Reverse output: $SCRIPT_DIR/Apple_reverse_custom_fuzzer/iSacnU_reverse"
