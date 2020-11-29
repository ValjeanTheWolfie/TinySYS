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

load_kernel:
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

.kill_floppy_monitor:
    mov dx, 0x03f2
    mov al, 0
    out dx, al

dectect_memory:
    mov si, message_detecting_memory
    call fat12_print

    xor ebx, ebx
    mov di, 0x7000
.int_e820h:
    mov si, message_wait_dot
    call fat12_print

    mov eax, 0xE820
    mov ecx, 20
    mov edx, 0x534D4150    ;'SMAP'
    int 15h
    jc .error
    add di, 20
    cmp ebx, 0
    jnz .int_e820h
    jmp .done
.error:
    mov si, message_detecting_memory_error
    call fat12_print
    jmp $
.done:
    mov si, message_done
    call fat12_print

go_to_protected_mode:
    call prepare_print_without_bios

    cli
    lgdt [Gdt32_Register]
    lidt [Idt_Register]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword Selector32_Code: protected_mode_start

[bits 32]
protected_mode_start:
    mov ax, Selector32_Data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov esp, 0x7000
    mov ax, Selector32_Video
    mov gs, ax

    mov esi, message_in_protected_mode
    call print32

check_IA32_mode_supported:
; cpuid 80000000H 
; EAX Maximum Input Value for Extended Function CPUID Information.
; cpuid 80000001H 
; EDX Bit 29: Intel® 64 Architecture available if 1.
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .not_supported
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jnz .supported
.not_supported:
    mov esi, message_not_support_ia32
    call print32
    jmp $
.supported:
    mov esi, message_support_ia32
    call print32

    mov esi, message_go_to_long_mode
    call print32


prepare_page_table:
    mov dword [0x90000], 0x91007
    mov dword [0x90004], 0
    mov dword [0x90800], 0x91007
    mov dword [0x90804], 0

    mov dword [0x91000], 0x92007
    mov dword [0x91004], 0

    mov dword [0x92000], 0x000083
    mov dword [0x92004], 0
    mov dword [0x92008], 0x200083
    mov dword [0x9200c], 0
    mov dword [0x92010], 0x400083
    mov dword [0x92014], 0
    mov dword [0x92018], 0x600083
    mov dword [0x9201c], 0
    mov dword [0x92020], 0x800083
    mov dword [0x92024], 0
    mov dword [0x92028], 0xa00083
    mov dword [0x9202c], 0
    mov dword [0x92030], 0xc00083
    mov dword [0x92034], 0
    mov dword [0x92038], 0xe00083
    mov dword [0x9203c], 0

go_to_long_mode:
    ; load GDT
    lgdt [Gdt64_Register]
    mov ax, Selector64_Data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x7000

    ; open PAE
    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    ; load page table to cr3
    mov eax, 0x90000
    mov cr3, eax

    ; enable long mode
    mov ecx, 0xC0000080
    rdmsr
    bts eax, 8
    wrmsr

    ; open PE and paging
    mov eax, cr0
    bts eax, 0
    bts eax, 31
    mov cr0, eax

    jmp Selector64_Code: 0x10_0000



[SECTION FAT12]
[bits 16]
%include "FAT12.inc"

[SECTION text]
    kernel_filename  db "KERNEL  BIN"

    message_enter_loader            db "Loader start!", CR, LF, 0
    message_in_unreal_mode          db "Switched to unreal mode.", CR, LF, 0
    message_load_kernel             db "Loading kernel", 0
    message_file_not_found          db "KERNEL.BIN not found!!! System halt.", CR, LF, 0
    message_done                    db "Done!!", CR, LF, 0
    message_detecting_memory        db "Dectecting memory", 0
    message_detecting_memory_error  db "Error are encountered. System halt", CR, LF, 0
    message_in_protected_mode       db "Entered protected mode!!", CR, LF, 0
    message_support_ia32            db "CPU support IA-32e(64 bit) mode.", CR, LF, 0
    message_not_support_ia32        db "CPU doesn't support 64 bit mode. System halt!", CR, LF, 0
    message_go_to_long_mode         db "Switching to long mode...", CR, LF, 0
    message_wait_dot                db ".", 0
    message_endline                 db CR, LF, 0


