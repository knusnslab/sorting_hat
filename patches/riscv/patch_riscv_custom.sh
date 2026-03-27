#! /bin/bash
cd riscv_inorder_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p1 < custom_inorder.patch

cd ..
cd riscv_reverse_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p1 < custom_reverse.patch
mv iScanU iScanU_rev

echo "creat custom fuzzer for RISC-V!"
