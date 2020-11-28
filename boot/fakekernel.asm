[bits 64]
mov al, 'K'
mov ah, 1_000_1_100b
mov word [0xb8000 + 60], ax

halt:
hlt
jmp halt
