LLVM_LDFLAGS =
LLVM_CLFAGS =
LLVM_LIBS =
OS = -DMacOS

LLVM_INC = $(LLVM_LDFLAGS) $(LLVM_CFLAGS) $(LLVM_LIBS)

ompCPP=/opt/homebrew/Cellar/llvm/your_version/bin/clang++
ompC=/opt/homebrew/Cellar/llvm/your_version/bin/clang
ompCFAGS=-I/opt/homebrew/Cellar/llvm/your_version/clfags
ompLDFLAGS=-L/opt/homebrew/Cellar/llvm/your_version/ldflags

classifier : classifier.c
	gcc -O2 -w -g -o $@ $^ $(LLVM_INC) $(OS)
	install_name_tool -add_rpath ~/your/llvm/lib/path classifier

verify : verify.c
	g++ -O2 -g -o $@ $^ $(LLVM_INC)
	install_name_tool -add_rpath ~/your/llvm/lib/path verify

accuMet : accuMet.cpp
	$(ompCPP) $(ompCFLAGS) -o $@ $^ $(ompLDFLAGS)

clean :
	rm classifier verify accuMet
