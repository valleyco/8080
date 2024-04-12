BDOS equ 8000H
    ORG 0000H
    jmp BIOS_WBOOT
io:    db 0
dsk:   db 0 
    ORG 0005H
    jmp BDOS
    ORG 3000H
BIOS_WBOOT:
    LDA WARM_FLAG
    CPI 1
    JNZ L00
    HLT
L00:    
    MVI A,1
    STA WARM_FLAG
    JMP 100H
WARM_FLAG: 
    DB 0    