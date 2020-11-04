;======================================
;        Master Boot Record
;--------------------------------------
; 2020.10.30  ValjeanTheWolfie  Create
;======================================
%include "boot.inc"

org 0x7c00    


%include "FAT12.inc"


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

    mov ax, loader_filename
    call fat12_search_file_in_rootdir

    mov bx, 0x1000
    mov fs, bx
    mov bx, 0x0000
    call fat12_load_file
    jmp 0x1000:0

; ==================
;    Data part
; ==================
    start_message db "Starting TinySYS...", CR, LF
    start_message_len equ ($ - start_message)

    loader_filename  db "LOADER  BIN"




    times 510 - ($ - $$)db 0 ;Padding data
    dw 0xaa55                ;Bootable mark




