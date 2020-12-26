#ifndef _TRAP_H_
#define _TRAP_H_

typedef struct _IdtInfo {
    void (*entranceAddr)(void);
    int accessByte;
    const char* printInfo;
} IdtInfo;

extern void initTss();
extern void initIdt();
extern void initInterrupt();

extern void handleIdtCall(int vectorID, unsigned long errorCode, unsigned long rip);
extern void handleFault(int vectorID, unsigned long errorCode, unsigned long rip);
extern void handleInterrupt(int vectorID);
extern void commonErrorCodeInfo(unsigned long errorCode);
extern void handlePageFault(unsigned long errorCode);
extern void controlProtectionExceptionInfo(unsigned long errorCode);


 
#endif