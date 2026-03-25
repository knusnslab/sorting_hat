#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <stdint.h>

#define urprint(outfile, rtype, reg_num, before, after) \
    fprintf(outfile, "chagned Register : %s%d\t b:%llx a:%llx\n", rtype, reg_num, before, after);

#define frprint(outfile, instr, rtype, reg_num, before, after) \
    fprintf(outfile, "Forged instr : %x\t %s%d b:%llx a:%llx\n", instr, rtype, reg_num, before, after);

#define frprint2(outfile, instr) \
    fprintf(outfile, "Forged instr : %llx\t detected CONSTRAINED UNPREDICTABLE behaviors\n", instr);

#define ItypePrint(outfile,Type, instr, desc) \
        fprintf(outfile, "%s : %x %s\n", Type, instr, desc);

/* #define INIT_VECTOR_REG()        \ */
/*         asm volatile(           \ */
/*         "movi v0.16b, #32\n"     \ */
/*         "movi v1.16b, #1\n"     \ */
/*         "movi v2.16b, #2\n"     \ */
/*         "movi v3.16b, #3\n"     \ */
/*         "movi v4.16b, #4\n"     \ */
/*         "movi v5.16b, #5\n"     \ */
/*         "movi v6.16b, #6\n"     \ */
/*         "movi v7.16b, #7\n"     \ */
/*         "movi v8.16b, #8\n"     \ */
/*         "movi v9.16b, #9\n"     \ */
/*         "movi v10.16b, #10\n"   \ */
/*         "movi v11.16b, #11\n"   \ */
/*         "movi v12.16b, #12\n"   \ */
/*         "movi v13.16b, #13\n"   \ */
/*         "movi v14.16b, #14\n"   \ */
/*         "movi v15.16b, #15\n"   \ */
/*         "movi v16.16b, #16\n"   \ */
/*         "movi v17.16b, #17\n"   \ */
/*         "movi v18.16b, #18\n"   \ */
/*         "movi v19.16b, #19\n"   \ */
/*         "movi v20.16b, #20\n"   \ */
/*         "movi v21.16b, #21\n"   \ */
/*         "movi v22.16b, #22\n"   \ */
/*         "movi v23.16b, #23\n"   \ */
/*         "movi v24.16b, #24\n"   \ */
/*         "movi v25.16b, #25\n"   \ */
/*         "movi v26.16b, #26\n"   \ */
/*         "movi v27.16b, #27\n"   \ */
/*         "movi v28.16b, #28\n"   \ */
/*         "movi v29.16b, #29\n"   \ */
/*         "movi v30.16b, #30\n"   \ */
/*         "movi v31.16b, #31\n"   \ */
/*         ) */

double v_init[32];
/* #define INIT_FLOATING_REG_TEMP()	\ */
/* 	asm volatile(				\ */
/*     "la t0, %0\n"                 \ */
/* 	"fld f0, 0(t0)\n"				\ */
/* 	"fld f1, 8(t0)\n"				\ */
/* 	"fld f2, 16(t0)\n"				\ */
/* 	"fld f3, 24(t0)\n"				\ */
/* 	"fld f4, 32(t0)\n"				\ */
/* 	"fld f5, 40(t0)\n"				\ */
/* 	"fld f6, 48(t0)\n"				\ */
/* 	"fld f7, 56(t0)\n"				\ */
/* 	"fld f8, 64(t0)\n"				\ */
/* 	"fld f9, 72(t0)\n"				\ */
/* 	"fld f10, 80(t0)\n"				\ */
/* 	"fld f11, 88(t0)\n"				\ */
/* 	"fld f12, 96(t0)\n"				\ */
/* 	"fld f13, 104(t0)\n"				\ */
/* 	"fld f14, 112(t0)\n"				\ */
/* 	"fld f15, 120(t0)\n"				\ */
/* 	::"m"(v_init[0])    \ */
/*     :"t0", "memory"							\ */
/* 	) */
#define INIT_FLOATING_REG1()	\
	asm volatile(				\
	"fld f0, %0\n"				\
	"fld f1, %1\n"				\
	"fld f2, %2\n"				\
	"fld f3, %3\n"				\
	"fld f4, %4\n"				\
	"fld f5, %5\n"				\
	"fld f6, %6\n"				\
	"fld f7, %7\n"				\
	"fld f8, %8\n"				\
	"fld f9, %9\n"				\
	"fld f10, %10\n"				\
	"fld f11, %11\n"				\
	"fld f12, %12\n"				\
	"fld f13, %13\n"				\
	"fld f14, %14\n"				\
	"fld f15, %15\n"				\
	::"m"(v_init[0]), "m"(v_init[1]), "m"(v_init[2]), "m"(v_init[3]), "m"(v_init[4]), "m"(v_init[5]), "m"(v_init[6]), \
		 "m"(v_init[7]), "m"(v_init[8]), "m"(v_init[9]), "m"(v_init[10]), "m"(v_init[11]), "m"(v_init[12]), "m"(v_init[13]), \
		 "m"(v_init[14]), "m"(v_init[15]) \
	:"t4", "memory"							\
	)

#define INIT_FLOATING_REG2()	\
	asm volatile(				\
	"fld f16, %0\n"				\
	"fld f17, %1\n"				\
	"fld f18, %2\n"				\
	"fld f19, %3\n"				\
	"fld f20, %4\n"				\
	"fld f21, %5\n"				\
	"fld f22, %6\n"				\
	"fld f23, %7\n"				\
	"fld f24, %8\n"				\
	"fld f25, %9\n"				\
	"fld f26, %10\n"				\
	"fld f27, %11\n"				\
	"fld f28, %12\n"				\
	"fld f29, %13\n"				\
	"fld f30, %14\n"				\
	"fld f31, %15\n"				\
	::"m"(v_init[16]), "m"(v_init[17]), "m"(v_init[18]), "m"(v_init[19]), "m"(v_init[20]), "m"(v_init[21]), "m"(v_init[22]), \
		 "m"(v_init[23]), "m"(v_init[24]), "m"(v_init[25]), "m"(v_init[26]), "m"(v_init[27]), "m"(v_init[28]), "m"(v_init[29]), \
		 "m"(v_init[30]), "m"(v_init[31]) \
	:	"t4", "memory"						\
	)

