BDOS EQU 5
    ORG 100H
    MVI C,2
    MVI E,'\n'
    CALL BDOS
    MVI C,9
    LXI D, MSG1
    CALL BDOS
    LXI D, MSG2
    CALL BDOS
    MVI C,2
    MVI E,'\n'
    CALL BDOS
    HLT

MSG1:    db 'Hello World\n$'
MSG2:    db 'It\'s a wonderfull day\n$'