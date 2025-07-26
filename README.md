# sorting_hat

To explore and analyze undocumented instructions on Apple Silicon, you can build a custom fuzzer based on the iScanU project (https://github.com/RensDofferhoff/iScanU).

Generating a custom fuzzer for Apple Silicon
From the project root directory, run the following command:

<code>``` ./fetch_Apple_custom.sh ```<code>

After running the command, Apple Silicon-optimized fuzzer source code will be generated in the Apple_inorder_custom_fuzzer and Apple_reverse_custom_fuzzer directories.

Instruction classification
Once you've finished exploring with the fuzzer, compile classifier.c:

<code>``` make classifier ```<code>

You can then use the generated classifier binary to categorize the output results from the fuzzer.
