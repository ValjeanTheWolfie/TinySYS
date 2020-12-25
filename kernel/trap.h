#ifndef _TRAP_H_
#define _TRAP_H_

extern void initTss();
extern void initIdt();
extern void handleIdtCall(int vectorID, unsigned long errorCode, unsigned long rip);
extern void commonErrorCodeInfo(unsigned long errorCode);
extern void handlePageFault(unsigned long errorCode);
extern void controlProtectionExceptionInfo(unsigned long errorCode);

typedef struct _IdtEntryInfo {
    void* entryAddr;
    const char* printInfo;
    unsigned char accessByte;
} IdtEntryInfo;

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


 
#endif