#include <nusys.h>

#include "crash_handler.h"
#include "debug_font.h"
#include "debug.h"

static OSMesgQueue chMessageQueue;
static OSMesg chMessageBuf;
static OSThread chThread;
static u64 chStack[CRASH_HANDLER_STACK / sizeof(u64)];

static reg_desc causeDesc[] = {
    {CAUSE_BD,      CAUSE_BD,    "BD"},
    {CAUSE_IP8,     CAUSE_IP8,   "IP8"},
    {CAUSE_IP7,     CAUSE_IP7,   "IP7"},
    {CAUSE_IP6,     CAUSE_IP6,   "IP6"},
    {CAUSE_IP5,     CAUSE_IP5,   "IP5"},
    {CAUSE_IP4,     CAUSE_IP4,   "IP4"},
    {CAUSE_IP3,     CAUSE_IP3,   "IP3"},
    {CAUSE_SW2,     CAUSE_SW2,   "IP2"},
    {CAUSE_SW1,     CAUSE_SW1,   "IP1"},
    {CAUSE_EXCMASK, EXC_INT,     "Interrupt"},
    {CAUSE_EXCMASK, EXC_MOD,     "TLB modification exception"},
    {CAUSE_EXCMASK, EXC_RMISS,   "TLB exception on load or instruction fetch"},
    {CAUSE_EXCMASK, EXC_WMISS,   "TLB exception on store"},
    {CAUSE_EXCMASK, EXC_RADE,    "Address error on load or instruction fetch"},
    {CAUSE_EXCMASK, EXC_WADE,    "Address error on store"},
    {CAUSE_EXCMASK, EXC_IBE,     "Bus error exception on instruction fetch"},
    {CAUSE_EXCMASK, EXC_DBE,     "Bus error exception on data reference"},
    {CAUSE_EXCMASK, EXC_SYSCALL, "System call exception"},
    {CAUSE_EXCMASK, EXC_BREAK,   "Breakpoint exception"},
    {CAUSE_EXCMASK, EXC_II,      "Reserved instruction exception"},
    {CAUSE_EXCMASK, EXC_CPU,     "Coprocessor unusable exception"},
    {CAUSE_EXCMASK, EXC_OV,      "Arithmetic overflow exception"},
    {CAUSE_EXCMASK, EXC_TRAP,    "Trap exception"},
    {CAUSE_EXCMASK, EXC_VCEI,    "Virtual coherency exception on intruction fetch"},
    {CAUSE_EXCMASK, EXC_FPE,     "Floating point exception (see fpcsr)"},
    {CAUSE_EXCMASK, EXC_WATCH,   "Watchpoint exception"},
    {CAUSE_EXCMASK, EXC_VCED,    "Virtual coherency exception on data reference"},
    {0,             0,           ""}
    };

static reg_desc srDesc[] = {
    {SR_CU3,      SR_CU3,     "CU3"},
    {SR_CU2,      SR_CU2,     "CU2"},
    {SR_CU1,      SR_CU1,     "CU1"},
    {SR_CU0,      SR_CU0,     "CU0"},
    {SR_RP,       SR_RP,      "RP"},
    {SR_FR,       SR_FR,      "FR"},
    {SR_RE,       SR_RE,      "RE"},
    {SR_BEV,      SR_BEV,     "BEV"},
    {SR_TS,       SR_TS,      "TS"},
    {SR_SR,       SR_SR,      "SR"},
    {SR_CH,       SR_CH,      "CH"},
    {SR_CE,       SR_CE,      "CE"},
    {SR_DE,       SR_DE,      "DE"},
    {SR_IBIT8,    SR_IBIT8,   "IM8"},
    {SR_IBIT7,    SR_IBIT7,   "IM7"},
    {SR_IBIT6,    SR_IBIT6,   "IM6"},
    {SR_IBIT5,    SR_IBIT5,   "IM5"},
    {SR_IBIT4,    SR_IBIT4,   "IM4"},
    {SR_IBIT3,    SR_IBIT3,   "IM3"},
    {SR_IBIT2,    SR_IBIT2,   "IM2"},
    {SR_IBIT1,    SR_IBIT1,   "IM1"},
    {SR_KX,       SR_KX,      "KX"},
    {SR_SX,       SR_SX,      "SX"},
    {SR_UX,       SR_UX,      "UX"},
    {SR_KSU_MASK, SR_KSU_USR, "USR"},
    {SR_KSU_MASK, SR_KSU_SUP, "SUP"},
    {SR_KSU_MASK, SR_KSU_KER, "KER"},
    {SR_ERL,      SR_ERL,     "ERL"},
    {SR_EXL,      SR_EXL,     "EXL"},
    {SR_IE,       SR_IE,      "IE"},
    {0,           0,          ""}
};

