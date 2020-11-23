%include "boot.inc"
org 0x10000

%include "FAT12.inc"


    ;Initialize the segment registers using the values in CS
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax


    mov si, message_enter_loader
    call fat12_print

    
  

    jmp $


; ==================
;    Data part
; ==================
    message_enter_loader db "Enter loader.", CR, LF, 0


