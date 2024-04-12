BDOS	EQU	00005H	;BDOS ENTRY TO CP/M
WBOOT	EQU	00000H	;RE-ENTRY TO CP/M WARM BOOT
BDOS_C_WRITE_CH EQU 02H ; WRITE CHAR
BDOS_C_WRITESTR EQU 09H ; WRITE STRING


CMD_CHAR_EXISTS EQU 1
CMD_CHAR_READ EQU 2
CMD_CHAR_WRITE EQU 3
CMD_BLOCK_READ EQU 4
CMD_BLOCK_WRITE EQU 5
BIOS_PORT_CMD EQU 10H 
BIOS_PORT_DATA EQU 11H 

    ORG 8000H
BDOS_START:
    PUSH H
    PUSH PSW
    PUSH B
    PUSH D
    
    MOV A, C
    ADD C
    MOV C, A
    MVI B, 0
    LXI H, FN_TABLE
    DAD B
    MOV E,M
    INX H
    MOV D,M    
    XCHG
    POP D
    POP B
    POP PSW
    PCHL
BDOS_EXIT:    
    POP H
    RET
    
WRITE_CHAR:
    PUSH PSW
    MVI A,CMD_CHAR_WRITE
    OUT BIOS_PORT_CMD
    MOV A,E
    OUT BIOS_PORT_DATA
    POP PSW
    JMP BDOS_EXIT

WRITE_STR:
    PUSH PSW
    PUSH H
LOOP0:
    LDAX D
    CPI '$'
    JZ EXIT0
    MOV L,A
    MVI A, CMD_CHAR_WRITE
    OUT BIOS_PORT_CMD
    MOV A,L
    OUT BIOS_PORT_DATA
    INX D
    JMP LOOP0
EXIT0:
    POP H
    POP PSW
    JMP BDOS_EXIT

FN_TABLE:
    DW 0, 0, WRITE_CHAR, 0
    DW 0, 0, 0, 0
    DW 0, WRITE_STR
USER_SP:
    DS 1