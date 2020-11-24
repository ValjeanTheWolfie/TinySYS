%include "boot.inc"
org 0x10000

loader_start:
    ;Initialize segment register DS
    mov ax, cs
    mov ds, ax


    mov si, message_enter_loader
    call fat12_print

    ; Open A20 line
    in al, 92h
    or al, 10b
    out 92h, al

    ;Switch to unreal mode, expanding the addressing of FS
    cli
    lgdt [Gdt32_Register]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    mov ax, Selector32_Data
    mov fs, ax

    mov eax, cr0
    and al, ~1
    mov cr0, eax
    sti

    mov si, message_in_unreal_mode
    call fat12_print

    ; Load kernel
  
    mov ax, kernel_filename
    call fat12_search_file_in_rootdir
    cmp ax, 0
    jnz .found_kernel

    mov si, message_file_not_found
    call fat12_print
    jmp $

.found_kernel:
    mov si, message_load_kernel
    call fat12_print

    mov ebx, 0x10_0000
    call fat12_load_file

    mov si, message_done
    call fat12_print

    jmp $



; ==================
;    Data part
; ==================
[SECTION descriptors]
%include "descriptor.inc"

    Gdt32:          GDT32_ENTRY(0, 0, 0, 0)
    Gdt32EntCode:   GDT32_ENTRY(GDT_BASE_ADDR_ZERO, GDT_LIMIT_UNLIMITED, GDT_ACCESS_BYTE_CODE, GDT_FLAGS_4K_32)
    Gdt32EntData:   GDT32_ENTRY(GDT_BASE_ADDR_ZERO, GDT_LIMIT_UNLIMITED, GDT_ACCESS_BYTE_DATA, GDT_FLAGS_4K_32)

    Gdt32Len equ $ - Gdt32
    Gdt32_Register dw Gdt32Len - 1
                   dd Gdt32
    Selector32_Code  equ  Gdt32EntCode - Gdt32
    Selector32_Data  equ  Gdt32EntData - Gdt32


    Gdt64:          GDT64_ENTRY(0, 0)
    Gdt64EntCode:   GDT64_ENTRY(GDT_ACCESS_BYTE_CODE, GDT_FLAGS_64)
    Gdt64EntData:   GDT64_ENTRY(GDT_ACCESS_BYTE_DATA, GDT_FLAGS_64)

    Gdt64Len equ $ - Gdt64
    Gdt64_Register dw Gdt64Len - 1
                   dd Gdt64
    Selector64_Code  equ  Gdt64EntCode - Gdt64
    Selector64_Data  equ  Gdt64EntData - Gdt64


[SECTION text]
    kernel_filename  db "KERNEL  BIN"

    message_enter_loader    db "Loader start!", CR, LF, 0
    message_in_unreal_mode  db "Switched to unreal mode.", CR, LF, 0
    message_load_kernel     db "Loading kernel", 0
    message_done            db "Done!!", CR, LF, 0
    message_file_not_found  db "KERNEL.BIN not found!!! System halt.", CR, LF, 0


[SECTION FAT12]
%include "FAT12.inc"