static reg_desc fpcsrDesc[] = {
    {FPCSR_FS,      FPCSR_FS,    "FS"},
    {FPCSR_C,       FPCSR_C,     "C"},
    {FPCSR_CE,      FPCSR_CE,    "Unimplemented operation"},
    {FPCSR_CV,      FPCSR_CV,    "Invalid operation"},
    {FPCSR_CZ,      FPCSR_CZ,    "Division by zero"},
    {FPCSR_CO,      FPCSR_CO,    "Overflow"},
    {FPCSR_CU,      FPCSR_CU,    "Underflow"},
    {FPCSR_CI,      FPCSR_CI,    "Inexact operation"},
    {FPCSR_EV,      FPCSR_EV,    "EV"},
    {FPCSR_EZ,      FPCSR_EZ,    "EZ"},
    {FPCSR_EO,      FPCSR_EO,    "EO"},
    {FPCSR_EU,      FPCSR_EU,    "EU"},
    {FPCSR_EI,      FPCSR_EI,    "EI"},
    {FPCSR_FV,      FPCSR_FV,    "FV"},
    {FPCSR_FZ,      FPCSR_FZ,    "FZ"},
    {FPCSR_FO,      FPCSR_FO,    "FO"},
    {FPCSR_FU,      FPCSR_FU,    "FU"},
    {FPCSR_FI,      FPCSR_FI,    "FI"},
    {FPCSR_RM_MASK, FPCSR_RM_RN, "RN"},
    {FPCSR_RM_MASK, FPCSR_RM_RZ, "RZ"},
    {FPCSR_RM_MASK, FPCSR_RM_RP, "RP"},
    {FPCSR_RM_MASK, FPCSR_RM_RM, "RM"},
    {0,             0,           ""}
};

static void debug_printreg(u32 value, char *name, reg_desc *desc) {
    char first = 1;
    //debug_printf("%s\t\t0x%08x <", name, value);
    while(desc->mask != 0) {
        if((value & desc->mask) == desc->value) {
            //(first) ? (first = 0) : ((void)debug_printf(","));
            //debug_printf("%s", desc->string);
        }
        desc++;
    }
    //debug_printf(">\n");
}
        
static void debug_crash(void *arg) {
    OSMesg msg;
    OSThread *curr;

    // Create the message queue for the fault message
    osCreateMesgQueue(&chMessageQueue, &chMessageBuf, 1);
    osSetEventMesg(OS_EVENT_FAULT, &chMessageQueue, (OSMesg)MSG_CRASH);

    // Thread loop
    while(1) {
        // Wait for a fault message to arrive
        osRecvMesg(&chMessageQueue, (OSMesg *)&msg, OS_MESG_BLOCK);
                
        debug_printf("CRASH!");
        debug_draw_char(50, 50, 'H', 1);
        osViSwapBuffer((void *)FB_ADDRESS);

        // Get the faulted thread
        curr = (OSThread *)__osGetCurrFaultedThread();
        if(curr != NULL) {
            __OSThreadContext* context = &curr->context;

            // Print the basic info
            /*debug_printf("Fault in thread: %d\n\n", curr->id);
            debug_printf("pc\t\t0x%08x\n", context->pc);
            debug_printreg(context->cause, "cause", causeDesc);
            debug_printreg(context->sr, "sr", srDesc);
            debug_printf("badvaddr\t0x%08x\n\n", context->badvaddr);
                    
            // Print the registers
            debug_printf("at 0x%016llx v0 0x%016llx v1 0x%016llx\n", context->at, context->v0, context->v1);
            debug_printf("a0 0x%016llx a1 0x%016llx a2 0x%016llx\n", context->a0, context->a1, context->a2);
            debug_printf("a3 0x%016llx t0 0x%016llx t1 0x%016llx\n", context->a3, context->t0, context->t1);
            debug_printf("t2 0x%016llx t3 0x%016llx t4 0x%016llx\n", context->t2, context->t3, context->t4);
            debug_printf("t5 0x%016llx t6 0x%016llx t7 0x%016llx\n", context->t5, context->t6, context->t7);
            debug_printf("s0 0x%016llx s1 0x%016llx s2 0x%016llx\n", context->s0, context->s1, context->s2);
            debug_printf("s3 0x%016llx s4 0x%016llx s5 0x%016llx\n", context->s3, context->s4, context->s5);
            debug_printf("s6 0x%016llx s7 0x%016llx t8 0x%016llx\n", context->s6, context->s7, context->t8);
            debug_printf("t9 0x%016llx gp 0x%016llx sp 0x%016llx\n", context->t9, context->gp, context->sp);
            debug_printf("s8 0x%016llx ra 0x%016llx\n\n",            context->s8, context->ra);

            // Print the floating point registers
            debug_printreg(context->fpcsr, "fpcsr", fpcsrDesc);
            debug_printf("\n");
            debug_printf("d0  %.15e\td2  %.15e\n", context->fp0.d,  context->fp2.d);
            debug_printf("d4  %.15e\td6  %.15e\n", context->fp4.d,  context->fp6.d);
            debug_printf("d8  %.15e\td10 %.15e\n", context->fp8.d,  context->fp10.d);
            debug_printf("d12 %.15e\td14 %.15e\n", context->fp12.d, context->fp14.d);
            debug_printf("d16 %.15e\td18 %.15e\n", context->fp16.d, context->fp18.d);
            debug_printf("d20 %.15e\td22 %.15e\n", context->fp20.d, context->fp22.d);
            debug_printf("d24 %.15e\td26 %.15e\n", context->fp24.d, context->fp26.d);
            debug_printf("d28 %.15e\td30 %.15e\n", context->fp28.d, context->fp30.d);*/
        }
    }
}

void init_crash_handler() {
    osCreateThread(&chThread,
            CRASH_HANDLER_ID, 
            debug_crash, 
            0, 
            (chStack + CRASH_HANDLER_STACK / sizeof(u64)),
            OS_PRIORITY_APPMAX
    );
    osStartThread(&chThread);
}