/* #define INIT_GPR_TO_ZERO()  \ */
/*         asm volatile(       \ */
/*         "mv a0, x0\n"     \ */
/*         "mv a1, x0\n"     \ */
/*         "mv a2, x0\n"     \ */
/*         "mv a3, x0\n"     \ */
/*         "mv a4, x0\n"     \ */
/*         "mv a5, x0\n"     \ */
/*         "mv a6, x0\n"     \ */
/*         "mv a7, x0\n"     \ */
/*         "mv s1, x0\n"     \ */
/*         "mv s2, x0\n"     \ */
/*         "mv s3, x0\n"    \ */
/*         "mv s4, x0\n"    \ */
/*         "mv s5, x0\n"    \ */
/*         "mv s6, x0\n"    \ */
/*         "mv s7, x0\n"    \ */
/*         "mv s8, x0\n"    \ */
/*         "mv s9, x0\n"    \ */
/*         "mv s10, x0\n"    \ */
/*         "mv s11, x0\n"    \ */
/*         "mv gp, x0\n"    \ */
/*         "mv tp, x0\n"    \ */
/*         "mv t0, x0\n"    \ */
/*         "mv t1, x0\n"    \ */
/*         "mv t2, x0\n"    \ */
/*         "mv t3, x0\n"    \ */
/*         "mv t4, x0\n"    \ */
/*         "mv t5, x0\n"    \ */
/*         "mv t6, x0\n"    \ */
/*         "mv t7, x0\n"    \ */
/*         ); */

	/* "lui   x4, 0x201e0\n"   \ */
	/* "addi  x4, x4, 0x274\n" \ */
	/* "slli  x4, x4, 8\n"     \ */
	/* "ori x4, x4, 0x12\n"    \ */
	/* "slli  x4, x4, 8\n"     \ */
	/* "ori x4, x4, 0x33\n"    \ */
	/* "slli  x4, x4, 8\n"     \ */
	/* "ori x4, x4, 0x1e\n"    \ */
	/* "slli  x4, x4, 8\n"     \ */
	/* "ori x4, x4, 0x26\n"    \ */
