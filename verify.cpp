#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <fstream>
#include <string_view>
#include <llvm-c/Disassembler.h>
#include <llvm-c/Target.h>

LLVMDisasmContextRef  DCR;
using namespace std;

// 그룹 정보를 저장하는 unordered_map
unordered_map<string, string> itg;

// 그룹 정보를 초기화하는 함수
void initializeGroups() {
    // ALU Group
    itg["abs"] = "ALU";
    itg["adc"] = "ALU";
    itg["addg"] = "ALU";
    itg["adrp"] = "ALU";
    itg["and"] = "ALU";
    itg["ands"] = "ALU";
    itg["asr"] = "ALU";
    itg["asrv"] = "ALU";
    itg[""] = "ALU";

    itg["add"] = "ALU";
    itg["add.16b"] = "ALU";
    itg["add.2s"] = "ALU";
    itg["add.4s"] = "ALU";
    itg["add.2d"] = "ALU";
    itg["add.8b"] = "ALU";
    itg["addp.2d"] = "ALU";
    itg["adr"] = "ALU"; //PC값으로 부터 상대주소 저장하고 register에 저장하는 명령어
    itg["addv.4s"] = "ALU";
    itg["adr"] = "ALU";
    itg["and"] = "ALU";
    itg["and.16b"] = "ALU";
    itg["and.8b"] = "ALU";
    itg["asr"] = "ALU";
    itg["bfi"] = "ALU";
    itg["bfxil"] = "ALU";
    itg["bic"] = "ALU";
    itg["bic.16b"] = "ALU";
    itg["bic.2s"] = "ALU";
    itg["bic.4s"] = "ALU";
    itg["bif.16b"] = "ALU";
    itg["bit.16b"] = "ALU";
    itg["bit.8b"] = "ALU";
    itg["bsl.16b"] = "ALU";
    itg["bsl.8b"] = "ALU";
    itg["cinv"] = "ALU";
    itg["clz"] = "ALU";
    itg["cmeq.16b"] = "ALU";
    itg["cmeq.2d"] = "ALU";
    itg["cmeq.2s"] = "ALU";
    itg["cmeq.4s"] = "ALU";
    itg["cmeq.8b"] = "ALU";
    itg["cmhi.16b"] = "ALU";
    itg["cmhi.2d"] = "ALU";
    itg["cmhi.2s"] = "ALU";
    itg["cmhi.8b"] = "ALU";
    itg["cmhs.16b"] = "ALU";
    itg["cmhs.8b"] = "ALU";
    itg["cmlt.2s"] = "ALU";
    itg["cmtst.2d"] = "ALU";
    itg["cmtst.4s"] = "ALU";
    itg["cneg"] = "ALU";
    itg["cnt.8b"] = "ALU";
    itg["csel"] = "ALU";
    itg["cset"] = "ALU";
    itg["csetm"] = "ALU";
    itg["csinc"] = "ALU";
    itg["csinv"] = "ALU";
    itg["csneg"] = "ALU";
    itg["dup.16b"] = "ALU";
    itg["dup.2d"] = "ALU";
    itg["dup.2s"] = "ALU";
    itg["dup.4h"] = "ALU";
    itg["dup.4s"] = "ALU";
    itg["dup.8b"] = "ALU";
    itg["eon"] = "ALU";
    itg["eor"] = "ALU";
    itg["eor.16b"] = "ALU";
    itg["eor.8b"] = "ALU";
    itg["ext.16b"] = "ALU";
    itg["extr"] = "ALU";
    itg["fabs"] = "ALU";
    itg["fadd"] = "ALU";
    itg["fadd.2d"] = "ALU";
    itg["fcsel"] = "ALU";
    itg["fcvt"] = "ALU";
    itg["fcvtms"] = "ALU";
    itg["fcvtzs"] = "ALU";
    itg["fcvtzu"] = "ALU";
    itg["fdiv"] = "ALU";
    itg["fmadd"] = "ALU";
    itg["fmax"] = "ALU";
    itg["fmaxnm"] = "ALU";
    itg["fminnm"] = "ALU";
    itg["fmla.2d"] = "ALU";
    itg["fmov"] = "ALU";
    itg["fmov.2d"] = "ALU";
    itg["fmsub"] = "ALU";
    itg["frintm"] = "ALU";
    itg["frintp"] = "ALU";
    itg["fsqrt"] = "ALU";
    itg["fusb"] = "ALU";
    itg["fsub.2d"] = "ALU";
    itg["lsl"] = "ALU";
    itg["lsr"] = "ALU";
    itg["madd"] = "ALU";
    itg["mla.4s"] = "ALU";
    itg["mls.4s"] = "ALU";
    itg["mneg"] = "ALU";
    itg["mov"] = "ALU";
    itg["mov.16b"] = "ALU";
    itg["mov.b"] = "ALU";
    itg["mov.d"] = "ALU";
    itg["mov.h"] = "ALU";
    itg["mov.s"] = "ALU";
    itg["movi"] = "ALU";
    itg["movi.16b"] = "ALU";
    itg["movi.2d"] = "ALU";
    itg["movi.2s"] = "ALU";
    itg["movi.4s"] = "ALU";
    itg["movi.8b"] = "ALU";
    itg["movk"] = "ALU";
    itg["msub"] = "ALU";
    itg["mul"] = "ALU";
    itg["mul.2s"] = "ALU";
    itg["mul.4s"] = "ALU";
    itg["mvn"] = "ALU";
    itg["mvn.16b"] = "ALU";
    itg["mvn.8b"] = "ALU";
    itg["mvni.2s"] = "ALU";
    itg["mvni.4s"] = "ALU";
    itg["neg"] = "ALU";
    itg["neg.2d"] = "ALU";
    itg["neg.4s"] = "ALU";
    itg["orn"] = "ALU";
    itg["orr"] = "ALU";
    itg["orr.16b"] = "ALU";
    itg["orr.2s"] = "ALU";
    itg["orr.8b"] = "ALU";
    itg["rbit"] = "ALU";
    itg["rev"] = "ALU";
    itg["rev16"] = "ALU";
    itg["rev64.2s"] = "ALU";
    itg["rev64.4s"] = "ALU";
    itg["ror"] = "ALU";
    itg["sbfiz"] = "ALU";
    itg["sbfx"] = "ALU";
    itg["scvtf"] = "ALU";
    itg["scvtf.2d"] = "ALU";
    itg["sdiv"] = "ALU";
    itg["shl.16b"] = "ALU";
    itg["shl.2d"] = "ALU";
    itg["shl.4s"] = "ALU";
    itg["smaddl"] = "ALU";
    itg["smin.4s"] = "ALU";
    itg["sminv.4s"] = "ALU";
    itg["smsubl"] = "ALU";
    itg["smulh"] = "ALU";
    itg["smull"] = "ALU";
    itg["sshl.2d"] = "ALU";
    itg["sshll.2d"] = "ALU";
    itg["sshll.4s"] = "ALU";
    itg["sshll.8h"] = "ALU";
    itg["sshll2.2d"] = "ALU";
    itg["sshll2.4s"] = "ALU";
    itg["sshll2.8h"] = "ALU";
    itg["sub"] = "ALU";
    itg["sub.16b"] = "ALU";
    itg["sub.2d"] = "ALU";
    itg["sub.2s"] = "ALU";
    itg["sub.4s"] = "ALU";
    itg["sub.8b"] = "ALU";
    itg["sxtb"] = "ALU";
    itg["sxth"] = "ALU";
    itg["sxtw"] = "ALU";
    itg["tbl.16b"] = "ALU";
    itg["trn1.4s"] = "ALU";
    itg["uaddlv.8b"] = "ALU";
    itg["uaddw.2d"] = "ALU";
    itg["uaddw2.2d"] = "ALU";
    itg["ubfiz"] = "ALU";
    itg["ubfx"] = "ALU";
    itg["ucvtf"] = "ALU";
    itg["ucvtf.2d"] = "ALU";
    itg["udiv"] = "ALU";
    itg["umaddl"] = "ALU";
    itg["umax.4s"] = "ALU";
    itg["umaxv.4s"] = "ALU";
    itg["umin.4s"] = "ALU";
    itg["umov.b"] = "ALU";
    itg["umov.h"] = "ALU";
    itg["umsubl"] = "ALU";
    itg["umulh"] = "ALU";
    itg["umull"] = "ALU";
    itg["umull.2d"] = "ALU";
    itg["ushl.2d"] = "ALU";
    itg["ushl.2s"] = "ALU";
    itg["ushl.4s"] = "ALU";
    itg["ushll.2d"] = "ALU";
    itg["ushll.4s"] = "ALU";
    itg["ushll.8h"] = "ALU";
    itg["ushll2.2d"] = "ALU";
    itg["ushll2.4s"] = "ALU";
    itg["ushll2.8h"] = "ALU";
    itg["ushr.16b"] = "ALU";
    itg["ushr.2d"] = "ALU";
    itg["ushr.2d"] = "ALU";
    itg["ushr.4s"] = "ALU";
    itg["ushr.8b"] = "ALU";
    itg["usra.4s"] = "ALU";
    itg["uzp1.16b"] = "ALU";
    itg["uzp1.4s"] = "ALU";
    itg["uzp1.8h"] = "ALU";
    itg["uzp2.4s"] = "ALU";
    itg["xtn.2s"] = "ALU";
    itg["xtn.4h"] = "ALU";
    itg["xtn.8b"] = "ALU";
    itg["zip1.16b"] = "ALU";
    itg["zip1.4s"] = "ALU";
    itg["zip2.16b"] = "ALU";
    itg["cinc"] = "ALU";
    itg["cmgt.2d"] = "ALU";
    itg["fmul"] = "ALU";
    itg["fmul.2d"] = "ALU";
    itg["fneg"] = "ALU";
    itg["fnmadd"] = "ALU";
    itg["fnmsub"] = "ALU";
    itg["fsub"] = "ALU";
    itg["uminv.4s"] = "ALU";
    itg["umull2.2d"] = "ALU";
    itg["ushr.2s"] = "ALU";

    //Branch Group
    itg["blr"] = "BRANCH";
    itg["br"] = "BRANCH";
    itg["ret"] = "BRANCH";

    //Load/Store Group
    itg["ld1.b"] = "LS";
    itg["ld1.d"] = "LS";
    itg["ld1.s"] = "LS";
    itg["ld1r.2d"] = "LS";
    itg["ld1r.4s"] = "LS";
    itg["ld2.16b"] = "LS";
    itg["ld2.8b"] = "LS";
    itg["ld3.4s"] = "LS";
    itg["ld4.16b"] = "LS";
    itg["ld4.4s"] = "LS";
    itg["ld4.8b"] = "LS";
    itg["ldp"] = "LS";
    itg["ldpsw"] = "LS";
    itg["ldr"] = "LS";
    itg["ldrb"] = "LS";
    itg["ldrh"] = "LS";
    itg["ldrsb"] = "LS";
    itg["ldrsh"] = "LS";
    itg["ldrsw"] = "LS";
    itg["ldur"] = "LS";
    itg["ldurb"] = "LS";
    itg["ldurh"] = "LS";
    itg["ldursb"] = "LS";
    itg["ldursh"] = "LS";
    itg["ldursw"] = "LS";
    itg["st1.s"] = "LS";
    itg["st2.16b"] = "LS";
    itg["st2.8b"] = "LS";
    itg["st3.4h"] = "LS";
    itg["st4.16b"] = "LS";
    itg["st4.8b"] = "LS";
    itg["stp"] = "LS";
    itg["str"] = "LS";
    itg["strb"] = "LS";
    itg["strh"] = "LS";
    itg["stur"] = "LS";
    itg["sturb"] = "LS";
    itg["sturh"] = "LS";

    //Condition Group
    itg["adcs"] = "COND";
    itg["ands"] = "COND";
    itg["adds"] = "COND";
    itg["bics"] = "COND";
    itg["cmn"] = "COND";
    itg["cmp"] = "COND";
    itg["fccmp"] = "COND";
    itg["ccmp"] = "COND";
    itg["fcmp"] = "COND";
    itg["negs"] = "COND";
    itg["subs"] = "COND";
    itg["tst"] = "COND";
    itg["ccmn"] = "COND";

    //NOP Group
    itg["nop"] = "NOP";

    //undefined
    itg["udf"] = "UDF";

    //MISC
    //SYSTEM Group
    // itg["AT"] = "SYS"; // sys isntruction alias
    // itg["brk"] = "SYS";

    itg["AT"] = "BRANCH"; // sys isntruction alias
    itg["brk"] = "BRANCH";
}

