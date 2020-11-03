;======================================
;      FAT12 for 1.44M Floppy
;--------------------------------------
; 2020.10.30  ValjeanTheWolfie  Create
;======================================

; References
; Microsoft Extensible Firmware Initiative FAT32 File System Specification
; HardwareWhite Paper      
; https://msdn.microsoft.com/zh-cn/windows/hardware/gg463080


;================================
;   FAT12 file system info
;================================
    jmp near boot_start
    BS_OEMName      db  'Valjean '
    BPB_BytesPerSec dw  512
    BPB_SecPerClus  db  1
    BPB_RsvdSecCnt  dw  1
    BPB_NumFATs     db  2
    BPB_RootEntCnt  dw  224
    BPB_TotSec16    dw  2880
    BPB_Media       db  0xf0
    BPB_FATSz16     dw  9
    BPB_SecPerTrk   dw  18
    BPB_NumHeads    dw  2
    BPB_HiddSec     dd  0
    BPB_TotSec32    dd  0
    BS_DrvNum       db  0
    BS_Reserved1    db  0
    BS_BootSig      db  0x29
    BS_VolID        dd  0
    BS_VolLab       db  'TinySYS    '
    BS_FileSysType  db  'FAT12   '


;==========================================
; File-loading functions for FAT12 floppy
;==========================================

; Definitions
RootDirSectorCnt    equ    14 ; = (BPB_BytesPerSec * 32 + (BPB_BytesPerSec - 1)) / BPB_BytesPerSec
RootDirStartSector  equ    19 ; = BPB_RsvdSecCnt + BPB_FATSz16 * BPB_NumFATs
FAT1StartSector     equ    1  

FAT12ReadBufferAddr equ    0x8000
FAT12SectorSize     equ    512


; RAM Variables
dw_SectorInd        dw    0
dw_DataClusterInd   dw    0
db_char             db    0, 0


;--------------------------------------------
; Proc Name: fat12_read_floppy_sector
; Function : Read floppy sector
; Input    : AX    : start sector index
;            ES:BX : read buffer address
;            CL    : number of sectors to read
; Output   : none
;---------------------------------------------
fat12_read_floppy_sector:
;prepare_stack:
    push bp
    mov bp, sp
    sub esp, 4
    mov word [bp - 4], ax
    mov byte [bp - 2], cl
.loop:
; Covert LBA to CHS
; LBA = ((CylinderInd * HeadPerCylinder) + HeadInd) * SectorPerTrack + dw_SectorInd - 1
    mov ax, [bp - 4]
    push bx
    mov bl, [BPB_SecPerTrk]
    div bl
    mov cl, ah
    inc cl
    mov dh, al
    and dh, 1
    mov ch, al
    shr ch, 1
    pop bx
    mov dl, [BS_DrvNum]

    mov ah, 02h
    mov al, byte [bp - 2]
    int 13h

    jc .loop   ; Repeat until load success
;restore stack

    add esp, 4
    pop bp
    ret

;--------------------------------------------
; Proc Name: fat12_search_file_in_rootdir
; Function : Read floppy sector
; Input    : AX    : Filename str(11-byte format)
; Output   : AX = 0: Not found
;            else  : The first cluster number
;---------------------------------------------
fat12_search_file_in_rootdir:
;prepare_stack:
    push bp
    mov bp, sp
    sub esp, 2
    mov word [bp - 2], ax
; Search from the first root dir sector

    mov word [dw_SectorInd], RootDirStartSector
.load_one_sector:

    mov ax, [dw_SectorInd]
    mov bx, 0
    mov es, bx
    mov bx, FAT12ReadBufferAddr
    mov cl, 1 ;Load one sector
    call fat12_read_floppy_sector

    mov si, FAT12ReadBufferAddr
    cld
.compare_filename:

    mov di, [bp - 2]
    mov cx, 11
    repe cmpsb
    jne .compare_next_dir_entry 
; Found match
    and si, 0xFFF0
    mov ax, [si + 0x1A]
    jmp .return
.compare_next_dir_entry:

    and si, 0xFFE0
    add si, 0x20
    cmp si, FAT12ReadBufferAddr + FAT12SectorSize  ;Reach the sector end
    jb .compare_filename
    add word [dw_SectorInd], 1
    cmp word [dw_SectorInd], RootDirStartSector + RootDirSectorCnt
    jb .load_one_sector
; No match
    xor ax, ax
.return:
    add esp, 2
    pop bp
    ret

;--------------------------------------------
; Proc Name: fat12_get_next_fat12_cluster
; Function : Load file to 
; Input    : AX    : Data cluster index
; Output   : AX    : Next data cluster index
;---------------------------------------------
fat12_get_next_fat12_cluster:
    push ax
    mov ax, FAT1StartSector
    mov bx, FAT12ReadBufferAddr
    mov cx, 0
    mov es, cx
    mov cl, 2
    call fat12_read_floppy_sector
    pop ax

; BX = AX + (AX / 2) : covert FAT12 Entry index to addr
    mov bx, ax
    shr ax, 1
    add bx, ax 

    mov ax, [bx + FAT12ReadBufferAddr]
    test bx, 1
    jnz .odd
    shr ax, 4
.odd:   ;even number is OK to fall through, because it doesn't change anything
    and ax, 0x0fff
    ret

;--------------------------------------------
; Proc Name: fat12_load_file:
; Function : Load file to 
; Input    : AX    : The first data cluster index
;            EBX   : Address to load to
; Output   : none
;---------------------------------------------
fat12_load_file:
;prepare_stack:
    push bp
    mov bp, sp
    sub esp, 4
    mov dword [bp - 4], ebx
.load_one_cluster:

    push ax
    cmp ax, 0x0ff8
    jae .return
    
    add ax, RootDirStartSector + RootDirSectorCnt - 2
    mov ebx, [bp - 4]
    mov ecx, ebx
    shr ecx, 4
    and cx, 0xF000
    mov es, cx
    mov cl, 1
    call fat12_read_floppy_sector
    add dword [bp - 4], FAT12SectorSize

    pop ax
    call fat12_get_next_fat12_cluster
.return:
    add esp, 4
    pop bp
    ret



boot_start:
    nop