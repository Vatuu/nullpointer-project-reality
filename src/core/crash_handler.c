#include <nusys.h>
#include <stdio.h>

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
    {CAUSE_EXCMASK, EXC_RMISS,   "TLB on load or instruction"},
    {CAUSE_EXCMASK, EXC_WMISS,   "TLB on store"},
    {CAUSE_EXCMASK, EXC_RADE,    "Addr err on load or instruction"},
    {CAUSE_EXCMASK, EXC_WADE,    "Addr err on store"},
    {CAUSE_EXCMASK, EXC_IBE,     "Bus err on instruction fetch"},
    {CAUSE_EXCMASK, EXC_DBE,     "Bus err on data reference"},
    {CAUSE_EXCMASK, EXC_SYSCALL, "System call exception"},
    {CAUSE_EXCMASK, EXC_BREAK,   "Breakpoint exception"},
    {CAUSE_EXCMASK, EXC_II,      "Reserved instruction"},
    {CAUSE_EXCMASK, EXC_CPU,     "Coprocessor unusable"},
    {CAUSE_EXCMASK, EXC_OV,      "Arithmetic overflow"},
    {CAUSE_EXCMASK, EXC_TRAP,    "Trap exception"},
    {CAUSE_EXCMASK, EXC_VCEI,    "Virtual coherency on intruction"},
    {CAUSE_EXCMASK, EXC_FPE,     "Floating point exception"},
    {CAUSE_EXCMASK, EXC_WATCH,   "Watchpoint exception"},
    {CAUSE_EXCMASK, EXC_VCED,    "Virtual coherency on data reference"},
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

static void debug_printreg(u16* buffer, u32 x, u32 y, u32 value, char *name, reg_desc *desc) {
    char first = 1;
    char str[128];
    char* cause;

    while(desc->mask != 0) {
        if((value & desc->mask) == desc->value) {
            cause = desc->string;
        }
        desc++;
    }

    sprintf(str, "%s | %08x | %s", name, (u8)value, cause);
    debug_draw_string(buffer, x, y, str);
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

        u16* buffer = nuGfxCfb[0];
        osViSetMode(&osViModeNtscLpn1);
        osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
        osViBlack(FALSE);
        osViSwapBuffer(buffer);

        // Get the faulted thread
        curr = (OSThread *)__osGetCurrFaultedThread();
        if(curr != NULL) {
            __OSThreadContext* context = &curr->context;

            // Print the basic info
            char str[128];
            sprintf(str, "Fault in thread: %d", curr->id);
            debug_draw_string(buffer, 5, 5, str);
            sprintf(str, "pc    | %08x", (u8)context->pc);
            debug_draw_string(buffer, 5, 15, str);

            debug_printreg(buffer, 5, 25, (u8)context->cause, "cause", causeDesc);
            debug_printreg(buffer, 5, 35, (u8)context->sr, "sr   ", srDesc);
            sprintf(str, "badvaddr | %08x", (u8)context->badvaddr);
            debug_draw_string(buffer, 5, 45, str);
                    
            // Print the registers
            sprintf(str, "at %08x | v0 %08x | v1 %08x", (u8)context->at, (u8)context->v0, (u8)context->v1);
            debug_draw_string(buffer, 5, 55, str);            
            sprintf(str, "a0 %08x | a1 %08x | a2 %08x", (u8)context->a0, (u8)context->a1, (u8)context->a2);
            debug_draw_string(buffer, 5, 65, str);
            sprintf(str, "a3 %08x | t0 %08x | t1 %08x", (u8)context->a3, (u8)context->t0, (u8)context->t1);
            debug_draw_string(buffer, 5, 75, str);
            sprintf(str, "t2 %08x | t3 %08x | t4 %08x", (u8)context->t2, (u8)context->t3, (u8)context->t4);
            debug_draw_string(buffer, 5, 85, str);
            sprintf(str, "t5 %08x | t6 %08x | t7 %08x", (u8)context->t5, (u8)context->t6, (u8)context->t7);
            debug_draw_string(buffer, 5, 95, str);
            sprintf(str, "s0 %08x | s1 %08x | s2 %08x", (u8)context->s0, (u8)context->s1, (u8)context->s2);
            debug_draw_string(buffer, 5, 105, str);
            sprintf(str, "s3 %08x | s4 %08x | s5 %08x", (u8)context->s3, (u8)context->s4, (u8)context->s5);
            debug_draw_string(buffer, 5, 115, str);
            sprintf(str, "s6 %08x | s7 %08x | t8 %08x", (u8)context->s6, (u8)context->s7, (u8)context->t8);
            debug_draw_string(buffer, 5, 125, str);
            sprintf(str, "t9 %08x | gp %08x | sp %08x", (u8)context->t9, (u8)context->gp, (u8)context->sp);
            debug_draw_string(buffer, 5, 135, str);
            sprintf(str, "s8 %08x | ra %08x", (u8)context->s8, (u8)context->ra);
            debug_draw_string(buffer, 5, 145, str);

            // Print the floating point registers
            debug_printreg(buffer, 5, 165, (u8)context->fpcsr, "fpcsr", fpcsrDesc);
            sprintf(str, "d0  %.4e | d2  %.4e | d4  %.4e", context->fp0.d,  context->fp2.d, context->fp4.d);
            debug_draw_string(buffer, 5, 175, str);
            sprintf(str, "d6  %.4e | d8  %.4e | d10 %.4e", context->fp6.d,  context->fp8.d, context->fp10.d);
            debug_draw_string(buffer, 5, 185, str);
            sprintf(str, "d12 %.4e | d14 %.4e | d16 %.4e", context->fp12.d,  context->fp14.d, context->fp16.d);
            debug_draw_string(buffer, 5, 195, str);
            sprintf(str, "d18 %.4e | d20 %.4e | d22 %.4e", context->fp18.d, context->fp20.d, context->fp22.d);
            debug_draw_string(buffer, 5, 205, str);
            sprintf(str, "d24 %.4e | d26 %.4e | d28 %.4e", context->fp24.d, context->fp26.d, context->fp28.d);
            debug_draw_string(buffer, 5, 215, str);
            sprintf(str, "d30 %.4e", context->fp30.d);
            debug_draw_string(buffer, 5, 225, str);

            osWritebackDCache(buffer, 2 * 320 * 32);
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