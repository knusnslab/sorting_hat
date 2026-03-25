#include <stdio.h>
#include <time.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <mach/mach_time.h>

#define urprint(outfile, rtype, reg_num, before, after) \
    fprintf(outfile, "chagned Register : %s%d\t b:%llx a:%llx\n", rtype, reg_num, before, after);

#define frprint(outfile, instr, rtype, reg_num, before, after) \
    fprintf(outfile, "Forged instr : %llx\t %s%d b:%llx a:%llx\n", instr, rtype, reg_num, before, after);

#define frprint2(outfile, instr) \
    fprintf(outfile, "Forged instr : %llx\t detected CONSTRAINED UNPREDICTABLE behaviors\n", instr);

#define ItypePrint(outfile,Type, instr, desc) \
        fprintf(outfile, "%s : %llx %s\n", Type, instr, desc);

#define INIT_VECTOR_REG()        \
        asm volatile(           \
        "movi v0.16b, #32\n"     \
        "movi v1.16b, #1\n"     \
        "movi v2.16b, #2\n"     \
        "movi v3.16b, #3\n"     \
        "movi v4.16b, #4\n"     \
        "movi v5.16b, #5\n"     \
        "movi v6.16b, #6\n"     \
        "movi v7.16b, #7\n"     \
        "movi v8.16b, #8\n"     \
        "movi v9.16b, #9\n"     \
        "movi v10.16b, #10\n"   \
        "movi v11.16b, #11\n"   \
        "movi v12.16b, #12\n"   \
        "movi v13.16b, #13\n"   \
        "movi v14.16b, #14\n"   \
        "movi v15.16b, #15\n"   \
        "movi v16.16b, #16\n"   \
        "movi v17.16b, #17\n"   \
        "movi v18.16b, #18\n"   \
        "movi v19.16b, #19\n"   \
        "movi v20.16b, #20\n"   \
        "movi v21.16b, #21\n"   \
        "movi v22.16b, #22\n"   \
        "movi v23.16b, #23\n"   \
        "movi v24.16b, #24\n"   \
        "movi v25.16b, #25\n"   \
        "movi v26.16b, #26\n"   \
        "movi v27.16b, #27\n"   \
        "movi v28.16b, #28\n"   \
        "movi v29.16b, #29\n"   \
        "movi v30.16b, #30\n"   \
        "movi v31.16b, #31\n"   \
        )

float v_init[32][4];
#define INIT_VECTOR_REG2()        \
        asm volatile(           \
        "ld1    {v0.4s}, [%0]   \n" \
        "ld1    {v1.4s}, [%1]   \n" \
        "ld1    {v2.4s}, [%2]   \n" \
        "ld1    {v3.4s}, [%3]   \n" \
        "ld1    {v4.4s}, [%4]   \n" \
        "ld1    {v5.4s}, [%5]   \n" \
        "ld1    {v6.4s}, [%6]   \n" \
        "ld1    {v7.4s}, [%7]   \n" \
        "ld1    {v8.4s}, [%8]   \n" \
        "ld1    {v9.4s}, [%9]   \n" \
        "ld1    {v10.4s}, [%10]   \n"    \
        "ld1    {v11.4s}, [%11]   \n"    \
        "ld1    {v12.4s}, [%12]   \n"    \
        "ld1    {v13.4s}, [%13]   \n"    \
        "ld1    {v14.4s}, [%14]   \n"    \
        "ld1    {v15.4s}, [%15]   \n"    \
        ::  "r"(v_init[0]), "r"(v_init[1]), "r"(v_init[2]), "r"(v_init[3]), "r"(v_init[4]), "r"(v_init[5]), "r"(v_init[6]), \
             "r"(v_init[7]), "r"(v_init[8]), "r"(v_init[9]), "r"(v_init[10]), "r"(v_init[11]), "r"(v_init[12]), "r"(v_init[13]), \
             "r"(v_init[14]), "r"(v_init[15]), "r"(v_init[16])\
        : "memory" \
        )

#define INIT_VECTOR_REG3()        \
        asm volatile(           \
        "ld1    {v16.4s}, [%0]   \n" \
        "ld1    {v17.4s}, [%1]   \n" \
        "ld1    {v18.4s}, [%2]   \n" \
        "ld1    {v19.4s}, [%3]   \n" \
        "ld1    {v20.4s}, [%4]   \n" \
        "ld1    {v21.4s}, [%5]   \n" \
        "ld1    {v22.4s}, [%6]   \n" \
        "ld1    {v23.4s}, [%7]   \n" \
        "ld1    {v24.4s}, [%8]   \n" \
        "ld1    {v25.4s}, [%9]   \n" \
        "ld1    {v26.4s}, [%10]   \n"    \
        "ld1    {v27.4s}, [%11]   \n"    \
        "ld1    {v28.4s}, [%12]   \n"    \
        "ld1    {v29.4s}, [%13]   \n"    \
        "ld1    {v30.4s}, [%14]   \n"    \
        "ld1    {v31.4s}, [%15]   \n"    \
        ::  "r"(v_init[16]), "r"(v_init[17]), "r"(v_init[18]), "r"(v_init[19]), "r"(v_init[20]), "r"(v_init[21]), \
             "r"(v_init[22]), "r"(v_init[23]), "r"(v_init[24]), "r"(v_init[25]), "r"(v_init[26]), "r"(v_init[27]), "r"(v_init[28]), \
             "r"(v_init[29]), "r"(v_init[30]), "r"(v_init[31])\
        : "memory" \
        )

#define INIT_GPR_TO_ZERO()  \
        asm volatile(       \
        "mov x0, xzr\n"     \
        "mov x1, xzr\n"     \
        "mov x2, xzr\n"     \
        "mov x3, xzr\n"     \
        "mov x4, xzr\n"     \
        "mov x5, xzr\n"     \
        "mov x6, xzr\n"     \
        "mov x7, xzr\n"     \
        "mov x8, xzr\n"     \
        "mov x9, xzr\n"     \
        "mov x10, xzr\n"    \
        "mov x11, xzr\n"    \
        "mov x12, xzr\n"    \
        "mov x13, xzr\n"    \
        "mov x14, xzr\n"    \
        "mov x15, xzr\n"    \
        "mov x16, xzr\n"    \
        "mov x17, xzr\n"    \
        "mov x18, xzr\n"    \
        "mov x19, xzr\n"    \
        "mov x20, xzr\n"    \
        "mov x21, xzr\n"    \
        "mov x22, xzr\n"    \
        "mov x23, xzr\n"    \
        "mov x24, xzr\n"    \
        "mov x25, xzr\n"    \
        "mov x26, xzr\n"    \
        "mov x27, xzr\n"    \
        "mov x28, xzr\n"    \
        );

