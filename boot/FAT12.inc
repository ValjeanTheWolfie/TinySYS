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
FAT12SectorSize     equ    512 ; i.e. BPB_BytesPerSec

FAT12CommonBufferAddr     equ    0x7400
FAT12FatTableBufferAddr   equ    0x7000


;--------------------------------------------
; Proc Name: fat12_read_floppy_sector
; Function : Read floppy sector to 0:BX
; Input    : AX    : start sector index
;            0:BX  : Dest Buffer Address
;            CL    : number of sectors to read
; Output   : none
;---------------------------------------------
fat12_read_floppy_sector:
    %define w_startSector     bp - 2
    %define w_destBufferAddr  bp - 4
    %define b_readSectorsCnt  bp - 6
    enter 6, 0
    mov word [w_startSector], ax
    mov word [w_destBufferAddr], bx
    mov byte [b_readSectorsCnt], cl
.loop:
; Covert LBA to CHS
; LBA = ((CylinderInd * HeadPerCylinder) + HeadInd) * SectorPerTrack + SectorInd - 1
    mov ax, [w_startSector]
    mov bl, [BPB_SecPerTrk]
    div bl
    mov cl, ah
    inc cl
    mov dh, al
    and dh, 1
    mov ch, al
    shr ch, 1
    mov bx, 0
    mov es, bx
    mov bx, [w_destBufferAddr]
    mov dl, [BS_DrvNum]

    mov ah, 02h
    mov al, byte [b_readSectorsCnt]
    int 13h

    jc .loop   ; Repeat until load success

    leave
    ret

;--------------------------------------------
; Proc Name: fat12_search_file_in_rootdir
; Function : Read floppy sector
; Input    : AX    : Filename str(11-byte format)
; Output   : AX = 0: Not found
;            else  : The first cluster number
;---------------------------------------------
fat12_search_file_in_rootdir:
    %define w_filename    bp - 2
    %define w_sectorInd   bp - 4
    enter 4, 0
    mov word [w_filename], ax
; Search from the first root dir sector
    mov word [w_sectorInd], RootDirStartSector
.load_one_sector:
    mov ax, [w_sectorInd]
    mov bx, FAT12CommonBufferAddr
    mov cl, 1 ;Load one sector
    call fat12_read_floppy_sector

    mov si, FAT12FatTableBufferAddr
    cld
.compare_filename:
    mov di, [w_filename]
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
    cmp si, FAT12CommonBufferAddr + FAT12SectorSize  ;Reach the sector end
    jb .compare_filename
    add word [w_sectorInd], 1
    cmp word [w_sectorInd], RootDirStartSector + RootDirSectorCnt
    jb .load_one_sector
; No match
    xor ax, ax
.return:
    leave
    ret

;--------------------------------------------
; Proc Name: fat12_get_next_fat12_cluster
; Function : Find next FAT12 cluster according 
;            to the FAT12 table 
; Input    : AX    : Data cluster index
; Output   : AX    : Next data cluster index
;---------------------------------------------
w_loadedFatTableInd dw -1

fat12_get_next_fat12_cluster:
    %define w_clusterInd    bp - 2
    %define w_clusterAddr   bp - 4
    enter 4, 0

; AX = AX + (AX / 2) : covert FAT12 Entry index to addr
    mov word [w_clusterInd], ax ;backup to judge odd or even
    mov bx, ax
    shr bx, 1
    add ax, bx 

; Calculate the sector to load:
    xor dx, dx
    mov bx, FAT12SectorSize
    div bx
    mov word [w_clusterAddr], dx
    cmp ax, word [w_loadedFatTableInd]
    je .fat_table_loaded

    mov word [w_loadedFatTableInd], ax
    mov dx, ax
    add ax, FAT1StartSector
    mov bx, FAT12FatTableBufferAddr
    mov cl, 2
    call fat12_read_floppy_sector

.fat_table_loaded:
    mov bx, [w_clusterAddr]
    add bx, FAT12FatTableBufferAddr
    mov ax, [bx]
    test word [w_clusterInd], 1
    jz .even
    shr ax, 4
.even:   ;odd number is OK to fall through, because it doesn't change anything
    and ax, 0x0fff
    leave
    ret

;--------------------------------------------
; Proc Name: fat12_load_file:
; Function : Load file to the specific address
; Input    : AX    : The first data cluster index
;            FS:EBX: Destination address to load
; Output   : none
;---------------------------------------------
loading_file_message     db '.', 0
loading_complete_message db CR, LF, 0

fat12_load_file:
    %define w_cluster    bp - 2
    %define dw_destAddr  bp - 6
    enter 6, 0
    mov word [w_filename], ax
    mov dword [dw_destAddr], ebx
.load_one_cluster:
    mov si, loading_file_message
    call fat12_print

    mov word [w_cluster], ax
    cmp ax, 0x0ff8
    jae .return
    
    add ax, RootDirStartSector + RootDirSectorCnt - 2
    mov bx, FAT12CommonBufferAddr
    mov cl, 1
    call fat12_read_floppy_sector

    cld
    mov eax, 0
    mov ds, ax
    mov si, FAT12CommonBufferAddr
    mov ebx, [dw_destAddr]
    mov ecx, FAT12SectorSize
.copy_from_buffer_to_destination:
    lodsb
    mov byte [fs:ebx], al
    inc ebx
    loop .copy_from_buffer_to_destination
    mov dword [dw_destAddr], ebx

    mov ax, [w_cluster]
    call fat12_get_next_fat12_cluster
    jmp .load_one_cluster
.return:
    mov si, loading_complete_message
    call fat12_print

    leave
    ret
;--------------------------------------------
; Proc Name: fat12_print
; Function : print C-style string
; Input    : DS:SI : String address
; Output   : none
;---------------------------------------------
fat12_print:
    pusha
    mov ah, 0x0e
    mov bx, 0x07
.loop:
    lodsb
    cmp al, 0
    jz .done
    int 10h
    jmp .loop
.done:
    popa
    ret


boot_start:
