IO_PORT_CMD := 10H
IO_PORT_DATA := 11H

CMD_CHAR_EXISTS := 1
CMD_CHAR_READ := 2
CMD_CHAR_WRITE := 3

CMD_BLOCK_READ := 4
CMD_BLOCK_WRITE := 5
    
    JMP	BOOT	;-3: Cold start routine
	JMP	WBOOT	; 0: Warm boot - reload command processor
	JMP	CONST	; 3: Console status
	JMP	CONIN	; 6: Console input
	JMP	CONOUT	; 9: Console output
	JMP	LIST	;12: Printer output
	JMP	PUNCH	;15: Paper tape punch output
    JMP READER	;18: Paper tape reader input
	JMP	HOME	;21: Move disc head to track 0
	JMP	SELDSK	;24: Select disc drive
	JMP	SETTRK	;27: Set track number
	JMP	SETSEC	;30: Set sector number
	JMP	SETDMA	;33: Set DMA address
	JMP	READ	;36: Read a sector
	JMP	WRITE	;39: Write a sector
    ; In CP/M 2 and later, the following extra jumps appear:

	JMP	LISTST	;42: Status of list device
	JMP	SECTRAN	;45: Sector translation for skewing

BOOT:
    RET
WBOOT:
    RET
CONST:
    RET
CONIN:
    RET
CONOUT:
    push psw
    mvi a, CMD_CHAR_WRITE
    out IOPORT
    mov a, c
    out IOPORT
    pop psw
    RET
LIST:
    RET
PUNCH:
    RET
READER:
    RET
HOME:
    RET
SELDSK:
    RET
SETTRK:
    RET
SETSEC:
    RET
SETDMA:
    RET
READ:
    RET
WRITE:
    RET
LISTST:
    RET
SECTRAN:
    RET