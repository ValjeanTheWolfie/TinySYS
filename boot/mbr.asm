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
    mov sp, 0x7000


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
    mov si, start_message
    call print


    mov ax, loader_filename
    call fat12_search_file_in_rootdir
    cmp ax, 0
    jz loader_not_found

    mov bx, 0x1000
    mov fs, bx
    mov bx, 0x0000
    call fat12_load_file
    jmp 0x1000:0

loader_not_found:
    mov si, loader_not_found_message
    call print
    jmp $

; ==================
;    Data part
; ==================
    start_message db "Starting TinySYS...", CR, LF, 0
    loader_not_found_message db "loader.bin not found. System halt.", CR, LF, 0

    loader_filename  db "LOADER  BIN"




    times 510 - ($ - $$)db 0 ;Padding data
    dw 0xaa55                ;Bootable mark




