#ifndef _CC_CRASHHANDLER_
#define _CC_CRASHHANDLER_

#define CRASH_HANDLER_STACK 0x2000
#define CRASH_HANDLER_ID 14
#define MSG_CRASH 0x10

typedef struct {
    u32 mask;
    u32 value;
    char *string;
} reg_desc;

static void debug_crash(void *arg);

extern void init_crash_handler();

#endif