#define INIT_GPR_TO_NUM()  \
        asm volatile(       \
        "mov x0, #0x1cb1\n"     \
        "mov x1, #0xf521\n"     \
        "mov x2, #0x4cb3\n"     \
        "mov x3, #0x5464\n"     \
        "mov x4, #0x4adf\n"     \
        "mov x5, #0xd410\n"     \
        "mov x6, #0xe26f\n"     \
        "mov x7, #0x7e1f\n"     \
        "mov x8, #0x0e07\n"     \
        "mov x9, #0xcf24\n"     \
        "mov x10, #0x54dd\n"    \
        "mov x11, #0xe3e9\n"    \
        "mov x12, #0x52b3\n"    \
        "mov x13, #0xd1c9\n"    \
        "mov x14, #0x6d55\n"    \
        "mov x15, #0x5530\n"    \
        "mov x16, #0xcd07\n"    \
        "mov x17, #0x09bd\n"    \
        "mov x18, #0x99cd\n"    \
        "mov x19, #0x55d0\n"    \
        "mov x20, #0x3ab3\n"    \
        "mov x21, #0x15ad\n"    \
        "mov x22, #0xe100\n"    \
        "mov x23, #0xc42e\n"    \
        "mov x24, #0xa93a\n"    \
        "mov x25, #0x9dde\n"    \
        "mov x26, #0x4ef8\n"    \
        "mov x27, #0xd2cb\n"    \
        "mov x28, #0x9f6d\n"    \
        );

#define INIT_GPR_TO_64NUM()  \
        asm volatile(       \
        "movz x0, #0x3937, lsl #48\n" \
        "movk x0, #0x0d2c, lsl #32\n" \
        "movk x0, #0x4d41, lsl #16\n" \
        "movk x0, #0xe10d\n"     \
        "movz x1, #0x201e, lsl #48\n" \
        "movk x1, #0x0274, lsl #32\n" \
        "movk x1, #0x1233, lsl #16\n" \
        "movk x1, #0x1e26\n"     \
        "movz x2, #0x5a00, lsl #48\n" \
        "movk x2, #0x1878, lsl #32\n" \
        "movk x2, #0x3c46, lsl #16\n" \
        "movk x2, #0x9c72\n"     \
        "movz x3, #0x3fc8, lsl #48\n" \
        "movk x3, #0xdda1, lsl #32\n" \
        "movk x3, #0x1c4e, lsl #16\n" \
        "movk x3, #0x95bf\n"     \
        "movz x4, #0x6b48, lsl #48\n" \
        "movk x4, #0xfb76, lsl #32\n" \
        "movk x4, #0x0475, lsl #16\n" \
        "movk x4, #0x3702\n"     \
        "movz x5, #0x2b6e, lsl #48\n" \
        "movk x5, #0x24f0, lsl #32\n" \
        "movk x5, #0x4c3b, lsl #16\n" \
        "movk x5, #0x1ed7\n"     \
        "movz x6, #0x3d65, lsl #48\n" \
        "movk x6, #0x9ab3, lsl #32\n" \
        "movk x6, #0x598f, lsl #16\n" \
        "movk x6, #0x7d43\n"     \
        "movz x7, #0x5b68, lsl #48\n" \
        "movk x7, #0xa5fd, lsl #32\n" \
        "movk x7, #0x3369, lsl #16\n" \
        "movk x7, #0xb3ee\n"     \
        "movz x9, #0x3ec0, lsl #48\n" \
        "movk x9, #0xcd63, lsl #32\n" \
        "movk x9, #0x63ec, lsl #16\n" \
        "movk x9, #0x3ec5\n"     \
        "movz x10, #0x2b0c, lsl #48\n" \
        "movk x10, #0xe91c, lsl #32\n" \
        "movk x10, #0x5c9c, lsl #16\n" \
        "movk x10, #0x4359\n"     \
        "movz x11, #0x1709, lsl #48\n" \
        "movk x11, #0x75e8, lsl #32\n" \
        "movk x11, #0x6e1b, lsl #16\n" \
        "movk x11, #0xde29\n"     \
        "movz x12, #0x6796, lsl #48\n" \
        "movk x12, #0x4c91, lsl #32\n" \
        "movk x12, #0x3c7c, lsl #16\n" \
        "movk x12, #0xf8b9\n"     \
        "movz x13, #0x30ad, lsl #48\n" \
        "movk x13, #0x170e, lsl #32\n" \
        "movk x13, #0x33c4, lsl #16\n" \
        "movk x13, #0xb11a\n"     \
        "movz x14, #0x3646, lsl #48\n" \
        "movk x14, #0xfadc, lsl #32\n" \
        "movk x14, #0x6dff, lsl #16\n" \
        "movk x14, #0x9d5b\n"     \
        "movz x15, #0x28b3, lsl #48\n" \
        "movk x15, #0xb821, lsl #32\n" \
        "movk x15, #0x2efd, lsl #16\n" \
        "movk x15, #0x9368\n"     \
        "movz x16, #0x09d8, lsl #48\n" \
        "movk x16, #0x674b, lsl #32\n" \
        "movk x16, #0x5e65, lsl #16\n" \
        "movk x16, #0x71fa\n"     \
        "movz x17, #0x561d, lsl #48\n" \
        "movk x17, #0xc2d9, lsl #32\n" \
        "movk x17, #0x3be3, lsl #16\n" \
        "movk x17, #0x60bb\n"     \
        "movz x18, #0x4ce3, lsl #48\n" \
        "movk x18, #0x720d, lsl #32\n" \
        "movk x18, #0x6854, lsl #16\n" \
        "movk x18, #0xdaeb\n"     \
        "movz x19, #0x1af0, lsl #48\n" \
        "movk x19, #0x6e29, lsl #32\n" \
        "movk x19, #0x1ed0, lsl #16\n" \
        "movk x19, #0x5391\n"     \
        "movz x20, #0x7f1e, lsl #48\n" \
        "movk x20, #0x22bd, lsl #32\n" \
        "movk x20, #0x137a, lsl #16\n" \
        "movk x20, #0xe77f\n"     \
        "movz x21, #0x69f4, lsl #48\n" \
        "movk x21, #0x0b2f, lsl #32\n" \
        "movk x21, #0x150a, lsl #16\n" \
        "movk x21, #0x7102\n"     \
        "movz x22, #0x6081, lsl #48\n" \
        "movk x22, #0x0371, lsl #32\n" \
        "movk x22, #0x3609, lsl #16\n" \
        "movk x22, #0x2137\n"     \
        "movz x23, #0x1163, lsl #48\n" \
        "movk x23, #0x7bf1, lsl #32\n" \
        "movk x23, #0x1a5e, lsl #16\n" \
        "movk x23, #0x1423\n"     \
        "movz x24, #0x167b, lsl #48\n" \
        "movk x24, #0xd1b4, lsl #32\n" \
        "movk x24, #0x1b04, lsl #16\n" \
        "movk x24, #0x8bf5\n"     \
        "movz x25, #0x4780, lsl #48\n" \
        "movk x25, #0x4e07, lsl #32\n" \
        "movk x25, #0x3882, lsl #16\n" \
        "movk x25, #0xd23e\n"     \
        "movz x26, #0x14b8, lsl #48\n" \
        "movk x26, #0xbfc7, lsl #32\n" \
        "movk x26, #0x6d36, lsl #16\n" \
        "movk x26, #0xac72\n"     \
        "movz x27, #0x2073, lsl #48\n" \
        "movk x27, #0x66bb, lsl #32\n" \
        "movk x27, #0x785d, lsl #16\n" \
        "movk x27, #0x8fa2\n"     \
        "movz x28, #0x4680, lsl #48\n" \
        "movk x28, #0xd0c3, lsl #32\n" \
        "movk x28, #0x3309, lsl #16\n" \
        "movk x28, #0xd65f\n"     \
        ::\
        : "x1","x2","x3","x4","x5","x6","x7", "x9","x10","x11","x12","x13","x14","x15","x16","x17","x18","x19","x20","x21","x22", \
        "x23","x24","x25","x26","x27","x28" \
        );
