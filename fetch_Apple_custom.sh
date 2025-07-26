#! /bin/bash
cd Apple_inorder_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p0 < custom.patch

cd ..
cd Apple_reverse_custom_fuzzer
git clone https://github.com/RensDofferhoff/iScanU.git
patch -p0 < custom.patch
mv iScanU iScanU_rev

echo "creat custom fuzzer for Apple Sillicon!"
