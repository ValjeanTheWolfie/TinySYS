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


extern void idt_ent_0 ();
extern void idt_ent_1 ();
extern void idt_ent_2 ();
extern void idt_ent_3 ();
extern void idt_ent_4 ();
extern void idt_ent_5 ();
extern void idt_ent_6 ();
extern void idt_ent_7 ();
extern void idt_ent_8 ();
extern void idt_ent_9 ();
extern void idt_ent_10();
extern void idt_ent_11();
extern void idt_ent_12();
extern void idt_ent_13();
extern void idt_ent_14();
extern void idt_ent_16();
extern void idt_ent_17();
extern void idt_ent_18();
extern void idt_ent_19();
extern void idt_ent_20();
extern void idt_ent_21();

extern void idt_ent_0x20();
extern void idt_ent_0x21();
extern void idt_ent_0x22();
extern void idt_ent_0x23();
extern void idt_ent_0x24();
extern void idt_ent_0x25();
extern void idt_ent_0x26();
extern void idt_ent_0x27();
extern void idt_ent_0x28();
extern void idt_ent_0x29();
extern void idt_ent_0x2a();
extern void idt_ent_0x2b();
extern void idt_ent_0x2c();
extern void idt_ent_0x2d();
extern void idt_ent_0x2e();
extern void idt_ent_0x2f();

 
#endif