#define INIT_GPR_TO_64NUM()  \
        asm volatile(       \
	"lui   x3, 0x3937\n"    \
    "srli x3, x3, 4\n"      \
    "ori x3, x3, 0x0d\n"    \
    "slli x3, x3, 8\n"      \
    "ori x3, x3, 0x2c\n"    \
    "slli  x3, x3, 8\n"     \
    "ori x3, x3, 0x4d\n"    \
	"slli  x3, x3, 8\n"     \
	"ori x3, x3, 0x41\n"    \
	"slli  x3, x3, 8\n"     \
	"ori x3, x3, 0xe1\n"    \
	"slli  x3, x3, 8\n"     \
	"ori x3, x3, 0x0d\n"    \
	"lui   x5, 0x5a00\n"   \
    "srli x5, x5, 4\n"      \
    "ori x5, x5, 0x18\n"    \
    "slli x5, x5, 8\n"      \
    "ori x5, x5, 0x78\n"    \
	"slli  x5, x5, 8\n"     \
	"ori x5, x5, 0x3c\n"    \
	"slli  x5, x5, 8\n"     \
	"ori x5, x5, 0x46\n"    \
	"slli  x5, x5, 8\n"     \
	"ori x5, x5, 0x9c\n"    \
	"slli  x5, x5, 8\n"     \
	"ori x5, x5, 0x72\n"    \
	"lui   x6, 0x3fc8\n"   \
    "srli x6, x6, 4\n"      \
    "ori x6, x6, 0xdd\n"    \
    "slli x6, x6, 8\n"      \
    "ori x6, x6, 0xa1\n"    \
	"slli  x6, x6, 8\n"     \
	"ori x6, x6, 0x1c\n"    \
	"slli  x6, x6, 8\n"     \
	"ori x6, x6, 0x4e\n"    \
	"slli  x6, x6, 8\n"     \
	"ori x6, x6, 0x95\n"    \
	"slli  x6, x6, 8\n"     \
	"ori x6, x6, 0xbf\n"    \
	"lui   x7, 0x6b48\n"   \
    "srli x7, x7, 4\n"      \
    "ori x7, x7, 0xfb\n"    \
    "slli x7, x7, 8\n"      \
    "ori x7, x7, 0x76\n"    \
	"slli  x7, x7, 8\n"     \
	"ori x7, x7, 0x04\n"    \
	"slli  x7, x7, 8\n"     \
	"ori x7, x7, 0x75\n"    \
	"slli  x7, x7, 8\n"     \
	"ori x7, x7, 0x37\n"    \
	"slli  x7, x7, 8\n"     \
	"ori x7, x7, 0x02\n"    \
	"lui   x8, 0x2b6e2\n"   \
	"addi  x8, x8, 0x4f0\n" \
	"slli  x8, x8, 8\n"     \
	"ori x8, x8, 0x4c\n"    \
	"slli  x8, x8, 8\n"     \
	"ori x8, x8, 0x3b\n"    \
	"slli  x8, x8, 8\n"     \
	"ori x8, x8, 0x1e\n"    \
	"slli  x8, x8, 8\n"     \
	"ori x8, x8, 0xd7\n"    \
	"lui   x9, 0x3d65\n"   \
    "srli x9, x9, 4\n"      \
    "ori x9, x9, 0x9a\n"    \
    "slli x9, x9, 8\n"      \
    "ori x9, x9, 0xb3\n"    \
	"slli  x9, x9, 8\n"     \
	"ori x9, x9, 0x59\n"    \
	"slli  x9, x9, 8\n"     \
	"ori x9, x9, 0x8f\n"    \
	"slli  x9, x9, 8\n"     \
	"ori x9, x9, 0x7d\n"    \
	"slli  x9, x9, 8\n"     \
	"ori x9, x9, 0x43\n"    \
	"lui   x10, 0x5b68a\n"  \
	"addi  x10, x10, 0x5fd\n" \
	"slli  x10, x10, 8\n"   \
	"ori x10, x10, 0x33\n"  \
	"slli  x10, x10, 8\n"   \
	"ori x10, x10, 0x69\n"  \
	"slli  x10, x10, 8\n"   \
	"ori x10, x10, 0xb3\n"  \
	"slli  x10, x10, 8\n"   \
	"ori x10, x10, 0xee\n"  \
	"lui   x11, 0x60a3a\n"  \
	"addi  x11, x11, 0x6f9\n" \
	"slli  x11, x11, 8\n"   \
	"ori x11, x11, 0x18\n"  \
	"slli  x11, x11, 8\n"   \
	"ori x11, x11, 0x27\n"  \
	"slli  x11, x11, 8\n"   \
	"ori x11, x11, 0x57\n"  \
	"slli  x11, x11, 8\n"   \
	"ori x11, x11, 0x01\n"  \
	"lui   x12, 0x3ec0\n"  \
    "srli x12, x12, 4\n"    \
    "ori x12, x12, 0xcd\n"  \
    "slli x12, x12, 8\n"    \
    "ori x12, x12, 0x63\n"  \
	"slli  x12, x12, 8\n"   \
	"ori x12, x12, 0x63\n"  \
	"slli  x12, x12, 8\n"   \
	"ori x12, x12, 0xec\n"  \
	"slli  x12, x12, 8\n"   \
	"ori x12, x12, 0x3e\n"  \
	"slli  x12, x12, 8\n"   \
	"ori x12, x12, 0xc5\n"  \
	"lui   x13, 0x2b0c\n"  \
    "srli x13, x13, 4\n"    \
    "ori x13, x13, 0xe9\n"  \
    "slli x13, x13, 8\n"    \
    "ori x13, x13, 0x1c\n"  \
	"slli  x13, x13, 8\n"   \
	"ori x13, x13, 0x5c\n"  \
	"slli  x13, x13, 8\n"   \
	"ori x13, x13, 0x9c\n"  \
	"slli  x13, x13, 8\n"   \
	"ori x13, x13, 0x43\n"  \
	"slli  x13, x13, 8\n"   \
	"ori x13, x13, 0x59\n"  \
	"lui   x14, 0x17097\n"  \
	"addi  x14, x14, 0x5e8\n" \
	"slli  x14, x14, 8\n"   \
	"ori x14, x14, 0x6e\n"  \
	"slli  x14, x14, 8\n"   \
	"ori x14, x14, 0x1b\n"  \
	"slli  x14, x14, 8\n"   \
	"ori x14, x14, 0xde\n"  \
	"slli  x14, x14, 8\n"   \
	"ori x14, x14, 0x29\n"  \
    "lui   x16, 0x30ad1\n"  \
	"addi  x16, x16, 0x70e\n" \
	"slli  x16, x16, 8\n"   \
	"ori x16, x16, 0x33\n"  \
	"slli  x16, x16, 8\n"   \
	"ori x16, x16, 0xc4\n"  \
	"slli  x16, x16, 8\n"   \
	"ori x16, x16, 0xb1\n"  \
	"slli  x16, x16, 8\n"   \
	"ori x16, x16, 0x1a\n"  \
	"lui   x17, 0x3646\n"  \
    "srli x17, x17, 4\n"    \
    "ori x17, x17, 0xfa\n"  \
    "slli x17, x17, 8\n"    \
    "ori x17, x17, 0xdc\n"  \
	"slli  x17, x17, 8\n"   \
	"ori x17, x17, 0x6d\n"  \
	"slli  x17, x17, 8\n"   \
	"ori x17, x17, 0xff\n"  \
	"slli  x17, x17, 8\n"   \
	"ori x17, x17, 0x9d\n"  \
	"slli  x17, x17, 8\n"   \
	"ori x17, x17, 0x5b\n"  \
	"lui   x18, 0x28b3\n"  \
    "srli x18, x18, 4\n" \
    "ori  x18, x18, 0xb8\n"    \
    "slli x18, x18, 8\n" \
    "ori x18, x18, 0x21\n"     \
	"slli  x18, x18, 8\n"   \
	"ori x18, x18, 0x2e\n"  \
	"slli  x18, x18, 8\n"   \
	"ori x18, x18, 0xfd\n"  \
	"slli  x18, x18, 8\n"   \
	"ori x18, x18, 0x93\n"  \
	"slli  x18, x18, 8\n"   \
	"ori x18, x18, 0x68\n"  \
	"lui   x19, 0x09d86\n"  \
	"addi  x19, x19, 0x74b\n" \
	"slli  x19, x19, 8\n"   \
	"ori x19, x19, 0x5e\n"  \
	"slli  x19, x19, 8\n"   \
	"ori x19, x19, 0x65\n"  \
	"slli  x19, x19, 8\n"   \
	"ori x19, x19, 0x71\n"  \
	"slli  x19, x19, 8\n"   \
	"ori x19, x19, 0xfa\n"  \
	"lui   x20, 0x561dc\n"  \
	"addi  x20, x20, 0x2d9\n" \
	"slli  x20, x20, 8\n"    \
	"ori x20, x20, 0x3b\n"  \
	"slli  x20, x20, 8\n"   \
	"ori x20, x20, 0xe3\n"  \
	"slli  x20, x20, 8\n"   \
	"ori x20, x20, 0x60\n"  \
	"slli  x20, x20, 8\n"   \
	"ori x20, x20, 0xbb\n"  \
	"lui   x21, 0x4ce37\n"  \
	"addi  x21, x21, 0x20d\n" \
	"slli  x21, x21, 8\n"   \
	"ori x21, x21, 0x68\n"  \
	"slli  x21, x21, 8\n"   \
	"ori x21, x21, 0x54\n"  \
	"slli  x21, x21, 8\n"   \
	"ori x21, x21, 0xda\n"  \
	"slli  x21, x21, 8\n"   \
	"ori x21, x21, 0xeb\n"  \
	"lui   x22, 0x1af0\n"  \
    "srli x22, x22, 4\n"    \
    "ori x22, x22, 0x6e\n"  \
    "slli x22, x22, 8\n"    \
    "ori x22, x22, 0x29\n"  \
	"slli  x22, x22, 8\n"   \
	"ori x22, x22, 0x1e\n"  \
	"slli  x22, x22, 8\n"   \
	"ori x22, x22, 0xd0\n"  \
	"slli  x22, x22, 8\n"   \
	"ori x22, x22, 0x53\n"  \
	"slli  x22, x22, 8\n"   \
	"ori x22, x22, 0x91\n"  \
	"lui   x23, 0x7f1e2\n"  \
	"addi  x23, x23, 0x2bd\n" \
	"slli  x23, x23, 8\n"   \
	"ori x23, x23, 0x13\n"  \
	"slli  x23, x23, 8\n"   \
	"ori x23, x23, 0x7a\n"  \
	"slli  x23, x23, 8\n"   \
	"ori x23, x23, 0xe7\n"  \
	"slli  x23, x23, 8\n"   \
	"ori x23, x23, 0x7f\n"  \
	"lui   x24, 0x69f4\n"  \
    "srli x24, x24, 4\n"    \
    "ori x24, x24, 0x0b\n"  \
    "slli x24, x24, 8\n"    \
    "ori x24, x24, 0x2f\n"  \
	"slli  x24, x24, 8\n"   \
	"ori x24, x24, 0x15\n"  \
	"slli  x24, x24, 8\n"   \
	"ori x24, x24, 0x0a\n"  \
	"slli  x24, x24, 8\n"   \
	"ori x24, x24, 0x71\n"  \
    "slli  x24, x24, 8\n"   \
	"ori x24, x24, 0x02\n"  \
	"lui   x25, 0x60810\n"  \
	"addi  x25, x25, 0x371\n" \
	"slli  x25, x25, 8\n"   \
	"ori x25, x25, 0x36\n"  \
	"slli  x25, x25, 8\n"   \
	"ori x25, x25, 0x09\n"  \
	"slli  x25, x25, 8\n"   \
	"ori x25, x25, 0x21\n"  \
	"slli  x25, x25, 8\n"   \
	"ori x25, x25, 0x37\n"  \
	"lui   x26, 0x1163\n"  \
    "srli x26, x26, 4\n"    \
    "ori x26, x26, 0x7b\n"  \
    "slli x26, x26, 8\n"    \
    "ori x26, x26, 0xf1\n"  \
	"slli  x26, x26, 8\n"   \
	"ori x26, x26, 0x1a\n"  \
	"slli  x26, x26, 8\n"   \
	"ori x26, x26, 0x5e\n"  \
	"slli  x26, x26, 8\n"   \
	"ori x26, x26, 0x14\n"  \
	"slli  x26, x26, 8\n"   \
	"ori x26, x26, 0x23\n"  \
	"lui   x27, 0x167bd\n"  \
	"addi  x27, x27, 0x1b4\n" \
	"slli  x27, x27, 8\n"   \
	"ori x27, x27, 0x1b\n"  \
	"slli  x27, x27, 8\n"   \
	"ori x27, x27, 0x04\n"  \
	"slli  x27, x27, 8\n"   \
	"ori x27, x27, 0x8b\n"  \
	"slli  x27, x27, 8\n"   \
	"ori x27, x27, 0xf5\n"  \
	"lui   x28, 0x4780\n"  \
    "srli x28, x28, 4\n"    \
    "ori x28, x28, 0x4e\n"  \
    "slli x28, x28, 8\n"    \
    "ori x28, x28, 0x07\n"  \
	"slli  x28, x28, 8\n"   \
	"ori x28, x28, 0x38\n"  \
	"slli  x28, x28, 8\n"   \
	"ori x28, x28, 0x82\n"  \
	"slli  x28, x28, 8\n"   \
	"ori x28, x28, 0xd2\n"  \
	"slli  x28, x28, 8\n"   \
	"ori x28, x28, 0x3e\n"  \
	"lui   x29, 0x14b8\n"  \
    "srli x29, x29, 4\n"    \
    "ori x29, x29, 0xbf\n"  \
    "slli x29, x29, 8\n"    \
    "ori x29, x29, 0xc7\n"    \
	"slli  x29, x29, 8\n"   \
	"ori x29, x29, 0x6d\n"  \
	"slli  x29, x29, 8\n"   \
	"ori x29, x29, 0x36\n"  \
	"slli  x29, x29, 8\n"   \
	"ori x29, x29, 0xac\n"  \
	"slli  x29, x29, 8\n"   \
	"ori x29, x29, 0x72\n"  \
	"lui   x30, 0x20736\n"  \
	"addi  x30, x30, 0x6bb\n" \
	"slli  x30, x30, 8\n"   \
	"ori x30, x30, 0x78\n"  \
	"slli  x30, x30, 8\n"   \
	"ori x30, x30, 0x5d\n"  \
	"slli  x30, x30, 8\n"   \
	"ori x30, x30, 0x8f\n"  \
	"slli  x30, x30, 8\n"   \
	"ori x30, x30, 0xa2\n"  \
	"lui   x31, 0x4680d\n"  \
	"addi  x31, x31, 0x0c3\n" \
	"slli  x31, x31, 8\n"   \
	"ori x31, x31, 0x33\n"  \
	"slli  x31, x31, 8\n"   \
	"ori x31, x31, 0x09\n"  \
	"slli  x31, x31, 8\n"   \
	"ori x31, x31, 0xd6\n"  \
	"slli  x31, x31, 8\n"   \
	"ori x31, x31, 0x5f\n"  \
 	::: \
    "a0","a1","a2","a3","a4","a5","a6","a7",	\
      "s0", "s1","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11", \
      "gp","tp", \
      "t0","t1","t2","t3","t4","t5","t6"	\
       );

        /* "mv x3, %0\n"      \ */
        /* "mv x4, %0\n"      \ */