// 입력된 명령어의 그룹을 찾는 함수
string getGroup(const string& instruction) {
    stringstream ss(instruction);
    string opcode;
    ss >> opcode; // 명령어의 첫 번째 토큰(opcode) 추출
    
    transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);
    // unordered_map에서 opcode에 해당하는 그룹 찾기
    auto it = itg.find(opcode);
    if (it != itg.end()) {
        return it->second; // 그룹 반환
    } else {
        return ""; // 찾지 못한 경우
    }
}

void init_llvm(){

    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllDisassemblers();
    DCR = LLVMCreateDisasmCPUFeatures("arm64-apple-darwin", "generic", "+all", NULL, 0, NULL, NULL);
}

string getDisasm(uint32_t inst){
    
    char outString[255];
    uint8_t byte[4];
    memcpy(byte, &inst, 4);
    size_t instr_size = LLVMDisasmInstruction(DCR, byte, 4, 0x0, outString, sizeof(outString));
    string result(outString);
    return result;
}


int main() {
    
    uint32_t byte;
    string dummy;
    string hex_str;
    string filename = "DUMP/thread";
    string outname = "V-result";
    ifstream inputFile(filename);
    ofstream outFile(outname);
    string line;   
    
    
    init_llvm();
    initializeGroups();
    
    if(inputFile.is_open()){
        cout << "Success to open the file" << endl;
    }
    else {
        cout << "Failed to open the file" << endl;
        exit(0);
    }

    if(outFile.is_open()){
        cout << "Sucess to open the file" << endl;
    }
    else {
        cout << "Faile to open the file" << endl;
        exit(0);
    }

    while(getline(inputFile, line)){ 
        istringstream iss(line);
        if(iss >> dummy >> hex_str)
            byte = stoul(hex_str, nullptr, 16);

        string targetInst = getDisasm(byte);
        string group1 = getGroup(targetInst);
        if (!group1.empty()) {
            // cout << targetInst << "는 그룹 " << group1 << "에 속합니다." << endl;
            outFile  << hex << byte << " "  << group1 << endl;
        } else {
            cout << targetInst << "에 해당하는 그룹을 찾을 수 없습니다." << endl;
        }
    }
    
    inputFile.close();
    
    return 0;
}
