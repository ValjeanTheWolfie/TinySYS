%include "boot.inc"
org 0x10000

    ;Initialize the segment registers using the values in CS
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Print message
    mov bl, 0x07 
    mov cx, start_message_len
    mov ax, start_message
    mov bp, ax
    mov ah, 13H
    mov al, 01b 
    int 10h

  

    jmp $


; ==================
;    Data part
; ==================

    start_message db "This is loader!!!!!!!!!!!!", CR, LF
    start_message_len equ ($ - start_message)


