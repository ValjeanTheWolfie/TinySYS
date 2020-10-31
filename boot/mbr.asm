;======================================
;        Master Boot Record
;--------------------------------------
; 2020.10.30  ValjeanTheWolfie  Create
;======================================
%include "boot.inc"

org 0x7c00    


%include "FAT12.inc"

boot_start:
    ;Initialize the segment registers using the values in CS
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ;Initialize the stack pointer register
    mov sp, 0x7c00


    ; Clear screen
    mov ah, 06h
    mov al, 00h
    int 10h

    ; Reset cursor
    mov ax, 02h
    mov bx, 0
    mov dx, 0
    int 10h

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
    start_message db "Starting TinySYS...", CR, LF, 0
    start_message_len equ ($ - start_message - 1)




    times 510 - ($ - $$)db 0 ;Padding data
    dw 0xaa55                ;Bootable mark