#define INIT_GPR_TO_MEM()   \
        asm volatile(       \
        "mv x5, %0\n"      \
        "mv x6, %0\n"      \
        "mv x7, %0\n"      \
        "mv x9, %0\n"      \
        "mv x10, %0\n"     \
        "mv x11, %0\n"     \
        "mv x12, %0\n"     \
        "mv x13, %0\n"     \
        "mv x14, %0\n"     \
        "mv x16, %0\n"     \
        "mv x17, %0\n"     \
        "mv x18, %0\n"     \
        "mv x19, %0\n"     \
        "mv x20, %0\n"     \
        "mv x21, %0\n"     \
        "mv x22, %0\n"     \
        "mv x23, %0\n"     \
        "mv x24, %0\n"     \
        "mv x25, %0\n"     \
        "mv x26, %0\n"     \
        "mv x27, %0\n"     \
        "mv x28, %0\n"     \
        "mv x29, %0\n"     \
        "mv x30, %0\n"     \
        "mv x31, %0\n"     \
        :                   \
        :"r"(test_pointer)\
        : "x3","x4","x5","x6","x7","x9","x10","x11","x12","x13","x14","x16","x17","x18","x19","x20","x21","x22", \
        "x23","x24","x25","x26","x27","x28","x29","x30","x31"\
        );

// N == V && Z == 0
// cond1 --> cond2 (changed)
/* #define SET_CPSR_COND2()  \ */
/*     asm volatile(       \ */
/*     "mov x8, #0xB0000000\n"      \ */
/*     "msr nzcv, x8\n"    \ */
/*     :::"x8"             \ */
/*     );                  \ */
/*  */
/* // N != V && Z == 1 && C == 1 */
/* // cond2 --> cond1 (changed) */
/* #define SET_CPSR_COND1()  \ */
/*     asm volatile(       \ */
/*     "mov x8, #0xE0000000\n"      \ */
/*     "msr nzcv, x8\n"    \ */
/*     :::"x8"           \ */
/*     );  */
/*  */
/* #define SET_CPSR_COND3()  \ */
/*     asm volatile(       \ */
/*     "mov x8, #0x0\n"      \ */
/*     "msr nzcv, x8\n"    \ */
/*     :::"x8"             \ */
/*     );  */
/*  */
/* #define SET_CPSR_COND4()  \ */
/*     asm volatile(       \ */
/*     "mov x8, #0xF0000000\n"      \ */
/*     "msr nzcv, x8\n"    \ */
/*     :::"x8"\ */
/* );  */