[SECTION descriptors]
%include "descriptor.inc"

    Gdt32:          GDT32_ENTRY(0, 0, 0, 0)
    Gdt32EntCode:   GDT32_ENTRY(GDT_BASE_ADDR_ZERO, GDT_LIMIT_UNLIMITED, GDT_ACCESS_BYTE_CODE, GDT_FLAGS_4K_32)
    Gdt32EntData:   GDT32_ENTRY(GDT_BASE_ADDR_ZERO, GDT_LIMIT_UNLIMITED, GDT_ACCESS_BYTE_DATA, GDT_FLAGS_4K_32)
    Gdt32EntVideo:  GDT32_ENTRY(0xb8000, GDT_LIMIT_UNLIMITED, GDT_ACCESS_BYTE_DATA, GDT_FLAGS_4K_32)

    Gdt32Len equ $ - Gdt32
    Gdt32_Register dw Gdt32Len - 1
                   dd Gdt32
    Selector32_Code  equ  Gdt32EntCode - Gdt32
    Selector32_Data  equ  Gdt32EntData - Gdt32
    Selector32_Video equ  Gdt32EntVideo - Gdt32

    Idt:            times 0x50 dq 0
    
    IdtLen equ $ - Idt
    Idt_Register  dw IdtLen - 1
                  dd Idt


    Gdt64:          GDT64_ENTRY(0, 0)
    Gdt64EntCode:   GDT64_ENTRY(GDT_ACCESS_BYTE_CODE, GDT_FLAGS_64)
    Gdt64EntData:   GDT64_ENTRY(GDT_ACCESS_BYTE_DATA, GDT_FLAGS_64)

    Gdt64Len equ $ - Gdt64
    Gdt64_Register dw Gdt64Len - 1
                   dd Gdt64
    Selector64_Code  equ  Gdt64EntCode - Gdt64
    Selector64_Data  equ  Gdt64EntData - Gdt64


[SECTION print32]

DefaulPrintColor     equ 0x07     ; white
VideoMemPerLine      equ 160      ; 80 char/line * 2 byte/char

dw_CursorPos    dw 0                        ; cursor position

; ----------------------------------------------------------
;  Proc Name: prepare_print_without_bios
;  Function : As its name is
; ----------------------------------------------------------
[bits 16]
prepare_print_without_bios:
    ; Save cursor position
    ; (BIOS interrupt INT 10H / AH=03H: Read Cursor Position)
    ; (Return: DH = Y coordinate  DL =  X coordinate        )
    mov ah, 03h
    mov bh, 00h
    int 10h

    ; Covert to video memory offset
    xor eax, eax
    mov al, dh
    mov bl, 80   ; 80 char/line
    mul bl
    movzx bx, dl
    add ax, bx
    shl eax, 1  ; 2 byte/char
    mov dword [dw_CursorPos], eax

    ; Disable the cursor (BIOS interrupt INT 10H / AH=01H: Set Cursor Type)
    mov ah, 01h
    mov ch, 0010_0000b  ; bit5 set: no cursor
    int 10h

    ret

; ----------------------------------------------------------
;  Proc Name: print32
;  Function : Print a C-style string (ended with '\0').
;  Input    : ESI - the address of the string
;  Output   : void
; ----------------------------------------------------------
[bits 32]
print32:
    pusha
    mov ebx, [dw_CursorPos]
.loop:
    lodsb
    cmp al, 0
    jz .end
    call .putchar32
    jmp .loop
.end:
    mov dword [dw_CursorPos], ebx
    popa
    ret
.putchar32:
; SubProc: Print a char to the screen and update EBX
; Input  : AL - char   EBX - cursor position
; Output : EBX - updated cursor position
    cmp al, CR
    je .is_cr
    cmp al, LF
    je .is_lf
    mov byte [gs:ebx], al
    inc ebx
    mov byte [gs:ebx], DefaulPrintColor
    inc ebx
    ret
.is_cr:
    mov edx, 0
    mov ecx, VideoMemPerLine
    mov eax, ebx
    div ecx
    sub ebx, edx
    ret
.is_lf:
    add ebx, VideoMemPerLine
    ret