#define INIT_GPR_TO_MEM()   \
        asm volatile(       \
        "mov x0, %0\n"      \
        "mov x1, %0\n"      \
        "mov x2, %0\n"      \
        "mov x3, %0\n"      \
        "mov x4, %0\n"      \
        "mov x5, %0\n"      \
        "mov x6, %0\n"      \
        "mov x7, %0\n"      \
        "mov x8, %0\n"      \
        "mov x9, %0\n"      \
        "mov x10, %0\n"     \
        "mov x11, %0\n"     \
        "mov x12, %0\n"     \
        "mov x13, %0\n"     \
        "mov x14, %0\n"     \
        "mov x15, %0\n"     \
        "mov x16, %0\n"     \
        "mov x17, %0\n"     \
        "mov x18, %0\n"     \
        "mov x19, %0\n"     \
        "mov x20, %0\n"     \
        "mov x21, %0\n"     \
        "mov x22, %0\n"     \
        "mov x23, %0\n"     \
        "mov x24, %0\n"     \
        "mov x25, %0\n"     \
        "mov x26, %0\n"     \
        "mov x27, %0\n"     \
        "mov x28, %0\n"     \
        :                   \
        :"r"(test_pointer)\
        : "x1","x2","x3","x4","x5","x6","x7","x9","x10","x11","x12","x13","x14","x15","x16","x17","x18","x19","x20","x21","x22", \
        "x23","x24","x25","x26","x27","x28" \
        );

// N == V && Z == 0
// cond1 --> cond2 (changed)
#define SET_CPSR_COND2()  \
    asm volatile(       \
    "mov x8, #0xB0000000\n"      \
    "msr nzcv, x8\n"    \
    :::"x8"             \
    );                  \

// N != V && Z == 1 && C == 1
// cond2 --> cond1 (changed)
#define SET_CPSR_COND1()  \
    asm volatile(       \
    "mov x8, #0xE0000000\n"      \
    "msr nzcv, x8\n"    \
    :::"x8"           \
    ); 

#define SET_CPSR_COND3()  \
    asm volatile(       \
    "mov x8, #0x0\n"      \
    "msr nzcv, x8\n"    \
    :::"x8"             \
    ); 

#define SET_CPSR_COND4()  \
    asm volatile(       \
    "mov x8, #0xF0000000\n"      \
    "msr nzcv, x8\n"    \
    :::"x8"\
); 

#define BRANCH_INSTR()  \
            asm volatile(   \
            "blr %[ptr]"    \
            ::[ptr]"r"(instr_pointer)   \
            );  

#define MAX_LENGTH 0xFFFFFFFF
#define PAGE_SIZE  0x04000000


/* csh handle; */
/* cs_insn *cs_instr; */

//gobal struct
struct Result {
    int signo;
    uint64_t addr;
};
struct Result result;

//condition bit struct
uint32_t instrBitsFlag = 0x0;
enum BitsFlags {
    GPR_flag = 0x1,  // Changed General Purpose Register?
    VPR_flag = 0x2, // Changed Vector Purpose Regiseter?
    Forged_flag = 0x4,  // enable == forged, disable == orignal
    ALU_flag = 0x8,    // Is ALU Instruction
    BR_flag = 0x10,    // Is Branch Instruction
    LS_flag = 0x20,     // Is Load/Store Instruction
    NOP_flag = 0x40,
    COND_flag = 0x80,
    STAGE1_flag = 0x100, 
    STAGE2_flag = 0x200, // MEM_flag
    STAGE3_flag = 0x400, // BR_STAGE_flag
    STAGE4_flag = 0x800, // COND_STAGE_flag
    STAGE5_flag = 0x1000,
    ALARM_flag = 0x2000, // SIGALARM
    ADJ_flag = 0x4000,
    SP_flag = 0x8000 //Special group(system, hint, barrier etc..)

};
const char *BitsFlagsNames[] ={
                        "GPR_flag",
                        "VPR_flag",
                        "Forged_flag",
                        "ALU_flag",
                        "BR_flag",
                        "LS_flag",
                        "NOP_flag",
                        "COND_flag",
                        "STAGE1_flag",
                        "STAGE2_flag",
                        "STAGE3_flag",
                        "STAGE4_flag",
                        "STAGE5_flag",
                        "ALARM_flag",
                        "ADJ_flag",
                        "SP_flag",
                        };

enum triaging_method_num {
    Normal = 0x0, //Undocumented instruction Triaging
    Forged = 0x1, //Compare Forged instruction and Orignal instruction
    Skip = 0x2, // ?? 이건 뭐냐.. 무슨 의도였지?
    Dry_run = 0x3 // 개 버러지 같이 첫번째 signal handler 들어가면 status 망가지는거 봐라 개 병신 컴 apple m1, 이걸 보고도 apple 잘만들었다 이지랄 하는 앱등이 있으면 나와봐
};
const char *methodName[] = {
                        "Undoc Instr",
                        "Forged Instr",
                        "Skip"
                        };

//gobal variable
jmp_buf jump_buffer;
jmp_buf st1, st2, st3, st4, st5;
uint8_t *Guard_page1, *Guard_page2;
uint8_t *instr_pointer;
uint32_t current_instr;
uint32_t adj_instr[6];
uint32_t original_instr;
int triage_Method = 0;
bool general_bit = false;
bool memory_bit = false;
bool use_memory = false;
bool Grp1, Grp2, Grp3;
uint32_t after_cpsr = 0x0;
uint8_t *test_pointer;
char orig_dis_instr[255] = {0};
char outString1[255];
char outString2[255];
char outString3[255];
char outString4[255];
uint32_t Base_CPSR = 0x0;
unsigned long current_stack;
unsigned char dummy_stack[2048] ={0};
unsigned char *dummy_ptr_fp; 
unsigned char *dummy_ptr_sp; 
unsigned char *dummy1;
void *code_mem;
int init_length;

//cycle check var
uint64_t nop_cycle;
uint64_t instr_cycle;

//Function pre define
/* void init_llvm(); */
void analyzing_result();
void result_log(uint64_t, int);
/* int checkForgedInst(); */
void sig_handler(int, siginfo_t*, void* );
void configure_handler(void (*)(int, siginfo_t*, void*));
void init_before_register();
void check_jit(void);
void arg_config(int, char**);
void file_read(FILE *);
static void adjust();
void init_vector_array(void);
void progress_bar(int, int, int);
static uint64_t cycle_check(uint32_t);
typedef void (*func_t)(void);
void stage1();
void stage2();
void stage3();
void stage4();
void stage5();
void busy_sleep_ms(long);

FILE* outfile;
FILE* undoc_out;
FILE* forged_out;
/* LLVMDisasmContextRef DCR; */

//For forged
volatile uint64_t fp_lr_sp[3] = {0};
//GPR Register
volatile uint64_t gpr_before[32] = {0};
volatile uint64_t gpr_after[32] = {0};
// Vector Register
volatile uint64_t vec_before[32][2] = {0};
volatile uint64_t vec_after[32][2] = {0};

