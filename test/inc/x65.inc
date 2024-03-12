; macro that defines the extensions of the 65x02

; these are the byte register indices
.enum
    D0 = 0
    D1 = 1
    D2 = 2
    D3 = 3
    D4 = 4
    D5 = 5
    D6 = 6
    D7 = 7
    DA = D1
    DX = D3
    DY = D5
.endenum

; these are the word register indices
.enum
    W0 = 0
    W1
    W2
    W3
    W4
    W5
    W6
    W7
.endenum

; these are the long word register indices
.enum
    X0 = 0
    X1
    X2
    X3
    X4
    X5
    X6
    X7
.endenum

.enum
    XTOP1 = $f2
    XTOP2 = $f3
    XTOP3 = $f4
.endenum

; transfer byte register from s to d
; $f2 00 ddd sss
.macro trb dd, ds
    .byte XTOP1, (dd << 3) | ds
.endmacro

.macro trw dd, ds
    .byte XTOP2, (dd << 3) | ds
.endmacro

.macro trl dd, ds
    .byte XTOP3, (dd << 3) | ds
.endmacro

.macro addb dd, ds, con
    .if
.macro 