#define BRANCH_INSTR()  \
            asm volatile(   \
            "blr %[ptr]"    \
            ::[ptr]"r"(instr_pointer)   \
            );  

#define MAX_LENGTH 0xFFFFFFFF
#define PAGE_SIZE  0x04000000

#define REG_FP 8


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
struct sigaction s;
jmp_buf st1, st2, st3, st4, st5;
uint8_t *Guard_page1, *Guard_page2;
uint8_t *instr_pointer;
uint32_t current_instr;
uint32_t swap_instr;
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
int check_reg;

//cycle check var
uint64_t nop_cycle;
uint64_t instr_cycle;

//Function pre define
void analyzing_result();
void result_log(uint64_t, int);
void sig_handler(int, siginfo_t*, void* );
void configure_handler(void (*)(int, siginfo_t*, void*));
void init_before_register();
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
/* void stage5(); */
void busy_sleep_ms(long);
static inline unsigned long get_cycles(void);
void check_tp(void);
void adjust_check_tp(int);

FILE* outfile;
FILE* undoc_out;

volatile uint64_t fp_ra_sp[3] = {0};
volatile uint64_t gpr_before[32];
volatile uint64_t gpr_after[32];
volatile uint64_t vec_before[32][2];
volatile uint64_t vec_after[32][2];

extern char resume1, resume2, resume3, resume4, _instr_start, _instr_end;
/* asm( */
/*         ".global _instr_start     \n" */
/*         "_instr_start:            \n" */
/*         "lui fp, 0xdead\n" */
/*         "srli fp, fp, 8\n" */
/*         "ori fp, fp, 0xbe\n" */
/*         "slli fp, fp, 8\n" */
/*         "ori fp, fp, 0xaf\n" */
/*         "lui   x15, 0x6796\n"   */
/*         "srli x15, x15, 8\n"     */
/*         "ori x15, x15, 0x4c\n"   */
/*         "slli x15, x15, 8\n"     */
/*         "ori x15, x15, 0x91\n"   */
/*         "slli  x15, x15, 8\n"    */
/*         "ori x15, x15, 0x3c\n"   */
/*         "slli  x15, x15, 8\n"    */
/*         "ori x15, x15, 0x7c\n"   */
/*         "slli  x15, x15, 8\n"    */
/*         "ori x15, x15, 0xf8\n"   */
/*         "slli  x15, x15, 8\n"    */
/*         "ori x15, x15, 0xb9\n"   */
/*         ".global _instr_end      \n" */
/*         "_instr_end:             \n" */
/* ); */

void adjust_check_tp(int i){
    uint8_t mask;
    mask = (adj_instr[i] >> 7) & 0x1F;
    if(mask == 0x4)
        adj_instr[i] |= 0x300;

}

void check_tp(){
    uint8_t mask;
    mask = (current_instr >> 7) & 0x1F;
    if(mask == 0x4)
        current_instr |= 0x300;

}

inline unsigned long get_cycles() {
    unsigned long cycles;
    asm volatile (
        "rdcycle %0"
        : "=r"(cycles) // 출력: cycles 변수에 결과 저장
    );
    return cycles;
}

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
    
    *code++ = 0x0000100F; // ISB
    *code++ = 0x00000013; // NOP
    /* *code++ = 0xd284e209; // MOV x9, #10000 */
    // loop_start:
    for(int i = 0; i < 80; i++){
        *code++ = inst;
    }
    /* *code++ = 0xf1000529; // SUBS x9, x9, #1 */
    /* *code++ = 0x54fc17e1; // B.NE loop start */
    *code++ = 0x0000100F; // ISB
    *code++ = 0x00008067; // RET


    /* __builtin___clear_cache((char*)code_mem, (char*)code_mem + sizeof(code_mem)); */
    func_t func = (func_t)code_mem;

    start = get_cycles();
    for(int i=0; i<100; i++){
        func();
    }
    end = get_cycles();
    
    result = end - start;
    return result;

}


void init_vector_array(){
    double value = 1.1f;
    for(int i=0; i<32; i++){
		v_init[i] = value;
		value += 0.1f;
    }
}

void result_log(uint64_t instr, int reg_num){
    if(triage_Method == Normal)
        fprintf(undoc_out, "instr : %llx reg x%d is changed!\n", instr, reg_num);
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

            if((instrBitsFlag & ADJ_flag) == ADJ_flag){
                fprintf(undoc_out, "adjust stage");
            }

            fprintf(undoc_out, "Changed Register: ");
            fprintf(undoc_out, " %d", check_reg);
            fprintf(undoc_out, "\n");
            fprintf(undoc_out, "Changed Flag :");
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
        gpr_before[3] = 0x39370d2c4d41e10d;
        gpr_before[4]= 0x201e027412331e26;
        gpr_before[5]= 0x5a0018783c469c72;
        gpr_before[6]= 0x3fc8dda11c4e95bf;
        gpr_before[7]= 0x6b48fb7604753702;
        /* gpr_before[8]= 0x2b6e24f04c3b1ed7; */
        gpr_before[8] = 0xdeadbeaf;
        gpr_before[9]= 0x3d659ab3598f7d43;
        gpr_before[10]= 0x5b68a5fd3369b3ee;
        gpr_before[11]= 0x60a3a6f918275701;
        gpr_before[12]= 0x3ec0cd6363ec3ec5;
        gpr_before[13]= 0x2b0ce91c5c9c4359;
        gpr_before[14]= 0x170975e86e1bde29;
        gpr_before[15]= 0x67964c913c7cf8b9;
        gpr_before[16]= 0x30ad170e33c4b11a;
        gpr_before[17]= 0x3646fadc6dff9d5b;
        gpr_before[18]= 0x28b3b8212efd9368;
        gpr_before[19]= 0x9d8674b5e6571fa;
        gpr_before[20]= 0x561dc2d93be360bb;
        gpr_before[21]= 0x4ce3720d6854daeb;
        gpr_before[22]= 0x1af06e291ed05391;
        gpr_before[23]= 0x7f1e22bd137ae77f;
        gpr_before[24]= 0x69f40b2f150a7102;
        gpr_before[25]= 0x6081037136092137;
        gpr_before[26]= 0x11637bf11a5e1423;
        gpr_before[27]= 0x167bd1b41b048bf5;
        gpr_before[28]= 0x47804e073882d23e;
        gpr_before[29]= 0x14b8bfc76d36ac72;
        gpr_before[30]= 0x207366bb785d8fa2;
        gpr_before[31]= 0x4680d0c33309d65f;

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
        gpr_before[i] = (uint64_t)test_pointer;
}