asm(
        ".global _init_start     \n"
        "_init_start:            \n"
        "mov lr, #0xdead        \n"
        "lsl lr, lr, #16        \n"
        "mov x8, #0xbeaf        \n"
        "orr lr, lr, x8         \n"
        "movz x8, #0x60a3, lsl #48\n" 
        "movk x8, #0xa6f9, lsl #32\n" 
        "movk x8, #0x1827, lsl #16\n" 
        "movk x8, #0x5701\n"
        /* "mov x8, 0x0e07         \n" */
        ".global _init_end      \n"
        "_init_end:             \n"
);
extern char init_start, init_end;

void busy_sleep_ms(long milli){
    struct timespec start, current;
    long nanoseconds = milli * 1000000;
    long elapsed_ns = 0;

    if(clock_gettime(CLOCK_MONOTONIC, &start) == -1){
        return;
    }
    do{
        if(clock_gettime(CLOCK_MONOTONIC, &current) == -1){
            return;
        }
        elapsed_ns = (current.tv_sec - start.tv_sec) * 1000000000L + (current.tv_nsec - start.tv_nsec);
    }while(elapsed_ns < nanoseconds);
}

inline void progress_bar(int progress, int total, int method){

    int bar_width = 50;
    float percentage = (float)progress / total;
    int pos = bar_width * percentage;

    if(method == Normal){
        printf("\033[A\033[K");
        printf("\033[A\033[K");
    }
    else {
        printf("\033[A\033[K");
        printf("\033[A\033[K");
        printf("\033[A\033[K");
    }
    printf("\r[");
    
    for(int i=0; i< bar_width; i++){
        if(i < pos) printf("=");
        else if(i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%\(%d \/ %d\) \n",(int)(percentage * 100), progress, total);

}

uint64_t check_cycle(uint32_t inst){
    
    uint32_t *code;
    uint64_t start, end, result;
    code = code_mem;
    pthread_jit_write_protect_np(0);
    
    *code++ = 0xd5033fdf; // ISB
    *code++ = 0xd503201f; // NOP
    /* *code++ = 0xd284e209; // MOV x9, #10000 */
    // loop_start:
    for(int i = 0; i < 8000; i++){
        *code++ = inst;
    }
    /* *code++ = 0xf1000529; // SUBS x9, x9, #1 */
    /* *code++ = 0x54fc17e1; // B.NE loop start */
    *code++ = 0xd5033fdf; // ISB
    *code++ = 0xd65f03c0; // RET

    pthread_jit_write_protect_np(1);

    /* __builtin___clear_cache((char*)code_mem, (char*)code_mem + sizeof(code_mem)); */
    func_t func = (func_t)code_mem;

    start = mach_absolute_time();
    for(int i=0; i<100; i++){
        func();
    }
    end = mach_absolute_time();
    
    result = end - start;
    return result;

}


void init_vector_array(){
    float value = 1.1f;

    for(int i=0; i<32; i++){
        for(int k=0; k<4; k++){
            v_init[i][k] = value;
            value += 0.2f;
        }
    }
}
/*
void init_llvm(){
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllDisassemblers();
    DCR = LLVMCreateDisasmCPUFeatures("aarch64--macho", "generic", "+all", NULL, 0, NULL, NULL);
}
*/
/*
*   Forged bit Mask
*   Case1 : 20~16bit & 14~10bit --> return 1
*   Case2 : 20~16bit            --> return 1
*   Case3 : 14~10bit            --> return 1
*   Not Forged Instruction      --> return 0
*/
/*
int checkForgedInst(){

    uint32_t inst1 = current_instr | 0x1F7C00;  //20~16 & 14~10
    uint32_t inst2 = current_instr | 0x1F0000;  //20~16
    uint32_t inst3 = current_instr | 0x7C00;    //14~10
    uint32_t inst4 = current_instr;
    
    size_t vc1 = LLVMDisasmInstruction(DCR, &inst1, 4, 0x0, outString1, sizeof(outString1));
    size_t vc2 = LLVMDisasmInstruction(DCR, &inst2, 4, 0x0, outString2, sizeof(outString2));
    size_t vc3 = LLVMDisasmInstruction(DCR, &inst3, 4, 0x0, outString3, sizeof(outString3));
    size_t vc4 = LLVMDisasmInstruction(DCR, &inst4, 4, 0x0, outString4, sizeof(outString4));

    if(vc2 > 0){
        original_instr = inst2;
        fprintf(forged_out, "F:%lx [O:%s]\t\n", current_instr, outString2);
        printf("LLVM : %s\n",outString2);
        return Forged;
    }
    else if(vc3 > 0){
        original_instr = inst3;
        fprintf(forged_out, "F:%lx [O:%s]\t\n", current_instr, outString3);
        printf("LLVM : %s\n",outString3);
        return Forged;
    }
    else if(vc1 >0){
        original_instr = inst1;
        fprintf(forged_out, "F:%lx [O:%s]\t\n", current_instr, outString1);
        printf("LLVM : %s\n",outString1);
        return Forged;
    }
    else if(vc4 > 0){
        fprintf(forged_out, "F:%lx [O:%s]\t\n", current_instr, outString4);
        printf("False Positive : %s\n", outString4);
        return Skip;
    }
    else{
        // Not forged Instrcution
        // False Positive?
        // TODO : add Code
        return Normal;
    }

}
*/

void result_log(uint64_t instr, int reg_num){
    if(triage_Method == Normal)
        fprintf(undoc_out, "instr : %llx reg x%d is changed!\n", instr, reg_num);
    else
        fprintf(forged_out, "instr : %llx reg x%d is changed!\n", instr, reg_num);
}

void analyzing_result(){
    
    switch(triage_Method) {
        case Normal:
            /* usleep(1*1000); */
            if((instrBitsFlag & LS_flag) == LS_flag){
                instrBitsFlag &= ~ALU_flag;
                ItypePrint(undoc_out, methodName[triage_Method], current_instr, "LS");
            }
            else if((instrBitsFlag & COND_flag) == COND_flag){
                ItypePrint(undoc_out, methodName[triage_Method], current_instr, "COND");
            }
            else if((instrBitsFlag & BR_flag) == BR_flag){
                ItypePrint(undoc_out, methodName[triage_Method], current_instr, "BRANCH");
            }
            else if((instrBitsFlag & SP_flag) == SP_flag){
                ItypePrint(undoc_out, methodName[triage_Method], current_instr, "SPECIAL");
            }
            else if((instrBitsFlag & ALU_flag) == ALU_flag){
                ItypePrint(undoc_out, methodName[triage_Method], current_instr, "ALU");
            }
            else {
                ItypePrint(undoc_out, methodName[triage_Method], current_instr, "NOP");
            }

            fprintf(undoc_out, "Changed Register: ");
            /* for(int i=0; i<28; i++){ */
            /*     if(gpr_before[i] != gpr_after[i]){ */
            /*         instrBitsFlag |= GPR_flag; */
            /*         fprintf(undoc_out, "x%d ",i); */
            /*     } */
            /* } */
            
            fprintf(undoc_out, "\n");
            fprintf(undoc_out, "Changed Flag :");
            for(int i=0; i<31; i++){
                if(instrBitsFlag & (1<<i)){
                    fprintf(undoc_out, " %s", BitsFlagsNames[i]);
                }
            }
            
            fprintf(undoc_out, "\n\n");
            fflush(undoc_out);
            break;
        default:
            break;
    }
}

void init_before_register(){
    
    for(int i=1; i<32; i++){
        uint64_t temp = i << 8 | i;
        temp = temp << 16 | temp;
        temp = temp << 32 | temp;
        vec_before[i][0] = temp;
        vec_before[i][1] = temp;
    }

    uint64_t temp = 32 << 8 | 32;
    temp = temp << 16 | temp;
    temp = temp << 32 | temp;
    vec_before[0][0] = temp;
    vec_before[0][1] = temp;
    
    if(1){
        gpr_before[0] = 0x39370d2c4d41e10d;
        gpr_before[1]= 0x201e027412331e26;
        gpr_before[2]= 0x5a0018783c469c72;
        gpr_before[3]= 0x3fc8dda11c4e95bf;
        gpr_before[4]= 0x6b48fb7604753702;
        gpr_before[5]= 0x2b6e24f04c3b1ed7;
        gpr_before[6]= 0x3d659ab3598f7d43;
        gpr_before[7]= 0x5b68a5fd3369b3ee;
        gpr_before[8]= 0x60a3a6f918275701;
        gpr_before[9]= 0x3ec0cd6363ec3ec5;
        gpr_before[10]= 0x2b0ce91c5c9c4359;
        gpr_before[11]= 0x170975e86e1bde29;
        gpr_before[12]= 0x67964c913c7cf8b9;
        gpr_before[13]= 0x30ad170e33c4b11a;
        gpr_before[14]= 0x3646fadc6dff9d5b;
        gpr_before[15]= 0x28b3b8212efd9368;
        gpr_before[16]= 0x9d8674b5e6571fa;
        gpr_before[17]= 0x561dc2d93be360bb;
        gpr_before[18]= 0x4ce3720d6854daeb;
        gpr_before[19]= 0x1af06e291ed05391;
        gpr_before[20]= 0x7f1e22bd137ae77f;
        gpr_before[21]= 0x69f40b2f150a7102;
        gpr_before[22]= 0x6081037136092137;
        gpr_before[23]= 0x11637bf11a5e1423;
        gpr_before[24]= 0x167bd1b41b048bf5;
        gpr_before[25]= 0x47804e073882d23e;
        gpr_before[26]= 0x14b8bfc76d36ac72;
        gpr_before[27]= 0x207366bb785d8fa2;
        gpr_before[28]= 0x4680d0c33309d65f;

    }
    else {
        gpr_before[0] = 0x1cb1;
        gpr_before[1] = 0xf521;
        gpr_before[2] = 0x4cb3;
        gpr_before[3] = 0x5464;
        gpr_before[4] = 0x4adf;
        gpr_before[5] = 0xd410;
        gpr_before[6] = 0xe26f;
        gpr_before[7] = 0x7e1f;
        gpr_before[8] = 0x0e07;
        gpr_before[9] = 0xcf24;
        gpr_before[10] = 0x54dd;
        gpr_before[11] = 0xe3e9;
        gpr_before[12] = 0x52b3;
        gpr_before[13] = 0xd1c9;
        gpr_before[14] = 0x6d55;
        gpr_before[15] = 0x5530;
        gpr_before[16] = 0xcd07;
        gpr_before[17] = 0x09bd;
        gpr_before[18] = 0x99cd;
        gpr_before[19] = 0x55d0;
        gpr_before[20] = 0x3ab3;
        gpr_before[21] = 0x15ad;
        gpr_before[22] = 0xe100;
        gpr_before[23] = 0xc42e;
        gpr_before[24] = 0xa93a;
        gpr_before[25] = 0x9dde;
        gpr_before[26] = 0x4ef8;
        gpr_before[27] = 0xd2cb;
        gpr_before[28] = 0x9f6d;
    }

}

void init_before_memregister(){
    for(int i =0; i<32; i++)
        gpr_before[i] = test_pointer;
}

void check_jit(){
    
    if(pthread_jit_write_protect_supported_np() == 1){
        #define JIT
        printf("jit write supported\n");
    }
    else {
        printf("jit write not supported\n");
    }
    
}

void sig_handler(int signo, siginfo_t* si, void *p){
    
    /* usleep(1*1000); */
    uint32_t pre_stage = instrBitsFlag & 0x1F00;
    ucontext_t *uc = (ucontext_t*)p;
    uint32_t cpsr_temp = uc->uc_mcontext->__ss.__cpsr;
    uint64_t *gpr_ptr = (uint64_t *)&uc->uc_mcontext->__ss;
    /* uint64_t  *pc_addr = uc->uc_mcontext->__ss.__opaque_pc; */
    /* uint64_t  pc_val = (uint64_t)uc->uc_mcontext->__ss.__opaque_pc; */
    /* uint64_t pc_addr = pc_val & 0x0000FFFFFFFFFFFF; */
    fp_lr_sp[1] = fp_lr_sp[1] & 0x0000FFFFFFFFFFFF;
    uint64_t *pc_addr = uc->uc_mcontext->__ss.__pc;
    if(signo == SIGALRM){
        /* alarm(0); */
        instrBitsFlag &= 0x0;
        instrBitsFlag |= ALARM_flag;
        goto end;
    }
    
    usleep(1*1000);
    
check_prework: 
    if(triage_Method == Normal){
        switch(instrBitsFlag & 0x1F00) {
            case STAGE1_flag:
                /* if(pc_addr != (uint64_t)(instr_pointer+init_length) && pc_addr != (uint64_t)(instr_pointer+init_length+4)){ */
                /*     instrBitsFlag &= ~STAGE1_flag; */
                /*     instrBitsFlag |= BR_flag; */
                /*     #<{(| goto Save_normal; |)}># */
                /*     goto end; */
                /* } */
                if((pc_addr == 0xdeadbeaf)){
                    instrBitsFlag &= ~STAGE1_flag;
                    instrBitsFlag |= BR_flag;
                    goto end;
                }
                if((signo == SIGILL) && (pc_addr == (instr_pointer+init_length))){
                    instrBitsFlag &= ~STAGE1_flag;
                    instrBitsFlag |= SP_flag;
                    goto end;
                }
                else if((signo == SIGILL) && (pc_addr == (instr_pointer+init_length+4))){
                    instrBitsFlag &= ~STAGE1_flag;
                    instrBitsFlag |= STAGE4_flag;
                    goto end;
                }
                else if(signo == SIGSEGV){
                    instrBitsFlag &= ~STAGE1_flag;
                    instrBitsFlag |= STAGE2_flag; // go Mem or Branch 
                    goto end;
                }
                else if(signo == SIGBUS){ // If mem : fp, sp or RET
                    instrBitsFlag &= ~STAGE1_flag;
                    instrBitsFlag |= STAGE2_flag; // go Mem or Branch 
                    goto end;
                }
                else if(signo == SIGILL){ // if branch pc+offset
                    instrBitsFlag &= ~STAGE1_flag;
                    instrBitsFlag |= BR_flag;
                    goto end;
                }
                else {  // SIGTRAP
                    instrBitsFlag &= ~STAGE1_flag;
                    instrBitsFlag |= STAGE4_flag;
                    goto Save_normal;
                }
            case STAGE2_flag:
                if(signo == SIGILL){
                    instrBitsFlag &= ~STAGE2_flag;
                    instrBitsFlag |= BR_flag; // enable BR_flag
                    /* goto Save_normal; */
                    goto end;
                }
                else if(signo == SIGBUS || signo == SIGSEGV){ //ret call도 여기 아님?
                    instrBitsFlag &= ~STAGE2_flag;
                    instrBitsFlag |= LS_flag; // enable LS_flag
                    /* goto Save_normal; */
                    goto end;
                }
                else {
                    instrBitsFlag &= ~STAGE2_flag;
                    instrBitsFlag |= STAGE3_flag;
                    goto end;
                }
            case STAGE3_flag:
                if(signo == SIGILL){
                    instrBitsFlag &= ~STAGE3_flag;
                    instrBitsFlag |= BR_flag;
                    /* goto Save_normal; */
                    goto end;
                }
                else {
                    instrBitsFlag &= ~STAGE3_flag;
                    instrBitsFlag |= LS_flag;
                    /* goto Save_normal; */
                    goto end;
                }
            case STAGE4_flag:
                cpsr_temp = cpsr_temp & 0xF0000000;
                cpsr_temp = cpsr_temp ^ 0x0;
                //conditional branch가 여기서 작동할 수도 있다
                //STAGE4의 flag init은 0x0 이기 때문이다.
                if(signo == SIGSEGV | signo == SIGBUS){
                    instrBitsFlag &= ~STAGE4_flag;
                    instrBitsFlag |= STAGE3_flag;
                    goto end;
                }
                if(cpsr_temp == 0){
                    instrBitsFlag &= ~STAGE4_flag;
                    instrBitsFlag |= STAGE5_flag;
                    goto Save_normal;
                }
                else{
                    instrBitsFlag &= ~STAGE4_flag;
                    instrBitsFlag |= COND_flag;
                    goto end;
                }
            case STAGE5_flag:
                cpsr_temp = cpsr_temp & 0xF0000000;
                cpsr_temp = cpsr_temp ^ 0xF0000000;
                if(cpsr_temp == 0){
                    instrBitsFlag &= ~STAGE5_flag;
                    instrBitsFlag |= NOP_flag;
                    goto Save_normal;
                }
                else {
                    instrBitsFlag &= ~STAGE5_flag;
                    instrBitsFlag |= COND_flag;
                    goto Save_normal;
                }
            default:
                break;
        }
    }
    else {
        goto end;
    }
    
Save_normal: // Save Undocumented instruction status
    /* usleep(1*1000); */
    /* busy_sleep_ms(1); */
    for(int i=0; i<32; i++){
        /* uint64_t *temp = (uint64_t *)&uc->uc_mcontext->__ns.__v[i]; */
        uint32_t *v_reg_hex = (uint32_t *)&uc->uc_mcontext->__ns.__v[i];
        uint32_t *v_init_hex = (uint32_t *)v_init[i];
            
        gpr_after[i] = *(gpr_ptr + i);
        /* if(i == 29 || i == 31) */
        /*     gpr_after[i] = gpr_after[i] & 0x00000FFFFFFFFF; */
        if((gpr_before[i] != (gpr_after[i]))){
            instrBitsFlag &= ~(0x1F40); // unmaks All stage and NOP flag
            instrBitsFlag |= ALU_flag;
            break;
            /* instrBitsFlag &= ~STAGE4_flag; */
        }
        for(int k=0; k<4; k++){
            if(v_reg_hex[k] != v_init_hex[k]){
                instrBitsFlag &= ~NOP_flag;
                instrBitsFlag |= ALU_flag;
                instrBitsFlag |= VPR_flag;
            }
        }
    }
    after_cpsr = uc->uc_mcontext->__ss.__cpsr;
    /* uc->uc_mcontext->__ss.__x[29] = gpr_before[29]; // 무슨 이유인지 모르겠는데 fp가 망가진다 recovery 해줘야함 */
    goto end;
    
end:
    /* sleep(1); */
    /* uc->uc_mcontext->__ss.__lr = gpr_before[30]; */
    uc->uc_mcontext->__ss.__sp = fp_lr_sp[2];
    uc->uc_mcontext->__ss.__lr = fp_lr_sp[1];
    uc->uc_mcontext->__ss.__fp = fp_lr_sp[0];
    asm volatile("ISB sy\n");
    if(pre_stage == STAGE1_flag) longjmp(st1, 1);
    else if(pre_stage == STAGE2_flag) longjmp(st2, 1);
    else if(pre_stage == STAGE3_flag) longjmp(st3, 1);
    else if(pre_stage == STAGE4_flag) longjmp(st4, 1);
    else if(pre_stage == STAGE5_flag) longjmp(st5, 1);
    else longjmp(st1, 1); // for dry run only

}

void configure_handler(void (*handler)(int, siginfo_t*, void*)){
    
    struct sigaction s;
   
    s.sa_sigaction = sig_handler;   
    s.sa_flags = SA_SIGINFO | SA_ONSTACK;
    
    sigaction(SIGILL, &s, NULL);
    sigaction(SIGSEGV, &s, NULL);
    sigaction(SIGTRAP, &s, NULL);
    sigaction(SIGBUS, &s, NULL);
    sigaction(SIGALRM, &s, NULL);

}

void adjust(){
 
    uint16_t mask = 0xd5000000;
    int adj_value[3] = {0};
    int max =0, idx= 0;
    
    /* if((current_instr & mask) == 0){ // 명령어의 끝이 0000이라 값을 뺄때 opcode가 달리는것을 방지 */
    /* if(current_instr == 0xd503201f) */
    if(current_instr & mask)
        goto adj_skip;
    adj_instr[0] = current_instr+1;  
    adj_instr[1] = current_instr+2;  
    adj_instr[2] = current_instr+3;
    adj_instr[3] = current_instr+4;  
    adj_instr[4] = current_instr+5;  
    adj_instr[5] = current_instr+6;  
    /* } */
    /* else { */
    /*     adj_instr[0] = current_instr-3;   */
    /*     adj_instr[1] = current_instr-2;   */
    /*     adj_instr[2] = current_instr-1;   */
    /*     adj_instr[3] = current_instr+1;   */
    /*     adj_instr[4] = current_instr+2;   */
    /*     adj_instr[5] = current_instr+3;   */
    /* } */
    
    for(int i=0; i<6; i++){
        instrBitsFlag &= 0x0;   //init instrBitsFlag
        pthread_jit_write_protect_np(0);
        memcpy(instr_pointer+init_length, &adj_instr[i], 4);
        pthread_jit_write_protect_np(1);
        
        instrBitsFlag |= STAGE1_flag;
        while((instrBitsFlag & 0x1F00) != 0x0){
            /* alarm(3); */
            dummy_ptr_fp = dummy1;
            dummy_ptr_sp = dummy1+0x10;
            gpr_before[29] = dummy_ptr_fp; //fp register
            gpr_before[30] = 0xdeadbeaf; //lr register
            gpr_before[31] = dummy_ptr_sp; //sp register

            if((instrBitsFlag & STAGE1_flag) == STAGE1_flag){
                init_before_register();
                stage1();
            }
            else if((instrBitsFlag & STAGE2_flag) == STAGE2_flag){
                init_before_memregister();
                stage2();
            }
            else if((instrBitsFlag & STAGE3_flag) == STAGE3_flag){
                init_before_memregister();
                stage3();
            }
            else if((instrBitsFlag & STAGE4_flag) == STAGE4_flag){
                init_before_register();
                stage4();
            }
            else if((instrBitsFlag & STAGE5_flag) == STAGE5_flag){
                init_before_register();
                stage5();
            }
            /* alarm(0); */
        }
        if((instrBitsFlag & ALU_flag) == ALU_flag)
            adj_value[0]++;
        else if((instrBitsFlag & BR_flag) == BR_flag)
            adj_value[1]++;
        else if((instrBitsFlag & SP_flag) == SP_flag)
            adj_value[2]++;
    }
    
    for(int i=0; i < 3; i++){
        if(adj_value[i] > max){
            max  = adj_value[i];
            idx = i;
        }
    }
    
    switch(idx){
        case 0:
            instrBitsFlag &= 0x0;
            instrBitsFlag |= ALU_flag;
            break;
        case 1:
            instrBitsFlag &= 0x0;
            instrBitsFlag |= BR_flag;
            break;
        case 2:
            instrBitsFlag &= 0x0;
            instrBitsFlag |= SP_flag;
            break;
        default:
            instrBitsFlag &= 0x0;
            instrBitsFlag |= NOP_flag;
    }

adj_skip:
        instrBitsFlag |= ADJ_flag;

}

void stage1(){
    
    if(setjmp(st1) == 0){
        
        asm volatile(
        "mov %0, fp\n"
        "mov %1, lr\n"
        :"=r"(fp_lr_sp[0]), "=r"(fp_lr_sp[1])
        :: "fp", "lr", "x8", "x9"
        );

        INIT_VECTOR_REG2();
        INIT_VECTOR_REG3();
        asm volatile(
        "mov %[current], sp\n"
        :[current]"=r"(fp_lr_sp[2])
        ::
        );
        
        asm volatile(
        "mov sp, %[sp]\n"
        "mov fp, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp)
        :"sp","fp"
        );
        
        INIT_GPR_TO_64NUM();
        SET_CPSR_COND2();
       
        asm volatile(
        "blr %[ptr]"
        ::[ptr]"r"(instr_pointer)
        );
    }
    else {
        asm volatile("isb sy\n");
        /* asm volatile( */
        /* "mov sp, %[current]\n" */
        /* "mov x29, %[x29]\n" */
        /* "mov x30, %[x30]\n" */
        /* :: */
        /* [current]"r"(current_stack), */
        /* [x29]"r"(gpr_before[29]), */
        /* [x30]"r"(gpr_before[30]) */
        /* :"sp","fp","lr" */
        /* ); */
    }
}

void stage2(){
    
    if(setjmp(st2) == 0){
        
        asm volatile(
        "mov %0, fp\n"
        "mov %1, lr\n"
        :"=r"(fp_lr_sp[0]), "=r"(fp_lr_sp[1])
        :: "fp", "lr", "x8", "x9"
        );

        INIT_VECTOR_REG2();
        INIT_VECTOR_REG3();
        asm volatile(
        "mov %[current], sp\n"
        :[current]"=r"(fp_lr_sp[2])
        ::
        );
        
        asm volatile(
        "mov sp, %[sp]\n"
        "mov fp, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp)
        :"sp","fp"
        );
        
        INIT_GPR_TO_MEM();
        SET_CPSR_COND1();
     
        asm volatile(
        "blr %[ptr]"
        ::[ptr]"r"(instr_pointer)
        );
        
    }
    else {
        asm volatile("isb sy\n");
    }

}

void stage3(){


    if(setjmp(st3) == 0){
        
        asm volatile(
        "mov %0, fp\n"
        "mov %1, lr\n"
        :"=r"(fp_lr_sp[0]), "=r"(fp_lr_sp[1])
        :: "fp", "lr", "x8", "x9"
        );

        INIT_VECTOR_REG2();
        INIT_VECTOR_REG3();
        asm volatile(
        "mov %[current], sp\n"
        :[current]"=r"(fp_lr_sp[2])
        ::
        );
        
        asm volatile(
        "mov sp, %[sp]\n"
        "mov fp, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp)
        :"sp","fp"
        );
        
        INIT_GPR_TO_MEM();
        SET_CPSR_COND3();
     
        asm volatile(
        "blr %[ptr]"
        ::[ptr]"r"(instr_pointer)
        );
    }
    else {
        asm volatile("isb sy\n");
    }
}

void stage4(){

    
    if(setjmp(st4) == 0){
        
        asm volatile(
        "mov %0, fp\n"
        "mov %1, lr\n"
        :"=r"(fp_lr_sp[0]), "=r"(fp_lr_sp[1])
        :: "fp", "lr", "x8", "x9"
        );

        INIT_VECTOR_REG2();
        INIT_VECTOR_REG3();
        
        asm volatile(
        "mov %[current], sp\n"
        :[current]"=r"(fp_lr_sp[2])
        ::
        );
        
        asm volatile(
        "mov sp, %[sp]\n"
        "mov fp, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp)
        :"sp","fp"
        );

        INIT_GPR_TO_64NUM();
        SET_CPSR_COND3();
     
        asm volatile(
        "blr %[ptr]"
        ::[ptr]"r"(instr_pointer)
        );
    }
    else {
        asm volatile("isb sy\n");
    }
}

void stage5(){

    if(setjmp(st5) == 0){
        
        asm volatile(
        "mov %0, fp\n"
        "mov %1, lr\n"
        :"=r"(fp_lr_sp[0]), "=r"(fp_lr_sp[1])
        :: "fp", "lr", "x8", "x9"
        );

        INIT_VECTOR_REG2();
        INIT_VECTOR_REG3();

        asm volatile(
        "mov %[current], sp\n"
        :[current]"=r"(fp_lr_sp[2])
        ::
        );
        
        asm volatile(
        "mov sp, %[sp]\n"
        "mov fp, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp)
        :"sp","fp"
        );

        INIT_GPR_TO_64NUM();
        SET_CPSR_COND4();
     
        asm volatile(
        "blr %[ptr]"
        ::[ptr]"r"(instr_pointer)
        );

    }
    else {
        asm volatile("isb sy\n");
    }
}   

int main(int argc, char** argv) {
    
    FILE *input_file;
    char line[1024];
    char *token;
    char *del = " ";
    char *new_line;
    uint32_t number=0x0;
    uint32_t init_x8 = 0xd29ea648; // mov x8, #0xf532
    uint32_t init_lr = 0xd280021e;
    /* uint32_t set_trap = 0x00000000; // set_udf#<{(|   |)}>#for debugging set_trap */
    uint32_t set_trap = 0xd4200000; // set_trap for runtime set_trap
    int total = 0;
    int progress = 0;
    char *fileName;
    char *outFile;
    stack_t ss;

    if(argc != 4) {
        printf("Usage: %s <file name> <file line>\n", argv[0]);
        exit(1);
    }

    total = atoi(argv[2]);
    fileName = argv[1];
    outFile = argv[3];
#ifdef MacOS
    check_jit();
    Guard_page1 = mmap(NULL, PAGE_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANON, 0,0);
    instr_pointer = mmap(Guard_page1, PAGE_SIZE, PROT_READ|PROT_EXEC|PROT_WRITE, MAP_PRIVATE|MAP_ANON|MAP_JIT, 0, 0);
    Guard_page2 = mmap(instr_pointer, PAGE_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANON,0,0);
    test_pointer = instr_pointer+0x80; // unaliged memory를 넣어 SIGBUS를 발생시키는 것이 목적
    printf("GP1 : %p\nIP : %p\nGP2 : %p\n", Guard_page1, instr_pointer, Guard_page2);
#endif
    
    init_length = (void *)&init_end - (void *)&init_start;
    printf("instr_pointer : %p\n", instr_pointer);
    /* instr_pointer = instr_pointer+ 0x3ff4; */
    instr_pointer = instr_pointer+ PAGE_SIZE - init_length - 8;
    printf("instr_pointer : %p\n", instr_pointer);
    assert(((uintptr_t)instr_pointer % 4) == 0);
    
#ifdef MacOS
    pthread_jit_write_protect_np(0);
#endif
    
    if((instr_pointer-0x3ffc) == MAP_FAILED){
        perror("mmap");
        exit(-1);
    }
    
    /* undoc_out = fopen("undoc_result.txt", "w"); */
    undoc_out = fopen(outFile, "w");
    forged_out = fopen("forged_result.txt", "w");
    /* input_file = fopen("threadH2", "r"); */
    input_file = fopen(fileName, "r");
    if(input_file == NULL) {
        perror("failed to open threadH");
        return 1;
    }

    memcpy(instr_pointer, (void *)&init_start, init_length);
    memcpy(instr_pointer+init_length+4, &set_trap, 4);
    
    /* dummy_ptr = (unsigned char *)(((uintptr_t)dummy_stack + 15) & ~15) + sizeof(dummy_stack); */
    memset(dummy_stack, 0, sizeof(dummy_stack));

    /* dummy1 = mmap(NULL, 0x2000, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,-1,0); */
    /* ((volatile char *)dummy1)[0] = 0; */
    /* if(mprotect(dummy1, sizeof(dummy1), PROT_NONE) != 0){ */
    /*     perror("mprotect"); */
    /*     return 1; */
    /* } */
    dummy1 = mmap(NULL, 0x8000, PROT_NONE, MAP_ANON | MAP_PRIVATE,-1,0) + 0x1000;
    dummy1 = (((uintptr_t)dummy1 +15) & ~15) + 1024;

    if((ss.ss_sp = malloc(SIGSTKSZ))==NULL){
        perror("stack alloc");
        exit(-1);
    }
    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;
    if(sigaltstack(&ss,0) < 0){
        perror("sigaltstack");
        exit(-1);
    }

    if(dummy1 == NULL){
        printf("dummy1 failed\n");
        exit(-2);
    }
    pthread_jit_write_protect_np(1);
    configure_handler(sig_handler); 
    /* init_llvm(); */
    
   /*
    *   Forged bit Mask
    *   Mask1 : 20~16bit & 14~10bit --> return 0
    *   Mask2 : 20~16bit            --> return 1
    *   Mask3 : 14~10bit            --> return 2
    *   Not Forged Instruction      --> return 3
    *
    */

    //dry run
    /* triage_Method = Dry_run; */
    init_vector_array();
    
    code_mem = mmap(NULL, 0x8000, PROT_READ | PROT_WRITE | PROT_EXEC,  MAP_ANON | MAP_PRIVATE | MAP_JIT, 0, 0);
    
    if(code_mem == MAP_FAILED) {
        perror("check cycle map failed");
        exit(-12);
    }
    //check nop cycle
    /* nop_cycle = check_cycle(0xd503201f); */
    nop_cycle = 320;
    printf("base cycle = %llu\n\n\n", nop_cycle);

    triage_Method = 123;
    //dry run
    do {
        /* dummy_ptr = dummy1; */
        /* gpr_before[29] = dummy_ptr; */
        /* gpr_before[30] = dummy_ptr; */
        /* gpr_before[31] = dummy_ptr; */
        /* init_before_register(); */
        stage1();
    } while(0);

    asm volatile("isb\n");
    sleep(1); // why? sync?

    while(fgets(line, 1024, input_file) != NULL) {

        /* asm volatile(".inst 0x00201220\n"); */
        busy_sleep_ms(1);
        progress++;
        /* printf("proegress : %lld\n", progress); */
        progress_bar(progress, total, triage_Method);
        if(progress % 1000 == 0)
            busy_sleep_ms(100);
        //Bitflags init func 필요
        instrBitsFlag &= 0x0;   //init instrBitsFlag
        token = strtok(line, del);
        
        if(token == NULL){
            continue;
        }
        token = strtok(NULL, del);
        if(token == NULL){
            continue;
        }
        number = strtoull(token, NULL, 16);
        current_instr = number;
        /* triage_Method = checkForgedInst(); //current Inst is Forged? */
        triage_Method = Normal;
        switch(triage_Method){
        
        case Normal: // General Undocumented Instruction Triaging
#ifdef MacOS
            pthread_jit_write_protect_np(0);
            printf("Undoc instr: %llx\n", current_instr); // Undocumented Instruction Triaging
            memcpy(instr_pointer+init_length, &current_instr, 4);
            pthread_jit_write_protect_np(1);
#endif      
            instrBitsFlag |= STAGE1_flag;
            while((instrBitsFlag & 0x1F00) != 0x0){
                
                dummy_ptr_fp = dummy1;
                dummy_ptr_sp = dummy1+0x10;
                gpr_before[29] = dummy_ptr_fp; //fp register
                gpr_before[30] = 0xdeadbeaf; //lr register
                gpr_before[31] = dummy_ptr_sp; //sp register
                
                /* printf("instrBitsFlag : %x\n", instrBitsFlag); */
                if((instrBitsFlag & STAGE1_flag) == STAGE1_flag){
                    init_before_register();
                    stage1();
                }
                else if((instrBitsFlag & STAGE2_flag) == STAGE2_flag){
                    init_before_memregister();
                    stage2();
                }
                else if((instrBitsFlag & STAGE3_flag) == STAGE3_flag){
                    init_before_memregister();
                    stage3();
                }
                else if((instrBitsFlag & STAGE4_flag) == STAGE4_flag){
                    init_before_register();
                    stage4();
                }
                else if((instrBitsFlag & STAGE5_flag) == STAGE5_flag){
                    init_before_register();
                    stage5();
                }
            }
            if((instrBitsFlag & NOP_flag) == NOP_flag){
                adjust();
            }
            if((instrBitsFlag & NOP_flag) == NOP_flag){
                instr_cycle = check_cycle(current_instr);
               if(instr_cycle > 3*nop_cycle){
                    instrBitsFlag &= 0x0;
                    instrBitsFlag |= SP_flag;
                    memset(code_mem, 0, sizeof(code_mem));
               }
            }
        /* asm volatile(".inst 0x00201221\n"); */
            break;
        case Skip:
            printf("Skip instr: %llx\n", current_instr);
            goto skip;
            break;
        default:
            printf("Dry run or Never loacte here\n");
            goto skip;
            break;
        }
        analyzing_result();
skip:0;
        /* printf("\n"); */
        fflush(stdout);
    }
    
    munmap(code_mem, 0x8000);
    munmap(Guard_page1, sizeof(Guard_page1));
    munmap(Guard_page2, sizeof(Guard_page2));
    munmap(dummy1, sizeof(dummy1));
    munmap(instr_pointer, sizeof(instr_pointer));
    printf("Search end\n");
    
    return 0;
}