void sig_handler(int signo, siginfo_t* si, void *p){
    
    uint32_t pre_stage = instrBitsFlag & 0x1F00;
    ucontext_t *uc = (ucontext_t*)p;
    //uint32_t cpsr_temp = uc->uc_mcontext->__ss.__cpsr; // RISC-V에는 별도의 flag register가 존재하지 않음.
    uint64_t *gpr_ptr = (uint64_t *)&uc->uc_mcontext.__gregs;
    //fp_ra_sp[1] = fp_ra_sp[1] & 0x0000FFFFFFFFFFFF;
    uintptr_t pc_addr = uc->uc_mcontext.__gregs[REG_PC];
    

	if(signo == SIGALRM){
        alarm(0);
        instrBitsFlag &= 0x0;
        instrBitsFlag |= ALARM_flag;
        goto end;
    }
    
    /* usleep(1*1000); */
    
check_prework: 
    if(triage_Method == Normal){
        switch(instrBitsFlag & 0x1F00) {
            case STAGE1_flag:
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
                    goto Save_normal;
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
                //conditional branch가 여기서 작동할 수도 있다
                //STAGE4의 flag init은 0x0 이기 때문이다.
                if(signo == SIGSEGV | signo == SIGBUS){
                    instrBitsFlag &= ~STAGE4_flag;
                    instrBitsFlag |= STAGE3_flag;
                    goto end;
                }
                else{
                    instrBitsFlag &= ~STAGE4_flag;
                    if((instrBitsFlag & ALU_flag) == ALU_flag);
                    else
                        instrBitsFlag |= NOP_flag;
                    goto end;
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
        double *v_reg_hex = (double *)&uc->uc_mcontext.__fpregs.__d.__f;
        double *v_init_hex = (double *)v_init;
            
        if(v_reg_hex[i] != v_init_hex[i]){
            instrBitsFlag &= ~NOP_flag;
            instrBitsFlag |= ALU_flag;
            instrBitsFlag |= VPR_flag;
        }
    }
    for(int i=3; i<32; i++){
        gpr_after[i] = *(gpr_ptr + i);
        if((i!=4) && (gpr_before[i] != (gpr_after[i]))){
            check_reg = i;
            instrBitsFlag |= ALU_flag;
            break;
        }
    }
    /* after_cpsr = uc->uc_mcontext->__ss.__cpsr; */
    /* uc->uc_mcontext->__ss.__x[29] = gpr_before[29]; // 무슨 이유인지 모르겠는데 fp가 망가진다 recovery 해줘야함 */
    goto end;
    
end:
    /* sleep(1); */
    /* uc->uc_mcontext->__ss.__lr = gpr_before[30]; */
    uc->uc_mcontext.__gregs[REG_SP] = fp_ra_sp[2];
    uc->uc_mcontext.__gregs[REG_RA] = fp_ra_sp[1];
    uc->uc_mcontext.__gregs[REG_FP] = fp_ra_sp[0];

    asm volatile("fence.i \n");
#ifdef RESUME
    if(pre_stage == STAGE1_flag)
        uc->uc_mcontext.__gregs[REG_PC] = (uintptr_t)&resume1;
    else if(pre_stage == STAGE2_flag)
        uc->uc_mcontext.__gregs[REG_PC] = (uintptr_t)&resume2;
    else if(pre_stage == STAGE3_flag)
        uc->uc_mcontext.__gregs[REG_PC] = (uintptr_t)&resume3;
    else if(pre_stage == STAGE4_flag)
        uc->uc_mcontext.__gregs[REG_PC] = (uintptr_t)&resume4;
#endif

#ifndef RESUME
    if(pre_stage == STAGE1_flag) siglongjmp(st1, 1);
    else if(pre_stage == STAGE2_flag) siglongjmp(st2, 1);
    else if(pre_stage == STAGE3_flag) siglongjmp(st3, 1);
    else if(pre_stage == STAGE4_flag) siglongjmp(st4, 1);
    else if(pre_stage == STAGE5_flag) siglongjmp(st5, 1);
    else siglongjmp(st1, 1); // for dry run only
#endif

}

void configure_handler(void (*handler)(int, siginfo_t*, void*)){
    
    /* struct sigaction s; */
   
    s.sa_sigaction = sig_handler;   
    /* s.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_NODEFER; */
    s.sa_flags = SA_SIGINFO | SA_ONSTACK;
    /* s.sa_flags = s.sa_flags & ~SA_RESETHAND; */
    /* s.sa_flags = SA_SIGINFO; */
    sigemptyset(&s.sa_mask);

    printf("sa_flag : %x\n",s.sa_flags );
    sigaction(SIGILL, &s, NULL);
    sigaction(SIGSEGV, &s, NULL);
    sigaction(SIGTRAP, &s, NULL);
    sigaction(SIGBUS, &s, NULL);
    sigaction(SIGALRM, &s, NULL);

}

void adjust(){
 	
    uint16_t mask = 0xd5000000;
    int adj_value[3] = {0};
    int max =0, idx= -1;
	int LOW_BIT = 7;
    int HIGH_BIT = 11;
    int BIT_COUNT = HIGH_BIT - LOW_BIT + 1; // 5
    uint32_t MASK = ((1U << BIT_COUNT) - 1) << LOW_BIT;
    uint32_t INCREMENT = 1U << LOW_BIT;
    /* if((current_instr & mask) == 0){ // 명령어의 끝이 0000이라 값을 뺄때 opcode가 달리는것을 방지 */
    /* if(current_instr == 0xd503201f) */
    if(current_instr & mask)
        goto adj_skip;
	
    adj_instr[0] = (current_instr & ~MASK) | (((current_instr & MASK) + ((1U << LOW_BIT)) & MASK));
    adj_instr[1] = (current_instr & ~MASK) | (((current_instr & MASK) + ((2U << LOW_BIT)) & MASK));
    adj_instr[2] = (current_instr & ~MASK) | (((current_instr & MASK) + ((3U << LOW_BIT)) & MASK));
    adj_instr[3] = (current_instr & ~MASK) | (((current_instr & MASK) + ((4U << LOW_BIT)) & MASK));
    adj_instr[4] = (current_instr & ~MASK) | (((current_instr & MASK) + ((5U << LOW_BIT)) & MASK));
    adj_instr[5] = (current_instr & ~MASK) | (((current_instr & MASK) + ((6U << LOW_BIT)) & MASK));
    
    for(int i=0; i<6; i++){
        instrBitsFlag &= 0x0;   //init instrBitsFlag
        adjust_check_tp(i);
        memcpy(instr_pointer+init_length, &adj_instr[i], 4);
        
        instrBitsFlag |= STAGE1_flag;
        while((instrBitsFlag & 0x1F00) != 0x0){
            alarm(3);
            dummy_ptr_fp = dummy1;
            dummy_ptr_sp = dummy1+0x10;
            gpr_before[8] = *dummy_ptr_fp; //fp register
            gpr_before[1] = 0xdeadbeaf; //lr register
            gpr_before[2] = *dummy_ptr_sp; //sp register

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
            /* else if((instrBitsFlag & STAGE5_flag) == STAGE5_flag){ */
            /*     init_before_register(); */
            /*     stage5(); */
            /* } */
            alarm(0);
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
/* __attribute__((optimize("-fomit-frame-pointer"))) */
void stage1(){
    
    if(sigsetjmp(st1, 1) == 0){
        //store original ra, fp
        asm volatile(
        "sd x8, %0\n"
        "sd x1, %1\n"
        :"=m"(fp_ra_sp[0]), "=m"(fp_ra_sp[1])
        :: "x8", "x1"
        );
        
        /* asm volatile( */
        /* "mv %0, x8\n" */
        /* "mv %1, x1\n" */
        /* ::"r"(fp_ra_sp[0]), "r"(fp_ra_sp[1]) */
        /* : "x8", "x1" */
        /* ); */

        INIT_FLOATING_REG1();
        INIT_FLOATING_REG2();
        //store orignal sp
        asm volatile(
        "mv %[current], sp\n"
        :[current]"=r"(fp_ra_sp[2])
        :: "t4"
        );
        
        //load dummy sp, fp
        asm volatile(
        "mv a5, %[instr]\n"
        "mv sp, %[sp]\n"
        "mv x8, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp), [instr]"r"(instr_pointer)
        :"sp","x8", "a5"
        );
        
        INIT_GPR_TO_64NUM();
		
		asm volatile(
		/* "mv a5, sp\n" */
		"jr a5\n"
		);    	
    }
    else {
#ifdef RESUME
        asm volatile(
            ".global resume1\n"
            "resume1:\n"
        );
#endif
        asm volatile("fence.i\n");
    }
}

/* __attribute__((optimize("-fomit-frame-pointer"))) */
void stage2(){
    
    if(sigsetjmp(st2, 1) == 0){
        
        asm volatile(
        "sd x8, %0\n"
        "sd x1, %1\n"
        :"=m"(fp_ra_sp[0]), "=m"(fp_ra_sp[1])
        :: "x8", "x1"
        );

        INIT_FLOATING_REG1();
        INIT_FLOATING_REG2();
        asm volatile(
        "mv %[current], sp\n"
        :[current]"=r"(fp_ra_sp[2])
        ::
        );
        
        asm volatile(
        "mv sp, %[sp]\n"
        "mv x8, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp)
        :"sp","x8"
        );
        /* asm volatile( */
        /* "ld sp, 0(%[sp])\n" */
        /* "ld x8, 0(%[fp])\n" */
        /* ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp) */
        /* :"sp","x8" */
        /* ); */
        
        INIT_GPR_TO_MEM();
        asm volatile("mv a5, %[instr]\n"::[instr]"r"(instr_pointer): "a5");
     
		asm volatile(
		"jr a5\n"
		);
        
    }
    else {
#ifdef RESUME
        asm volatile(
            ".global resume2\n"
            "resume2:\n"
        );
#endif
        asm volatile("fence.i\n");
    }

}

/* __attribute__((optimize("-fomit-frame-pointer"))) */
void stage3(){


    if(sigsetjmp(st3, 1) == 0){
        
        asm volatile(
        "sd x8, %0\n"
        "sd x1, %1\n"
        :"=m"(fp_ra_sp[0]), "=m"(fp_ra_sp[1])
        :: "x8", "x1"
        );

        INIT_FLOATING_REG1();
        INIT_FLOATING_REG2();
        asm volatile(
        "mv %[current], sp\n"
        :[current]"=r"(fp_ra_sp[2])
        ::
        );
        
        asm volatile(
        "mv sp, %[sp]\n"
        "mv x8, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp)
        :"sp","x8"
        );
        
        INIT_GPR_TO_MEM();
        asm volatile("mv a5, %[instr]\n"::[instr]"r"(instr_pointer): "a5");
     
		asm volatile(
		/* "mv a5, sp\n" */
		"jr a5\n"
		);

        /* asm volatile( */
        /* "jr %[ptr]\n" */
        /* ::[ptr]"r"(instr_pointer) */
        /* ); */
    }
    else {
#ifdef RESUME
        asm volatile(
            ".global resume3\n"
            "resume3:\n"
        );
#endif
        asm volatile("fence.i\n");
    }
}

/* __attribute__((optimize("-fomit-frame-pointer"))) */
void stage4(){

    if(sigsetjmp(st4, 1) == 0){
        
        asm volatile(
        "sd x8, %0\n"
        "sd x1, %1\n"
        :"=m"(fp_ra_sp[0]), "=m"(fp_ra_sp[1])
        :: "x8", "x1"
        );

        INIT_FLOATING_REG1();
        INIT_FLOATING_REG2();
        
        asm volatile(
        "mv %[current], sp\n"
        :[current]"=r"(fp_ra_sp[2])
        ::
        );
        
        asm volatile(
        "mv a5, %[instr]\n"
        "mv sp, %[sp]\n"
        "mv x8, %[fp]\n"
        ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp), [instr]"r"(instr_pointer)
        :"sp","x8", "a5"
        );

        INIT_GPR_TO_64NUM();
     
		asm volatile(
		"jr a5\n"
		);
    }
    else {
#ifdef RESUME
        asm volatile(
            ".global resume4\n"
            "resume4:\n"
        );
#endif
        asm volatile("fence.i\n");
    }
}

/* __attribute__((optimize("-fomit-frame-pointer"))) */
/* void stage5(){ */
/*  */
/*     if(sigsetjmp(st5) == 0){ */
/*          */
/*         asm volatile( */
/*         "sd x8, 0(%0)\n" */
/*         "sd x2, 0(%0)\n" */
/*         :"=r"(fp_ra_sp[0]), "=r"(fp_ra_sp[1]) */
/*         :: "x8", "x2" */
/*         ); */
/*  */
/*         INIT_FLOATING_REG1(); */
/*         INIT_FLOATING_REG2(); */
/*  */
/*         asm volatile( */
/*         "mv %[current], sp\n" */
/*         :[current]"=r"(fp_ra_sp[2]) */
/*         :: */
/*         ); */
/*          */
/*         asm volatile( */
/*         "ld sp, 0(%[sp])\n" */
/*         "ld x8, 0(%[fp])\n" */
/*         ::[fp]"r"(dummy_ptr_fp) , [sp]"r"(dummy_ptr_sp) */
/*         :"sp","x8" */
/*         ); */
/*  */
/*         INIT_GPR_TO_64NUM(); */
/*       */
/* 		asm volatile( */
/* 		"mv a5, sp\n" */
/* 		"jr a5\n" */
/* 		); */
/* 	 */
/*         #<{(| asm volatile( |)}># */
/*         #<{(| "jr %[ptr]\n" |)}># */
/*         #<{(| ::[ptr]"r"(instr_pointer) |)}># */
/*         #<{(| ); |)}># */
/*  */
/*     } */
/*     else { */
/*         asm volatile("fence.i\n"); */
/*     } */
/* }    */

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
    /* uint32_t set_trap = 0x00100073; // EBREAK - RISC-V */
    uint32_t set_trap = 0x00000000; // EBREAK - RISC-V
    int total = 0;
    int progress = 0;
    char *fileName;
    char *outFile;
    stack_t ss;

    if(argc != 4) {
        printf("Usage: %s <file name> <file line> <file name>\n", argv[0]);
        exit(1);
    }

    total = atoi(argv[2]);
    fileName = argv[1];
    outFile = argv[3];
    
    Guard_page1 = mmap(NULL, PAGE_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1,0);
    instr_pointer = mmap(Guard_page1+PAGE_SIZE, PAGE_SIZE, PROT_READ|PROT_EXEC|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    Guard_page2 = mmap(instr_pointer+PAGE_SIZE, PAGE_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
    test_pointer = instr_pointer+0x80; // unaliged memory를 넣어 SIGBUS를 발생시키는 것이 목적

    printf("GP1 : %p\nIP : %p\nGP2 : %p\n", Guard_page1, instr_pointer, Guard_page2);
    
    init_length = (void *)&_instr_end - (void *)&_instr_start;
    printf("instr_pointer : %p\n", instr_pointer);
    /* instr_pointer = instr_pointer+ 0x3ff4; */
    instr_pointer = instr_pointer+ PAGE_SIZE - init_length - 8;
    printf("instr_pointer : %p\n", instr_pointer);
    assert(((uintptr_t)instr_pointer % 4) == 0);
    
    
    if((instr_pointer-0x3ffc) == MAP_FAILED){
        perror("mmap");
        exit(-1);
    }
    
    undoc_out = fopen(outFile, "w");
    input_file = fopen(fileName, "r");
    if(input_file == NULL) {
        perror("failed to open threadH");
        return 1;
    }

    memcpy(instr_pointer, &_instr_start, init_length);
    memcpy(instr_pointer+init_length+4, &set_trap, 4);
    
    memset(dummy_stack, 0, sizeof(dummy_stack));

    /* dummy1 = mmap(NULL, 0x4000, PROT_NONE, MAP_ANON | MAP_PRIVATE,-1,0) + 0x1000; */
    dummy1 = mmap(NULL, 0x4000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE,-1,0) + 0x1000;
    dummy1 = (unsigned char *)(((uintptr_t)dummy1 +15) & ~15) + 1024;

    if((ss.ss_sp = malloc(SIGSTKSZ))==NULL){
        perror("stack alloc");
        exit(-1);
    }
    ss.ss_size = 65536;
    ss.ss_flags = 0;
    if(sigaltstack(&ss,0) < 0){
        perror("sigaltstack");
        exit(-1);
    }

    if(dummy1 == NULL){
        printf("dummy1 failed\n");
        exit(-2);
    }
    configure_handler(sig_handler); 
    init_vector_array();
    
    code_mem = mmap(NULL, 0x8000, PROT_READ | PROT_WRITE | PROT_EXEC,  MAP_ANON | MAP_PRIVATE, -1, 0);
    
    if(code_mem == MAP_FAILED) {
        perror("check cycle map failed");
        exit(-12);
    }

    /* nop_cycle = 300; */
    nop_cycle = check_cycle(0x00000013);
    printf("base cycle = %llu\n\n\n", nop_cycle);

    //dry run
    /* do { */
    /*     triage_Method = 123; */
    /*     stage1(); */
    /* } while(0); */

    asm volatile("fence.i \n");
    /* sleep(1); // why? sync? */

    while(fgets(line, 1024, input_file) != NULL) {

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
        swap_instr = __builtin_bswap32(current_instr);
        triage_Method = Normal;
        switch(triage_Method){
        
        case Normal: // General Undocumented Instruction Triaging
            printf("Undoc instr: %x\n", current_instr); // Undocumented Instruction Triaging
            check_tp();
            memcpy(instr_pointer+init_length, &current_instr, 4);
            instrBitsFlag |= STAGE1_flag;
            while((instrBitsFlag & 0x1F00) != 0x0){
                alarm(1);
                dummy_ptr_fp = dummy1;
                dummy_ptr_sp = dummy1+0x10;
                gpr_before[8] = (uint64_t)(uintptr_t)dummy_ptr_fp; //fp register
                gpr_before[1] = 0xdeadbeaf; //lr register
                gpr_before[2] = (uint64_t)(uintptr_t)dummy_ptr_sp; //sp register
                
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
                /* else if((instrBitsFlag & STAGE5_flag) == STAGE5_flag){ */
                /*     init_before_register(); */
                /*     #<{(| stage5(); |)}># */
                /* } */
            }
                alarm(0);
            if((instrBitsFlag & NOP_flag) == NOP_flag){
                adjust();
            }
            if((instrBitsFlag & NOP_flag) == NOP_flag){
                instr_cycle = check_cycle(current_instr);
               if(instr_cycle > nop_cycle){
                    instrBitsFlag &= 0x0;
                    instrBitsFlag |= SP_flag;
                    memset(code_mem, 0, sizeof(code_mem));
               }
            }
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

